#ifndef __SORT_H__
#define __SORT_H__

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <new>
#include <pthread.h>
#include <thread>

/* Assignment 3: Multi-threading
   Fill in the sort function to implement a merge-sort algorithm.
 */

#define MAX_THREADS	1024

// merge with 2 data fragements
template <typename T>
void merge(T* array, int start, int middle, int end) {
    // if the size of data fragment is 1, return it
    if (start + 1 >= end)
        return;

    // size of left data fragment
    int left_size = middle - start;
    // size of right data fragment
    int right_size = end - middle;

    // copy of left data fragment
    T* left = new T[left_size];
    // copy of right data fragment
    T* right = new T[right_size];

    // copying data fragments
    for (int i = 0; i < left_size; i++)
        left[i] = array[start + i];
    for (int i = 0; i < right_size; i++)
        right[i] = array[middle + i];

    // sort datas in ascending order
    int i = 0, j = 0, k = start;
    while (i < left_size && j < right_size) {
        if (left[i] < right[j])
            array[k++] = left[i++];
        else
            array[k++] = right[j++];
    }

    // fills remain datas to array
    while (i < left_size)
        array[k++] = left[i++];
    while (j < right_size)
        array[k++] = right[j++];

    // deletes copies
    delete[] left;
    delete[] right;
}

// merge sort with array
template <typename T>
void merge_sort(T* array, int left, int right) {
    // if the size of data fragment is 1, return it
    if (left + 1 >= right)
        return;

    // mid : half of beginning and end of data fragment
    int mid = (left + right) >> 1;

    // splits a data fragment into 2 data fragments
    merge_sort(array, left, mid);
    merge_sort(array, mid, right);
    // combines divided fragments into a sorted fragment
    merge(array, left, mid, right);
}

// structure with left, right end of array
template <typename T>
struct SortArgs {
    T* array;	// data
    int left;	// start of frag
    int right;	// end of frag
};


template <typename T>
void* thread_merge_sort(void* arg) {
    SortArgs<T>* args = (SortArgs<T>*)arg;
    // merge sorts a fragment which size is 'frag_size'
    merge_sort(args->array, args->left, args->right);
    return NULL;
}


template <typename T>
void sort(T *array, const size_t num_data, const unsigned num_threads) {
    /* Assignment */
    // make the threads with MAX_THREADS
    pthread_t threads[MAX_THREADS];
    SortArgs<T> args[MAX_THREADS];

    // fragments' size
    int frag_size = num_data / num_threads;

    // merge-sort with #num_thread fragments which size is (num_data / num_threads)
    for (size_t i = 0; i < num_threads; i++) {
        args[i].array = array;
	// set the left and right end of fragements
        args[i].left = i * frag_size;
        args[i].right = (i == num_threads - 1) ? num_data : (i + 1) * frag_size;
	// execute thread_merge_sort() with data fragments
        pthread_create(&threads[i], NULL, thread_merge_sort<T>, &args[i]);
    }

    // execute threads in order
    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Merging the sorted fragments with threads by doubling its size
    // while the size of fragment being original size
    for (size_t size = frag_size; size < num_data; size *= 2) {
        for (size_t left = 0; left < num_data; left += size * 2) {
            int mid = std::min(left + size, num_data);
            int right = std::min(left + size * 2, num_data);
	    // merging #2N and #2N+1 fragments (0 <= N < (num_data / size / 2))
            if (mid < right) {
                merge(array, left, mid, right);
            }
        }
    }
}

#endif
