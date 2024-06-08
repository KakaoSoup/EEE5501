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

/* Assignment 4: Message Passing Interface
    a. Perform row-wise one-dimensional DFTs.
    b. Transpose the data matrix for column-wise DFTs.
    c. Perform row-wise one-dimensional DFTs on the transposed matrix.
    d. Transpose the data matrix back to the original orientation.
*/

 	// Function to pre-calculate weights
    	T* weight = new T[width];
    	pre_calculate_weights(weight, width);
// a. Row-wise one-dimensional DFTs.
    for (unsigned i = 0; i < height; i++) {
        if (i % num_ranks == static_cast<unsigned>(rank_id)) {
            std::complex<double>* x = new std::complex<double>[width];
            real_2_complex(&data[i * width], x);
            dft1d(x, width, weight);
            std::memcpy(&data[i * width], x, width * sizeof(std::complex<double>));
            delete[] x;
        }
    }

    // MPI_Allgather()-like operations
    std::complex<double>* recv_buffer = new std::complex<double>[width * height];
    MPI_Request* requests = new MPI_Request[num_ranks];
    int row_count = height / num_ranks;
    int remainder_rows = height % num_ranks;
    int* displacements = new int[num_ranks];
    int* recv_counts = new int[num_ranks];

    for (int i = 0; i < num_ranks; i++) {
        displacements[i] = i * row_count * width;
        recv_counts[i] = row_count * width;
        if (i < remainder_rows) {
            recv_counts[i] += width;
        }
    }

    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            MPI_Irecv(&recv_buffer[displacements[i]], recv_counts[i], MPI_CXX_DOUBLE_COMPLEX, i, 0, MPI_COMM_WORLD, &requests[i]);
        }
    }

    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            MPI_Send(&data[rank_id * row_count * width], recv_counts[rank_id], MPI_CXX_DOUBLE_COMPLEX, i, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Status* statuses = new MPI_Status[num_ranks];
 MPI_Waitall(num_ranks, requests, statuses);

    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            std::memcpy(&data[displacements[i]], &recv_buffer[displacements[i]], recv_counts[i] * sizeof(std::complex<double>));
        }
    }

    // b. Transpose the data matrix for column-wise DFTs.
    std::complex<double>* transposed = new std::complex<double>[width * height];
    for(unsigned i = 0; i < height; i++) {
        for(unsigned j = 0; j < width; j++) {
            transposed[j * height + i] = data[i * width + j];
        }
    }

    // c. Perform row-wise one-dimensional DFTs on the transposed matrix
    for (unsigned i = 0; i < width; i++) {
        if (i % num_ranks == static_cast<unsigned>(rank_id)) {
            std::complex<double>* x = new std::complex<double>[height];
            real_2_complex(&transposed[i * height], x);
            dft1d(x, height, weight);
            std::memcpy(&transposed[i * height], x, height * sizeof(std::complex<double>));
            delete[] x;
        }
    }

    // MPI_Allgather()-like operations for transposed data
    int col_count = width / num_ranks;
    int remainder_cols = width % num_ranks;
    int* col_displacements = new int[num_ranks];
    int* col_recv_counts = new int[num_ranks];

    for (int i = 0; i < num_ranks; i++) {
        col_displacements[i] = i * col_count * height;
        col_recv_counts[i] = col_count * height;
        if (i < remainder_cols) {
            col_recv_counts[i] += height;
        }
    }

    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            MPI_Irecv(&recv_buffer[col_displacements[i]], col_recv_counts[i], MPI_CXX_DOUBLE_COMPLEX, i, 0, MPI_COMM_WORLD, &requests[i]);
        }
    }

    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            MPI_Send(&transposed[rank_id * col_count * height], col_recv_counts[rank_id], MPI_CXX_DOUBLE_COMPLEX, i, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Waitall(num_ranks, requests, statuses);

    for (int i = 0; i < num_ranks; i++) {
        if (i != rank_id) {
            std::memcpy(&transposed[col_displacements[i]], &recv_buffer[col_displacements[i]], col_recv_counts[i] * sizeof(std::complex<double>));
        }
    }

    // d. Transpose the data matrix back to the original orientation.
    for(unsigned i = 0; i < width; i++) {
        for(unsigned j = 0; j < height; j++) {
            data[j * width + i] = transposed[i * height + j];
        }
    }

    // Cleanup
    delete[] transposed;
    delete[] recv_buffer;
    delete[] weight;
    delete[] requests;
    delete[] displacements;
    delete[] recv_counts;
    delete[] col_displacements;
    delete[] col_recv_counts;
    delete[] statuses;
}

#endif

