//DON'T MODIFY THIS FUNCTION
void naive(int *src, int *dst, int SIZE)
{
    for(int i=0; i < SIZE; i++)
        for(int j=0; j < SIZE; j++)
            dst[(j * SIZE) + i] = src[(i * SIZE) + j];
}

//DON'T MODIFY THE HEADER OF THE FUNCTION
void aware(int *src, int *dst, int SIZE)
{
    
    for (int i = 0; i < SIZE; i += 64) {
        for (int j = 0; j < SIZE; j += 64) {
            for (int k = i; k < i + 64; k++) {
                for (int l = j; l < j + 64; l++) {
                    dst[(l * SIZE) + k] = src[(k * SIZE) + l];
                }
            }
        }
    } 
    // printf("\ncache-aware version not implemented...\n");    
}

int Compact1By1(int x)
{
  printf("%d\n",x);  
  x &= 0x55555555;                  // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
  printf("%d\n",x);
  x = (x ^ (x >>  1)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
  printf("%d\n",x);
  x = (x ^ (x >>  2)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
  printf("%d\n",x);
  x = (x ^ (x >>  4)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
  printf("%d\n",x);
  x = (x ^ (x >>  8)) & 0x0000ffff; // x = ---- ---- ---- ---- fedc ba98 7654 3210
  printf("%d\n",x);
  return x;
}

int Compact1By1Y(int y){
    
    y >>= 1; 
    y &= 0x55555555;                  // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
    printf("%d\n",y);
    y = (y ^ (y >>  1)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
    printf("%d\n",y);
    y = (y ^ (y >>  2)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
    printf("%d\n",y);
    y = (y ^ (y >>  4)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
    printf("%d\n",y);
    y = (y ^ (y >>  8)) & 0x0000ffff; // x = ---- ---- ---- ---- fedc ba98 7654 3210
    printf("%d\n",y);
    return y;
    

    
}

int DecodeMorton2X(int code)
{
  return Compact1By1(code >> 0);
}

int DecodeMorton2Y(int code)
{
  return Compact1By1(code >> 1);
}

//DON'T MODIFY THE HEADER OF THE FUNCTION
void oblivious(int *src, int *dst, int SIZE)
{       
   // oblivious cache version 
   // matrix transposition recursive function
    // printf("\nocbivious cache version not implemented...\n");
    //  int x = 0;
    // int y = 0;
    // int temp = 0; 
    // for(int z=0; z < SIZE*SIZE; z + 64){
    //   x = Compact1By1(z >> 0);
    //   y = Compact1By1(z >> 1);
      
    //   printf("%d \n x= ",x);

    //   temp = x * SIZE + y ;
    //   for (int j= temp; j< temp + 64 ; j++){
        
    //         dst[(z * SIZE) + x] = src[(x * SIZE) + y];
        
    //   }
        
    // }
    

    


    
   

    // printf("\nocbivious version not implemented...\n");
}
uint32_t calcZOrder(uint16_t xPos, uint16_t yPos)
{
    static const uint32_t MASKS[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const uint32_t SHIFTS[] = {1, 2, 4, 8};

    uint32_t x = xPos;  // Interleave lower 16 bits of x and y, so the bits of x
    uint32_t y = yPos;  // are in the even positions and bits from y in the odd;

    x = (x | (x << SHIFTS[3])) & MASKS[3];
    x = (x | (x << SHIFTS[2])) & MASKS[2];
    x = (x | (x << SHIFTS[1])) & MASKS[1];
    x = (x | (x << SHIFTS[0])) & MASKS[0];

    y = (y | (y << SHIFTS[3])) & MASKS[3];
    y = (y | (y << SHIFTS[2])) & MASKS[2];
    y = (y | (y << SHIFTS[1])) & MASKS[1];
    y = (y | (y << SHIFTS[0])) & MASKS[0];

    const uint32_t result = x | (y << 1);
    return result;
}
    

//DON'T MODIFY THE HEADER OF THE FUNCTION
void optimized(int *src, int *dst, int SIZE)
{
//  traverse the src matrix in row major order block by block 
// user the z curve order to determine the order of which the src matrix blocks are visited 
// copy the src matrix blocks to the dst matrix in the order determined by the z curve order
// printf("\noptimized version not implemented...\n");
 
    
     

}


    
s