__kernel void MatriceTransformation(__global double *M, 
                                    __global double *tmp_M, 
                                    int SIZE) {

  int g_id = get_global_id(0);
  int i = g_id / SIZE;
  int j = g_id % SIZE;
  if (i > 1 && i < SIZE - 2 && j > 1 && j < SIZE - 2) {

    tmp_M[i*SIZE+j] = sqrt(M[(i-1)*SIZE+(j-1)]+M[(i-1)*SIZE+(j+1)]+M[(i+1)*SIZE+(j-1)]+M[(i+1)*SIZE+(j+1)])*
                      sqrt(M[(i)*SIZE+(j-2)]+M[(i)*SIZE+(j+2)]+M[(i-2)*SIZE+(j)]+M[(i+2)*SIZE+(j)]);
  }
}

