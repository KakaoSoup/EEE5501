#ifndef __DFT_H__
#define __DFT_H__

#include <complex>
#include <cstdlib>
#include <cstring>
#include <mpi.h>
#include "data.h"

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

}

#endif

