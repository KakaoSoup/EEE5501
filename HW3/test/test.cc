#include <pthread.h>
#include <iostream>

const int MAX_THREADS = 8;

template <typename T>
void merge(T* array, int start, int middle, int end) {
    if (start + 1 >= end)
        return;

    int left_size = middle - start;
    int right_size = end - middle;

    T* left = new T[left_size];
    T* right = new T[right_size];

    for (int i = 0; i < left_size; i++)
        left[i] = array[start + i];
    for (int i = 0; i < right_size; i++)
        right[i] = array[middle + i];

    int i = 0, j = 0, k = start;
    while (i < left_size && j < right_size) {
        if (left[i] < right[j])
            array[k++] = left[i++];
        else
            array[k++] = right[j++];
    }

    while (i < left_size)
        array[k++] = left[i++];
    while (j < right_size)
        array[k++] = right[j++];

    delete[] left;
    delete[] right;
}

template <typename T>
void merge_sort(T* array, int left, int right) {
    if (left + 1 >= right)
        return;
    int mid = (left + right) >> 1;
    merge_sort(array, left, mid);
    merge_sort(array, mid, right);
    merge(array, left, mid, right);
}

void* thread_merge_sort(void* arg) {
    int* args = (int*)arg;
    int* array = (int*)args[0];
    int left = args[1];
    int right = args[2];
    merge_sort(array, left, right);
    return nullptr;
}

template <typename T>
void sort(T* array, const size_t num_data, const unsigned num_threads) {
    pthread_t threads[MAX_THREADS];
    int* args[MAX_THREADS];
    int frag_size = num_data / num_threads;

    for (size_t i = 0; i < num_threads; i++) {
        args[i] = new int[3];
        args[i][0] = (int)array;
        args[i][1] = i * frag_size;
        args[i][2] = (i == num_threads - 1) ? num_data : (i + 1) * frag_size;
        pthread_create(&threads[i], nullptr, thread_merge_sort, args[i]);
    }

    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
        delete[] args[i];
    }

    // Merging the sorted fragments
    for (size_t size = frag_size; size < num_data; size *= 2) {
        for (size_t left = 0; left < num_data; left += 2 * size) {
            int mid = std::min(left + size, num_data);
            int right = std::min(left + 2 * size, num_data);
            if (mid < right) {
                merge(array, left, mid, right);
            }
        }
    }
}

int inputs[16] = {
    36, 65, 17, 1,
    8, 7, 15, 41,
    44, 63, 12, 99,
    9, 18, 2, 56
};

int main() {
    sort(inputs, 16, 2);

    for (int i = 0; i < 16; ++i) {
        std::cout << inputs[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
