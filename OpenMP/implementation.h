#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define LEN 51200

unsigned long long llcs_serial(const char *X, const char *Y, unsigned int **M)
{
    unsigned long long entries_visited = 0;

    int i = 0;
    while (i < LEN)
    {
        int j = 0;
        while (j < LEN)
        {
            if (X[i] == Y[j]) {
                M[i + 1][j + 1] = M[i][j] + 1;
            } else if (M[i + 1][j] < M[i][j + 1]) {
                M[i + 1][j + 1] = M[i][j + 1];
            } else {
                M[i + 1][j + 1] = M[i + 1][j];
            }

            entries_visited++;

            j++;
        }
        i++;

    }
    
     
 

    

    return entries_visited;
}

#if defined(_OPENMP)

unsigned long long llcs_parallel_tasks(const char *X, const char *Y, unsigned int **M) 
{

    unsigned long long entries_visited = 0;
    int slice;
    int z ;
    int step;
    int length = LEN;
    // setting the number of threads to 16 
    int threadNumber = 16;
    
    // starting the parallel region 
    #pragma omp parallel 
    // creating the master thread 
    #pragma omp single
    {
          
            for (slice = 0; slice < 2 * length - 1; slice++) 
            {
                
                int i,j;
                i = (slice >= length) ? length - 1 : slice ;
                j = (slice >= length) ? slice - length + 1 : 0 ;
                int diagonal_length = (i - j + 1);
                // grouping the tasks created for each diagonal iteration 
                    #pragma omp taskgroup
                    {
                        for (int threadId = 0; threadId < threadNumber; threadId++)
                        {
                            // calculating the indices of each block 
                            int blockSize = diagonal_length / threadNumber; 
                            int blockStart = threadId * blockSize;
                            int blockEnd = (threadId == threadNumber - 1) ? diagonal_length : (threadId + 1) * blockSize;
                            // initializing the local entries visied
                            int local_entries = 0;
                            int ii ;
                            int jj ;
                            // each thread task has it own local copy of i,j,blockStart, blockEnd which are initialize before
                            #pragma omp task firstprivate(i,j,blockStart,blockEnd) 
                            {
                                for (int step = blockStart; step < blockEnd; step++) 
                                {
                                    int ii = i - step;
                                    int jj = j + step;

                                    if (X[ii] == Y[jj]) {
                                        M[ii + 1][jj + 1] = M[ii][jj] + 1;
                                    } else if (M[ii + 1][jj] < M[ii][jj + 1]) {
                                        M[ii + 1][jj + 1] = M[ii][jj + 1];
                                    } else {
                                        M[ii + 1][jj + 1] = M[ii + 1][jj];
                                    }
                                    // updating the local variable 
                                    local_entries++;
                                }
                                // making sure only one thread does this operation
                                #pragma omp atomic 
                                entries_visited += local_entries;
                            }
                        }
                            
                    }
                            
            }
    }
    
    return entries_visited;
}


unsigned long long llcs_parallel_taskloop(const char *X, const char *Y, unsigned int **M)
{
    
    unsigned long long entries_visited = 0;
    int slice;
    int length = LEN;
    omp_set_num_threads(16);
    
    for (slice = 0; slice < 2 * length - 1; slice++) 
    {
        
        int i,j;
        i = (slice >= length) ? length - 1 : slice ;
        j = (slice >= length) ? slice - length + 1 : 0 ;
        int diagonal_length = (i - j + 1);
        // starting the parallel region 
            #pragma omp parallel 
            // creating the master threads that invokes the tasks 
            #pragma omp single
            {
                // grouping the tasks of each diagonal iteration 
                #pragma omp taskgroup
                {
                    #pragma omp taskloop num_tasks(diagonal_length) firstprivate(i,j) 
                    for (int step = 0; step < diagonal_length; step++) 
                    {
                        int ii = i - step;
                        int jj = j + step;

                        if (X[ii] == Y[jj]) {
                            M[ii + 1][jj + 1] = M[ii][jj] + 1;
                        } else if (M[ii + 1][jj] < M[ii][jj + 1]) {
                            M[ii + 1][jj + 1] = M[ii][jj + 1];
                        } else {
                            M[ii + 1][jj + 1] = M[ii + 1][jj];
                        }
                    }
                    // updating the visited entries 
                    entries_visited+=diagonal_length;   
                }      
            } 
    }

    
    return entries_visited;
}

#endif