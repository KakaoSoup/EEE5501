#include <pthread.h>
#include <cstring>
#include <iostream>

#define SIZE		16
#define MAX_THREAD	1024

size_t frag_size;
int func_cnt;

int data[SIZE] = {
	7,	65,	36,	10,
	22,	1,	13,	56,
	4,	25,	30,	24,
	77,	57,	12,	8
};

template <typename T>
void merge(T* array, int start, int middle, int end) {
	if(start+1 >= end)
		return;

	int left_size = middle - start;
	int right_size = end - middle;

	T* left = new T[left_size];
	T* right = new T[right_size];

	for(int i = 0; i < left_size; i++)
		//left[i] = array[start + i];
		*left = *array;
	for(int i = 0; i < right_size; i++)
		//right[i] = array[middle + i];
		right[i] = array[i];

	int i = 0, j = 0, k = start;
	while(i < left_size && j < right_size) {
		if(left[i] < right[j])
			//array[k++] = left[i++];
			array[k] = left[i];
		else
			//array[k++] = right[j++];
			//array[k] = right[j];
	}

	while(i < left_size)
		array[k++] = left[i++]; 
	while(j < right_size)
		array[k++] = right[j++]; 

	delete[] left;
	delete[] right;
}


template <typename T>
void merge_sort(T* array, int left, int right) {
	if(left+1 == right)
		return;

	int mid = (left + right) >> 1;
	merge_sort(array, left, mid);
	merge_sort(array, mid, right);
	merge(array, left, mid, right);
}

template <typename T>
void* merge_sort(T* array) {
	int left = frag_size * func_cnt++;
	int right = frag_size * func_cnt;
	int mid = (left + right) >> 1;

	merge_sort(array, left, mid);
	merge_sort(array, mid, right);
	merge(array, left, mid, right);
	return 0;
}

template <typename T>
void sort(T* array, const size_t num_data, const unsigned num_threads) {
	pthread_t threads[MAX_THREAD];	
	frag_size = num_data / num_threads;
	func_cnt = 0;

	for(size_t i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, merge_sort, (T*)(array));
		printf("thread #%ld = %p created\n", i, &threads[i]);
	}
	for(size_t i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);
}


int main(int argc, char* argv[]) {
	size_t num_threads = 4;
	int* array = new int[SIZE];
	for(size_t i = 0; i < SIZE; i++)
		array[i] = data[i];

	sort(array, SIZE, num_threads);

	delete[] array;
	return 0;
}
