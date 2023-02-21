
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <math.h>

#define MAX_SOURCE_SIZE (0x100000)

using namespace std;
//======================================================================================================================
//----------------------------------Helper functions--------------------------------------------------------------------
//======================================================================================================================

//matrix init for both sequential and openCL version
void initializeMatrix(int SIZE, double* M)
{
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (i == 0 || i == SIZE-1 || j == 0 || j == SIZE-1) {
				M[i*SIZE+j]	= 0.5;
			} else if (i == 1 || i == SIZE-2 || j == 1 || j == SIZE-2) {
				M[i*SIZE+j] = 0.2;
			} else {
				M[i*SIZE+j] = 0.0;
			}
		//printf("%f ", M[i*SIZE+j]);
		}
	//printf("\n");
	}
}
//======================================================================================================================
//----------------------------------Helper functions--------------------------------------------------------------------
//======================================================================================================================
bool compare( double* M, double* tmp_M,int size){

    for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {

				double dif = fabs(M[i*size+j] - tmp_M[i*size+j]);
				// printf("%d",dif);
				if (dif > 0.99)
				{
					printf("%d\n",dif);
					return false;
				}
		}     
    }
    return true;
}
//======================================================================================================================
//----------------------------------Sequential code---------------------------------------------------------------------
//======================================================================================================================
void serial_exec(double** M, double** tmp_M, int size)
{
    double* ptr;
    for (int i = 2; i < size-2; i++) {
		for (int j = 2; j < size-2; j++) {
			(*tmp_M)[i*size+j] = sqrt((*M)[(i-1)*size+(j-1)]+(*M)[(i-1)*size+(j+1)]+(*M)[(i+1)*size+(j-1)]+(*M)[(i+1)*size+(j+1)])*sqrt((*M)[(i)*size+(j-2)]+(*M)[(i)*size+(j+2)]+(*M)[(i-2)*size+(j)]+(*M)[(i+2)*size+(j)]);
			// printf("%f ", (*tmp_M)[i*size+j]);
        }
		// printf("\n");
    }

	ptr = *M;
	*M = *tmp_M;
	*tmp_M = ptr;
}


int main(int argc, char *argv[])
{	

	//total iterations
	int iterations = 100;
	//matrix size
	int SIZE = 8192; // 2048, 4096, 8192


	// Allocate memories for input arrays and output array.
	double *A = (double *)malloc(sizeof(double) * SIZE * SIZE); 
	double *B = (double *)malloc(sizeof(double) * SIZE * SIZE); 

	//For OpenCl version
	double *tmp_M = (double *)malloc(sizeof(double)*SIZE*SIZE);
	
    // -----------  OpenCl Code  ---------- 
    chrono::high_resolution_clock::time_point open_t1 = chrono::high_resolution_clock::now(); 
	// Load kernel from file kernel.cl
	FILE *kernelFile;
	char *kernelSource;
	size_t kernelSize;

	kernelFile = fopen("kernel.cl", "r");

	if (!kernelFile)
	{
		fprintf(stderr, "No file named kernel.cl was found\n");
		exit(-1);
	}
	kernelSource = (char *)malloc(MAX_SOURCE_SIZE);
	kernelSize = fread(kernelSource, 1, MAX_SOURCE_SIZE, kernelFile);
	fclose(kernelFile);


	cl_mem aMemObj;
	cl_mem bMemObj;
	// Getting platform and device information
	cl_platform_id platformId ;
	cl_device_id deviceID;
	cl_int ret;
	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel kernel;

	initializeMatrix(SIZE, A);
    initializeMatrix(SIZE, B);
	cout<< "Matrix initialization..." << endl;
	cout<< "Done !!" << endl;

	ret = clGetPlatformIDs(1, &platformId, NULL);
	ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceID, NULL);

	// Creating context.
	context = clCreateContext(0, 1, &deviceID, NULL, NULL, &ret);

	// Creating command queue
	commandQueue = clCreateCommandQueue(context, deviceID, 0, &ret);

	// Create program from kernel source
	program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, (const size_t *)&kernelSize, &ret);

	// Build program
	clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	// Create kernel
	kernel = clCreateKernel(program, "MatriceTransformation", &ret);

	// Memory buffers for each array
	aMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE * SIZE * sizeof(double), NULL, NULL);
	bMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE * SIZE * sizeof(double), NULL, NULL);


	// Copy lists to memory buffers
	ret = clEnqueueWriteBuffer(commandQueue, aMemObj, CL_TRUE, 0, SIZE * SIZE * sizeof(double), A, 0, NULL, NULL);
	ret |= clEnqueueWriteBuffer(commandQueue, bMemObj, CL_TRUE, 0, SIZE * SIZE * sizeof(double), B, 0, NULL, NULL);

	size_t globalItemSize = SIZE * SIZE; // number of work items 	
	size_t localItemSize = 64;  // workgroup size

	// Set arguments of the kernel 
	ret  =  clSetKernelArg(kernel, 0, sizeof(cl_mem), &aMemObj);
	ret |=  clSetKernelArg(kernel, 1, sizeof(cl_mem), &bMemObj);
	ret |= clSetKernelArg(kernel, 2, sizeof(int), &SIZE);
	// Execute the kernel
	for (int i = 0; i < iterations; ++i)
	{
		ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalItemSize, &localItemSize, 0, NULL, NULL);
		// make the permutations 
		cl_mem ptr = aMemObj;
		aMemObj = bMemObj;
		bMemObj = ptr;
		 // update arguments of the kernel 
        ret  =  clSetKernelArg(kernel, 0, sizeof(cl_mem), &aMemObj);
        ret |=  clSetKernelArg(kernel, 1, sizeof(cl_mem), &bMemObj);

	}

	clFinish(commandQueue);
	// Read from device back to host.
	ret = clEnqueueReadBuffer(commandQueue, aMemObj, CL_TRUE, 0, SIZE * SIZE * sizeof(double), tmp_M, 0, NULL, NULL);


	chrono::high_resolution_clock::time_point open_t2 = chrono::high_resolution_clock::now();

    chrono::duration<double> time_diff_OpenCl = chrono::duration_cast<chrono::duration<double>>(open_t2 - open_t1);
     printf("OpenCL code execution finished!\n"); 
	cout << "OpenCl execution time: " << time_diff_OpenCl.count() <<  " seconds"<< endl;
    cout << endl;  //taking end time of OpenCL execution

	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
		serial_exec(&A, &B, SIZE);
	}
	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
    chrono::duration<double> time_diff_serial = chrono::duration_cast<chrono::duration<double>>(t2 - t1);

	cout << "Serial execution time: " << time_diff_serial.count() <<  " seconds"<< endl;
    cout << endl; 

    printf("Verifying results: "); 
    if (compare(A, tmp_M, SIZE))
    {
        printf(" SAME !! \n");
    }else {
        printf("NOT THE SAME :( \n");
    }

	
	// Clean up, release memory
	clReleaseCommandQueue(commandQueue);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(aMemObj);
	clReleaseMemObject(bMemObj);
	clReleaseContext(context);
	free(A);
	free(B);
	free(tmp_M);

	return 0;
}