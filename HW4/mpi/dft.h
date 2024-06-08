#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include <cmath>
#include "data.h"

#define LEN	1024

// Helper function for 1-D DFT using the Cooley-Tukey FFT algorithm with precalculated weights
template <typename T>
void dft1d(T* data, int n, T* weights) {
    if (n <= 1) return;

    // Divide in evne and odd
    T* even = new T[n / 2];
    T* odd = new T[n / 2];

    // reorder it with even number first and odd number later.
    int i = 0, j = 0, k = 0;
    for (i = 0; i < n / 2; i++) {
        even[i] = data[i * 2];
        odd[i] = data[i * 2 + 1];
    }

    // reorder 'data' with even -> odd
    i = 0; j = 0; k = 0;
    while(k < n / 2)
	    data[k++] = even[i++];
    while(k < n)
	    data[k++] = odd[j++];

    // delete the copies
    delete[] even;
    delete[] odd;

    // recursive call of dft1d
    dft1d(data, n/2, weights);
    dft1d(&data[n/2], n/2, weights);

    // temp array to calculate
    T* temp = new T[n];

    // Combine
    for (int k = 0; k < n / 2; ++k) {
        T W = weights[k * LEN / n] * data[k + n/2];
        temp[k] = data[k] + W;
        temp[k + n / 2] = data[k] - W;
    }

    // copy temp to data
    for(i = 0; i < n; i++)
	    data[i] = temp[i];
    delete[] temp;
}

// Perform 2-D discrete Fourier transform (DFT)
template <typename T>
void dft2d(T* data, const unsigned width, const unsigned height, const int num_ranks, const int rank_id) {
    // Precalculate weights for the FT
    T* weights = new T[width / 2];
    for (unsigned k = 0; k < width / 2; k++) {
        weights[k] = std::polar(1.0, -2 * M_PI * k / width);
    }

    // a. Row-wise one-dimensional DFTs
    for (unsigned i = 0; i < height; i++) {
	// run dft1d for rows which are multiples of rank_id
        if ((i % num_ranks) == (unsigned)rank_id) {
            dft1d(&data[width * i], width, weights);
        }
    }

    // buffer for received data
    T* recv_buffer = new T[width * height];
    // MPI requests to check other processor's data transfer
    MPI_Request* requests = new MPI_Request[num_ranks * height];
    // count reqeusts
    int request_count = 0;

    // receive data from other processes to fill uncalculated values
    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            for (unsigned j = i; j < height; j += num_ranks) {
                MPI_Irecv(&recv_buffer[width * j], width, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD, &requests[request_count++]);
            }
        }
    }

    // Send data to all other processes
    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            for (unsigned j = rank_id; j < height; j += num_ranks) {
		// send the datas which are calculated in this processor
                MPI_Send(&data[width * j], width, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD);
            }
        }
    }

    // Wait for all non-blocking receives to complete
    MPI_Waitall(request_count, requests, MPI_STATUSES_IGNORE);

    // Copy received data back to original array
    for (unsigned i = 0; i < height; i++) {
        if ((i % num_ranks) != (unsigned)rank_id) {
            memcpy(&data[width * i], &recv_buffer[width * i], width * sizeof(T));
        }
    }

    // b. Transpose the array
    T* transposed = new T[height * width];
    for (unsigned i = 0; i < width; i++) {
        for (unsigned j = 0; j < height; j++) {
            transposed[height * i + j] = data[width * j + i];
        }
    }

    // c. Row-wise one-dimensional DFTs on the transposed array
    for (unsigned i = 0; i < width; i++) {
        if ((i % num_ranks) == (unsigned)rank_id) {
            dft1d(&transposed[height * i], height, weights);
        }
    }

    // Reset the request count
    request_count = 0;

    // receive data from other processes to fill uncalculated values
    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            for (unsigned j = i; j < width; j += num_ranks) {
                MPI_Irecv(&recv_buffer[height * j], height, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD, &requests[request_count++]);
            }
        }
    }
    
    // Send transposed data to all other processes
    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            for (unsigned j = rank_id; j < width; j += num_ranks) {
		// send the datas which are calculated in this processor
                MPI_Send(&transposed[height * j], height, MPI_DOUBLE_COMPLEX, i, j, MPI_COMM_WORLD);
            }
        }
    }

    // Wait for all non-blocking receives to complete
    MPI_Waitall(request_count, requests, MPI_STATUSES_IGNORE);

    // Copy received data back to transposed array
    for (unsigned i = 0; i < width; i++) {
        if ((i % num_ranks) != (unsigned)rank_id) {
            memcpy(&transposed[height * i], &recv_buffer[height * i], height * sizeof(T));
        }
    }

    // d. Transpose the array back to original orientation
    for (unsigned i = 0; i < width; i++) {
        for (unsigned j = 0; j < height; j++) {
            data[width * j + i] = transposed[height * i + j];
        }
    }

    delete[] transposed;
    delete[] weights;
    delete[] recv_buffer;
    delete[] requests;
}

#endif
