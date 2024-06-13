#include "complex.h"
#include "mandelbrot.h"

#define BLOCK_SIZE	16
#define X(x)	minX+(maxX-minX)/resolution*x
#define Y(y)	minY+(maxY-minY)/resolution*y

__global__ void gpu_mandelbrot(unsigned *uvm_mandelbrot) {
	// get x and y values of thread
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	// get index of device memory by x and y values
	int idx = y * resolution + x;

	if(x < resolution && y < resolution) {
		// complex number on complex plain with (-2.2, -1.5), (1.0, 1.6)
		Complex<float> c(X(x), Y(y));
		// complex number Zn for find iteration count, and Z0 = c
		Complex<float> z = c;

		// iteration count 
		int itr_cnt = 0;

		// find iteration count
		for(itr_cnt = 0; itr_cnt < max_iterations; itr_cnt++) {
			// if magnitude of Z^2 overs escape(=4.0), exit loop and get iteration cnt
			if(z.magnitude2() > escape)
				break;
			// find Zn by Z = Z^2 + C
			z = z * z + c;
		}
		// set iteraction count number to device memory with idx
		uvm_mandelbrot[idx] = itr_cnt;	
	}
}

void calc_mandelbrot(void) {
    	/* Assignment 5: GPU Programming */
	// Copies for 'mandelbrot'
	unsigned* uvm_mandelbrot;

	// Allocate Device Memory
	size_t display_size = resolution * resolution * sizeof(unsigned);
	cudaMalloc(&uvm_mandelbrot, display_size);

	// Define threads and thread blocks
	dim3 threadsPerBlock(BLOCK_SIZE, BLOCK_SIZE);
	dim3 numBlocks(resolution / BLOCK_SIZE, resolution / BLOCK_SIZE);

	// Launch Cuda Kernel 
	gpu_mandelbrot<<<numBlocks, threadsPerBlock>>>(uvm_mandelbrot);

	// Wait for Device to complete kernel task
	cudaDeviceSynchronize();

	// Copy the result of kernel
	cudaMemcpy(mandelbrot, uvm_mandelbrot, display_size, cudaMemcpyDeviceToHost);

	// Free Device Memory
	cudaFree(uvm_mandelbrot);
}

