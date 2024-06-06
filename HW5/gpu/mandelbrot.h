#ifndef __MANDELBROT_H__
#define __MANDELBROT_H__

#define max_iterations 2048     // Max number of iterations
#define resolution     1024     // Mandelbrot window resolution
#define escape         4.0      // Mandelbrot escape value
#define minX          -2.2      // Min X
#define maxX           1.0      // Max X
#define minY          -1.5      // Min Y
#define maxY           1.6      // Max Y

// Mandelbrot convergence map
extern unsigned mandelbrot[resolution * resolution];

// Calculate the Mandelbrot convergence map.
void calc_mandelbrot();

#endif

