#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include "data.h"

#define LEN 1024

void pre_calculate_weights(std::complex<double>* weight, unsigned N) {
    double angle;
    for (unsigned n = 0; n < N / 2; n++) {
        angle = (2 * n * M_PI) / N;
        weight[n] = std::complex<double>(cos(angle), -sin(angle));
    }
}

void real_2_complex(std::complex<double>* data, std::complex<double>* x) {
    for (unsigned i = 0; i < LEN; ++i) {
        x[i] = data[i];
    }
}

template <typename T>
void real_2_complex(T* data, std::complex<double>* x) {
    for (unsigned i = 0; i < LEN; ++i) {
        x[i] = std::complex<double>(static_cast<double>(data[i]), 0.0);
    }
}

void dft1d(std::complex<double>* data, const unsigned width,
		std::complex<double>* weight) {
    if (width == 1)
        return;

    std::complex<double>* odd = new std::complex<double>[width/2];
    std::complex<double>* even = new std::complex<double>[width/2];

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

    std::complex<double>* temp = new std::complex<double>[width];

    for (unsigned k = 0; k < width / 2; k++) {
	    std::complex<double> w = weight[(k*LEN) / width];
	    temp[k] = data[k + width/2] * w;
	    temp[k + width / 2] = -data[k];
    }

    for(unsigned k = 0; k < width; k++)
	    data[k] += temp[k];

    delete[] temp;
}


// Perform 2-D discrete Fourier transform (DFT).
template <typename T>
void dft2d(T *data, const unsigned width, const unsigned height,
           const int num_ranks, const int rank_id) {

    // Function to pre-calculate weights
    T* weight = new T[width];
    pre_calculate_weights(weight, width);

    // a. Row-wise one-dimensional DFTs.
    for (unsigned i = 0; i < height; i++) {
        std::complex<double>* x = new std::complex<double>[width];
        real_2_complex(&data[i * width], x);
        dft1d(x, width, weight);
        std::memcpy(&data[i * width], x, width * sizeof(std::complex<double>));
        delete[] x;
    }

    // MPI_Allgather()-like operations
    std::complex<double>* recv_buffer = new std::complex<double>[width * height];
    for (unsigned i = 0; i < height; i++) {
        if ((i % static_cast<unsigned>(num_ranks)) == static_cast<unsigned>(rank_id)) {
            // Send local data to all other processes
            for (int j = 0; j < num_ranks; j++) {
                if (j != rank_id) {
                    MPI_Send(&data[i * width], width, MPI_CXX_DOUBLE_COMPLEX, j, 0, MPI_COMM_WORLD);
                }
            }
        } else {
            // Receive data from other processes
            MPI_Recv(&recv_buffer[i * width], width, MPI_CXX_DOUBLE_COMPLEX, i % num_ranks, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Copy received data back to original data array
    std::memcpy(data, recv_buffer, width * height * sizeof(std::complex<double>));

    // Cleanup
    delete[] recv_buffer;
    delete[] weight;
}

/*
// Perform 2-D discrete Fourier transform (DFT).
template <typename T>
void dft2d(T* data, const unsigned width, const unsigned height, 
		const int num_ranks, const int rank_id) {
    std::complex<double>* weight = new std::complex<double>[LEN / 2];
    pre_calculate_weights(weight, LEN);

    // a. Row-wise one-dimensional DFTs.
    for (unsigned i = 0; i < height; i++) {
        std::complex<double>* x = new std::complex<double>[LEN];
        real_2_complex(&data[i * width], x);
        dft1d(x, width, weight);
        std::memcpy(&data[i * width], x, width * sizeof(std::complex<double>));
        delete[] x;
    }

    // b. Transpose the data matrix for column-wise DFTs
    std::complex<double>* transposed = new std::complex<double>[width * height];
    for (unsigned i = 0; i < height; i++) {
        for (unsigned j = 0; j < width; j++) {
            transposed[j * height + i] = data[i * width + j];
        }
    }

    // c. Perform row-wise one-dimensional DFTs on the transposed matrix.
    for (unsigned i = 0; i < width; i++) {
        std::complex<double>* x = new std::complex<double>[LEN];
        real_2_complex(&transposed[i * height], x);
        dft1d(x, height, weight);
        std::memcpy(&transposed[i * height], x, height * sizeof(std::complex<double>));
        delete[] x;
    }

    // d. Transpose the data matrix back to the original orientation.
    for (unsigned i = 0; i < width; i++) {
        for (unsigned j = 0; j < height; j++) {
            data[j * width + i] = transposed[i * height + j];
        }
    }

    delete[] transposed;
    delete[] weight;
}
*/
#endif

