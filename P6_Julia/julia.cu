//
// GPU Julia set application from Sanders and Kandrot (p. 54)
//

#include <stdio.h>
//#include <cutil.h>
#include "cpu_bitmap.h"

#define DIM 1000

struct cuComplex {
   float r;
   float i;
   __device__ cuComplex( float a, float b) : r(a), i(b) {}
   __device__ float magnitude1(void) {
      return r + i;
   }
   __device__ float magnitude2(void) {
      return r*r + i*i;
   }
   __device__ float magnitude3(void) {
      return r*r*r + i*i*i;
   }
   __device__ cuComplex operator*(const cuComplex& a) {
      return cuComplex(r*a.r - i*a.i, i*a.r + r*a.i);
   }
   __device__ cuComplex operator+(const cuComplex& a) {
      return cuComplex(r+a.r, i+a.i);
   }
   __device__ float braunl() { 
      return magnitude3() * magnitude1();
   }
};

__device__ int julia(int x, int y) {
   const float scale = 1.5;
   float jx = scale * (float) (DIM / 2 - x) / (DIM / 2);
   float jy = scale * (float) (DIM / 2 - y) / (DIM / 2);

   cuComplex c(-0.8, 0.156); // #1 julia
//   cuComplex c(-0.6, 0); // #2 julia2
//   cuComplex c(-0.123, 0.745); // #3 Douady's rabbit fractal - julia3
//   cuComplex c(-0.391, -0.587); // #2 Siegel's disk fractal - julia4
   cuComplex a(jx, jy);

   int i = 0;
   for (i = 0; i < 200; i++) {
      a = a * a + c;
      if (a.magnitude2() > 4) // original
//      if (a.magnitude3() > 1000) // #2
//      if (a.braunl() > 1000) // #3 Braunl function
         return 0;
   }

   return (int) a.magnitude2()%8;
}

__device__ int julia_value(int offset, int value) {
    if (value == 0) {
        if (offset * 4 + 0 == 0 || 
            offset * 4 + 0 == 2 ||
            offset * 4 + 0 == 3 ||
            offset * 4 + 0 == 5 ) {
            return 255;
        } else { return 0; }
    } else if (value == 1) {
        if (offset * 4 + 1 == 0 || 
            offset * 4 + 1 == 1 ||
            offset * 4 + 1 == 3 ||
            offset * 4 + 1 == 7 ) {
            return 255;
        } else { return 0; }
    } else if (value == 2) {
        if (offset * 4 + 2 == 0 || 
            offset * 4 + 2 == 1 ||
            offset * 4 + 2 == 5 ||
            offset * 4 + 2 == 6 ) {
            return 255;
        } else { return 0; }
    }
    return 0;
}

__global__ void kernel(unsigned char *ptr) {
   // map from threadIdx/blockIdx to pixel position
   int x = blockIdx.x;
   int y = blockIdx.y;
   int offset = x + y * gridDim.x;

   // now calculate the value at that position
   int juliaValue = julia(x, y);
   ptr[offset * 4 + 0] = julia_value(juliaValue, 0);
   ptr[offset * 4 + 1] = julia_value(juliaValue, 1);
   ptr[offset * 4 + 2] = julia_value(juliaValue, 2);
   ptr[offset * 4 + 3] = 255;
}

int main(void) {
   CPUBitmap bitmap(DIM, DIM);
   unsigned char *dev_bitmap;

   cudaMalloc((void**)&dev_bitmap, bitmap.image_size());
   dim3 grid(DIM, DIM);
   kernel<<<grid,1>>>(dev_bitmap);
   cudaMemcpy(bitmap.get_ptr(), dev_bitmap, bitmap.image_size(), cudaMemcpyDeviceToHost);
   bitmap.display_and_exit();
   cudaFree(dev_bitmap);
}
