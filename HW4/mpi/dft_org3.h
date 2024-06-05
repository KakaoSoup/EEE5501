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

template <typename T>
void dft2d(T* data, unsigned width, unsigned height, int num_ranks, int rank_id) {
    T* weight = new T[width];
    pre_calculate_weights(weight, width);

    int h = height;
    int w = width;

    // a. Row-wise one-dimensional DFTs.
    for (int i = 0; i < h; i++) {
        if ((i % num_ranks) == rank_id) {
            std::complex<double>* x = new std::complex<double>[width];
            real_2_complex(&data[i * width], x);
            dft1d(x, width, weight);
            std::memcpy(&data[i * width], x, width * sizeof(std::complex<double>));
            delete[] x;
        }
    }

    // MPI_Irecv and MPI_Send for data exchange
    std::complex<double>* recv_buffer = new std::complex<double>[width * height];
    MPI_Request* requests = new MPI_Request[height];

    for (int i = 0; i < h; i++) {
        if ((i % num_ranks) != rank_id) {
            MPI_Irecv(&recv_buffer[i * width], width, MPI_CXX_DOUBLE_COMPLEX, i % num_ranks, 0, MPI_COMM_WORLD, &requests[i]);
        }
    }

    for (int i = 0; i < h; i++) {
        if ((i % num_ranks) == rank_id) {
            for (int j = 0; j < num_ranks; j++) {
                if (j != rank_id) {
                    MPI_Send(&data[i * width], width, MPI_CXX_DOUBLE_COMPLEX, j, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    // Wait for all Irecv operations to complete
    MPI_Waitall(height, requests, MPI_STATUSES_IGNORE);

    // Copy received data back to original data array
    for (int i = 0; i < h; i++) {
        if ((i % num_ranks) != rank_id) {
            std::memcpy(&data[i * width], &recv_buffer[i * width], width * sizeof(std::complex<double>));
        }
    }

    delete[] recv_buffer;
    delete[] requests;

    // b. Transpose the data matrix for column-wise DFTs
    std::complex<double>* transposed = new std::complex<double>[width * height];
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            transposed[j * height + i] = data[i * width + j];
        }
    }

    // c. Perform row-wise one-dimensional DFTs on the transposed matrix.
    for (int i = 0; i < w; i++) {
        if ((i % num_ranks) == rank_id) {
            std::complex<double>* x = new std::complex<double>[height];
            real_2_complex(&transposed[i * height], x);
            dft1d(x, height, weight);
            std::memcpy(&transposed[i * height], x, height * sizeof(std::complex<double>));
            delete[] x;
        }
    }

    // MPI_Irecv and MPI_Send for data exchange
    recv_buffer = new std::complex<double>[width * height];
    requests = new MPI_Request[width];

    for (int i = 0; i < w; i++) {
        if ((i % num_ranks) != rank_id) {
            MPI_Irecv(&recv_buffer[i * height], height, MPI_CXX_DOUBLE_COMPLEX, i % num_ranks, 0, MPI_COMM_WORLD, &requests[i]);
        }
    }

    for (int i = 0; i < w; i++) {
        if ((i % num_ranks) == rank_id) {
            for (int j = 0; j < num_ranks; j++) {
                if (j != rank_id) {
                    MPI_Send(&transposed[i * height], height, MPI_CXX_DOUBLE_COMPLEX, j, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    // Wait for all Irecv operations to complete
    MPI_Waitall(width, requests, MPI_STATUSES_IGNORE);

    // Copy received data back to original transposed array
    for (int i = 0; i < w; i++) {
        if ((i % num_ranks) != rank_id) {
            std::memcpy(&transposed[i * height], &recv_buffer[i * height], height * sizeof(std::complex<double>));
        }
    }

    delete[] recv_buffer;
    delete[] requests;

    // d. Transpose the data matrix back to the original orientation.
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            data[j * width + i] = transposed[i * height + j];
        }
    }

    delete[] transposed;
    delete[] weight;
}


#endif
