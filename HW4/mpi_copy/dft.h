#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include "data.h"

template <typename T>
void dft1d(T* data, const unsigned width, T* weight) {
    if (width == 1)
        return;

    T* odd = new T[width/2];
    T* even = new T[width/2];

    for (unsigned k = 0; k < width / 2; k++) {
        even[k] = data[2*k];
        odd[k] = data[2*k + 1];
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

    T* temp = new std::complex<double>[width];

    for (unsigned k = 0; k < width / 2; k++) {
        T w = weight[k];
        temp[k] = data[k + width/2] * w;
        temp[k + width / 2] = -data[k];
    }

    for(unsigned k = 0; k < width; k++)
        data[k] += temp[k];

    delete[] temp;
}


// Perform 2-D discrete Fourier transform (DFT).
template <typename T>
void dft2d(T *data, const unsigned width, const unsigned height, const int num_ranks, const int rank_id) {
    	//  pre-calculate weights
	std::complex<double>* weight = new T[width];
	double angle;
	for (unsigned n = 0; n < (width / 2); n++) {
		angle = (2 * M_PI / width) * n;
    		weight[n].real(cos(angle));
    		weight[n].imag(-sin(angle));
	}

    	// a. Row-wise one-dimensional DFTs.
    	for (unsigned i = 0; i < height; i++) {
            	dft1d(&data[width * i], width, weight);
    	}

	// b. transpose the array
	T* transpose = new T[height * width];
	for(unsigned i = 0; i < width; i++)
		for(unsigned j = 0; j < height; j++)
			transpose[height * i + j] = data[width * j + i];

    	// c. Row-wise one-dimensional DFTs.
    	for (unsigned i = 0; i < width; i++) {
            	dft1d(&transpose[height * i], height, weight);
    	}


	// d. transpose the array to data
	for(unsigned i = 0; i < width; i++)
		for(unsigned j = 0; j < height; j++)
			data[width * j + i] = transpose[height * i + j];
	
	delete[] transpose;
	delete[] weight;
}


#endif

