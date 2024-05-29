#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include "data.h"


template <typename T>
void reordering(T* data, const unsigned width) {
	if(width == 1)
		return;

	T* odd = new T[width / 2];
	T* even = new T[width / 2];

	unsigned i = 0, j = 0, k = 0;

	for(k = 0; k < width; k++) {
		if(k & 0x1)
			odd[i++] = data[k];
		else
			even[j++] = data[k];	
	}

	i = 0; j = 0; k = 0;

	while(k < (width / 2))
		data[k++] = even[i++];
	while(k < width)
		data[k++] = odd[j++];

	delete[] odd;
	delete[] even;

	reordering(data, width >> 1);
	reordering(&data[width >> 1], width >> 1);
}

template <typename T>
void row_wise_dft(T *data, const unsigned width) {
	// 1. reordering the input array to bit-reversed order
	reordering(data, width);

	// 2. pre-calculating W^n weights
	// 3. logarithmic Transformation
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
	size_t cnt = 0;
	T* row_wise = data;
	while(cnt < height) {
		row_wise_dft(row_wise, width);
		row_wise += width;
		cnt++;
	}

	// b. Transpose the data matrix for column-wise DFTs

}

#endif

