#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include "data.h"

#define LEN	1024

void pre_calculate_weights(std::complex<double>* weight, unsigned N) {
	double angle;

	for(unsigned n = 0; n < N / 2; n++) {
		angle = (2 * n * M_PI) / N;
		weight[n] = std::complex<double>(cos(angle), -sin(angle));
	} 
}


template <typename T>
void real_2_complex(T* data, std::complex<double>* x) {
    for (unsigned i = 0; i < LEN; ++i) {
        if constexpr (std::is_same_v<T, std::complex<double>>) {
            x[i] = data[i];
        } else {
            x[i] = std::complex<double>(data[i], 0.0);
        }
    }
}


void dft1d(std::complex<double>* data, 
		const unsigned width, std::complex<double>* weight) {
	if(width == 1)
		return;

	std::complex<double>* odd = new std::complex<double>[width / 2];
	std::complex<double>* even = new std::complex<double>[width / 2];

	for(unsigned k = 0; k < (width / 2); k++) {
		even[k] = data[2*k];
		odd[k] = data[2*k+1];
	}

	unsigned i = 0, j = 0, k = 0;
	while(k < width / 2)
		data[k++] = even[i++];
	while(k < width)
		data[k++] = odd[j++];

	delete[] even;
	delete[] odd;

	dft1d(data, width/2, weight);
	dft1d(&data[width/2], width/2, weight);

	std::complex<double>* temp = new std::complex<double>[width];
	for(unsigned k = 0; k < width/2; k++) {
		std::complex<double> w = weight[(k*LEN) / width];
		temp[k] = data[k + width/2] * w;
		temp[k + width/2] = -data[k];
	}

	for(unsigned k = 0; k < width; k++)
		data[k] += temp[k];
	delete[] temp;
}

// Perform 2-D discrete Fourier transform (DFT).
template <typename T>
void dft2d(T *data, const unsigned width, const unsigned height,
           const int num_ranks, const int rank_id) {

/* Assignment 4: Message Passing Interface
    a. Perform row-wise one-dimensional DFTs.
    b. Transpose the data matrix for column-wise DFTs.
    c. Perform row-wise one-dimensional DFTs on the transposed matrix.
    d. Transpose the data matrix back to the original orientation.
*/

	// a. row-wise one-dimensional DFTs.
	std::complex<double>* weight = new std::complex<double>[LEN/2];
	pre_calculate_weights(weight, LEN);

	for(unsigned i = 0; i < height; i++) {
		std::complex<double>* x = new std::complex<double>[LEN];
		real_2_complex(&data[i * width], x);
		dft1d(x, width, weight);
		delete[] x;
	}

	// b. Transpose the data matrix for column-wise DFTs

}

#endif

