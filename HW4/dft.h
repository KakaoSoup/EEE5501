#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include "data.h"

#define LEN	1024

// Perform 1-D discrete Fourier transform (DFT).
template <typename T>
void dft1d(T* data, int n, T* w) {
    if (n <= 1) return;

    T* even = new T[n / 2];
    T* odd = new T[n / 2];

    // sort data with even and odd
    for (int i = 0; i < n / 2; ++i) {
        even[i] = data[i * 2];
        odd[i] = data[i * 2 + 1];
    }

    // Divide with even and odd
    dft1d(even, n / 2, w);
    dft1d(odd, n / 2, w);

    // Combine
    for (int k = 0; k < n / 2; ++k) {
        T W = w[k * LEN / n] * odd[k];
        data[k] = even[k] + W;
        data[k + n / 2] = even[k] - W;
    }

    // delete copy of even and odd values
    delete[] even;
    delete[] odd;
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

	// Precalculate weights for the FFT
    	T* weights = new T[width / 2];
    	for (unsigned k = 0; k < width / 2; ++k) {
        	weights[k] = std::polar(1.0, -2 * M_PI * k / width);
    	}

	// a. Row-wise one-dimensional DFTs.
	for (unsigned i = 0; i < height; i++) {
		if((i % num_ranks) == (unsigned)rank_id)
    			dft1d(&data[width * i], width, weights);
	}

	// recv_buffer for received data
	T* recv_buffer = new T[width * height];
	MPI_Request* requests = new MPI_Request[num_ranks * 2];
	int request_count = 0;

	// before non-blocking receives
	for(int i = 0; i < num_ranks; ++i) {
		if(i != rank_id)
			for(unsigned j = i; j < height; j+= num_ranks)
				MPI_Irecv(&recv_buffer[width * j], width, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD, &requests[request_count++]);
	}

	// Send data to other processes
	for(int i = 0; i < num_ranks; ++i) {
		if( i != rank_id)
			for(unsigned j = rank_id; j < height; j+= num_ranks)
				MPI_Send(&data[width * j], width, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD);
	}

	// wait for non-blocking receives to be completed
	MPI_Waitall(request_count, requests, MPI_STATUSES_IGNORE);

	// copy recevied data to data
	for(unsigned i = 0; i < height; ++i)
		if((i % num_ranks) != (unsigned)rank_id)
			memcpy(&data[width * i], &recv_buffer[width * i], width * sizeof(T));

	// b. transpose the array
	T* transpose = new T[height * width];
	for (unsigned i = 0; i < width; i++)
    		for (unsigned j = 0; j < height; j++)
        		transpose[height * i + j] = data[width * j + i];

	// c. Row-wise one-dimensional DFTs for tansforsed array.
	for (unsigned i = 0; i < width; i++) {
		if((i % num_ranks) == (unsigned)rank_id)
    			dft1d(&transpose[height * i], height, weights);
	}
	
	// reset request count
	request_count = 0;

	// before non-blocking receives
	for(int i = 0; i < num_ranks; ++i) {
		if(i != rank_id)
			for(unsigned j = i; j < width; j+= num_ranks)
				MPI_Irecv(&recv_buffer[height * j], height, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD, &requests[request_count++]);
	}

	// Send transposed data to other processes
	for(int i = 0; i < num_ranks; ++i) {
		if( i != rank_id)
			for(unsigned j = rank_id; j < width; j+= num_ranks)
				MPI_Send(&transpose[height * j], height, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD);
	}

	// wait for non-blocking receives to be completed
	MPI_Waitall(request_count, requests, MPI_STATUSES_IGNORE);

	// copy recevied data to data
	for(unsigned i = 0; i < height; ++i)
		if((i % num_ranks) != (unsigned)rank_id)
			memcpy(&data[width * i], &recv_buffer[width * i], width * sizeof(T));


	// d. transpose the array to data
	for (unsigned i = 0; i < width; i++)
    		for (unsigned j = 0; j < height; j++)
        		data[width * j + i] = transpose[height * i + j];

	delete[] transpose;
	delete[] weights;
	delete[] recv_buffer;
	delete[] requests;
}

#endif

