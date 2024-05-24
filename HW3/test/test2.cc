#include <iostream>

#define MAX_THREADS	1024

template <typename T>
void test(T* array, int len) {
	T* test = new T[len];

	for(int i = 0; i < 10; i++) {
		test[i] = array[i];
	}

	for(int i = 0; i < 10; i++) {
		std::cout << test[i] << " ";
	}

	return;
}

template <typename T>
void merge(T* array, int start, int middle, int end) {
	if(start + 1 >= end)
		return;

	int left_size = middle - start;
	int right_size = end - middle;

	T* left = new T[left_size];
	T* right = new T[right_size];

	for(int i = 0; i < left_size; i++)
		left[i] = array[i];
	for(int i = 0; i< right_size; i++)
		right[i] = array[i];

	int i = 0, j = 0, k = start;
	while(i < left_size && j < right_size) {
		if(left[i] < right[j])
			array[k++] = left[i++];
		else
			array[k++] = right[j++];
	}

	while(i < left_size)
		array[k++] = left[i++];
	while(j < right_size)
		array[k++] = left[j++];

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
	return;
}

int frag_size = 0;
int func_cnt = 0;

template <typename T>
void* merge_sort(void* array) {
	int left = frag_size * func_cnt++;
	int right = frag_size * func_cnt;

	merge_sort(array, left, right);
	return 0;
}


template <typename T>
void sort(T* array, const size_t num_data, const unsigned num_threads) {
	pthread_t threads[MAX_THREADS];
	frag_size = num_data / num_threads;
	func_cnt = 0;

	for(size_t i = 0; i < num_threads; i++)
		pthread_create(&threads[i], NULL, merge_sort, array);
	for(size_t i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);
}

int inputs[16] = {
	36, 65, 17, 1,
	8, 7, 15, 41,
	44, 63, 12, 99,
	9, 18, 2, 56
};

int main() {
	//int array[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//char array[20] = "Bull Shit!\n";
	sort(inputs, 16, 2);

	return 0;
	
}
