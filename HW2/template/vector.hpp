#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <cstdlib>
#include <new>

// Constructor
template <typename T>
vector_t<T>::vector_t(void) :
    array(0),
    array_size(0),
    num_elements(0) {
    // Nothing to do
}

// Get the number of elements in the array.
template <typename T>
inline size_t vector_t<T>::size(void) const { return num_elements; }

// Get the allocated array size in the unit of elements.
template <typename T>
inline size_t vector_t<T>::capacity(void) const { return array_size; }

// Get a reference of the element at the given index.
template <typename T>
inline T& vector_t<T>::operator[](const size_t m_index) const { return array[m_index]; }

// Get an iterator pointing to the first element of the array.
template <typename T>
inline typename vector_t<T>::iterator vector_t<T>::begin(void) const {
    return iterator_t<T>(array);
}

// Get an iterator pointing to the next of the last element.
template <typename T>
inline typename vector_t<T>::iterator vector_t<T>::end(void) const {
    return iterator_t<T>(array+num_elements);
}


/* Assignment 2: C++ Template
   Fill in the following functions of the template class.
 */


// Copy constructor
template <typename T>
vector_t<T>::vector_t(const vector_t<T> &m_vector) {
    	/* Assignment */
	// set the size and num_elements of new vector
	num_elements = m_vector.num_elements;
	array_size = m_vector.array_size;

	// memory allocation of new vector
	array = (T*)malloc(sizeof(T) * array_size);

	// copying elements of original vector
	for(size_t i = 0; i < array_size; i++) {
		new (&array[i]) T(m_vector.array[i]);
	}
}

// Destructor
template <typename T>
vector_t<T>::~vector_t(void) {
	/* Assignment */
	// delete elements of vector
	for(size_t i = 0; i < num_elements; i++)
		array[i].~T();

	// deallocate vector
	free(array);
}

// Reserve an array space for the given number of elements.
template <typename T>
void vector_t<T>::reserve(size_t m_array_size) {
    	/* Assignment */
	if(m_array_size > array_size) {
		// allocate new vector
		T* new_vector;	
		new_vector = (T*)malloc(m_array_size * sizeof(T));

		// copy and delete old vector's elements
		for(size_t i = 0; i < num_elements; i++) {
			new(&new_vector[i]) T(array[i]);
			array[i].~T();
		}

		// free old vector
		free(array);

		// allocate new vector to array
		array = new_vector;
		array_size = m_array_size;
	}
}

// Resize the array to have the given number of elements.
template <typename T>
void vector_t<T>::resize(size_t m_num_elements, T m_data) {
    	/* Assignment */

	// if reqeusted size is smaller than number of elements in array
	if(m_num_elements < num_elements) {
		// delete the elements to shrink vector
		for(size_t i = m_num_elements; i < num_elements; i++) {
			array[i].~T();
		}	
	}
	// if requested size is bigger than number of elements in array
	else {
		// if requested size is greater than current array_size
		if(m_num_elements > array_size) {
			// double up the array size
			vector_t<T>::reserve(array_size * 2);	
		}

		// fill the deficits by m_data
		for(size_t i = num_elements; i < m_num_elements; i++) {
			new(&array[i]) T(m_data);	
		}
	}

	num_elements = m_num_elements;
}

// Remove all elements in the array.
template <typename T>
void vector_t<T>::clear(void) {
    	/* Assignment */
	// delete elements from 1st element to last element
	for(size_t i = 0; i < num_elements; i++) {
		array[i].~T();
	}
	// set number of elements zero
	num_elements = 0;
}

// Add a new element at the end of the array.
template <typename T>
void vector_t<T>::push_back(const T &m_data) {
    	/* Assignment */
	// if the vector is full, then size up the vector
	if(num_elements + 1 > array_size) {
		// vector was empty -> size = 1
		if(array_size == 0)
			vector_t<T>::reserve(1);
		// vector was not empty -> double up the size
		else
			vector_t<T>::reserve(array_size * 2);
	}

	// push back new data to end of vector
	new(&array[num_elements++]) T(m_data);
}

// Remove the last element in the array.
template <typename T>
void vector_t<T>::pop_back(void) {
    	/* Assignment */

	// if vector is not empty
	if(num_elements) {
		// erase the last element
		array[--num_elements].~T();
	}

}

// Assign new contents to the array.
template <typename T>
vector_t<T>& vector_t<T>::operator=(const vector_t<T>& m_vector) {
    	/* Assignment */
	if(this != &m_vector) {
		// clear the vector
		clear();
		// if current vector's size is smaller than
		// the size of vector to be copied,
		// then reserve the memory space for current vector
		if(capacity() < m_vector.size())
			reserve(m_vector.size());

		// copying the elements of the vector to be copied
		for(size_t i = 0; i < m_vector.size(); i++) {
			push_back(m_vector[i]);
		}
	}

    	return *this; // return the current vector
}

// Add a new element at the location pointed by the iterator.
template <typename T>
typename vector_t<T>::iterator vector_t<T>::insert(vector_t<T>::iterator m_it,
        const T &m_data) {
    	/* Assignment */
	// if m_it is end of vector
	if(m_it == vector_t<T>::end()) {
		push_back(m_data);
	}
	else {
		// get last element's location to it1
		vector_t<T>::iterator it1 = --end();
		vector_t<T>::iterator it2;
		// push back last element to the tail of vector
		push_back(*it1);
		// shift right elements from m_it to last element
		for( ; it1 != m_it; ) {
			it2 = it1--;
			*it2 = *it1;
		}
		// insert new data to m_it
		*m_it = T(m_data);
	}
    	return m_it;  // return m_it
}

// Erase an element at the location pointed by the iterator.
template <typename T>
typename vector_t<T>::iterator vector_t<T>::erase(vector_t<T>::iterator m_it) {
    /* Assignment */
	if(m_it == end()) {
		// invalid m_it, because end() refers to an invalid elements;
		return m_it;
	}
	else {
		// get fw(==m_it), bk(fw+1)
		vector_t<T>::iterator bk = m_it;
		vector_t<T>::iterator fw = bk++;
		// shift left elements from m_it to last element
		while(bk != end()) {
			*fw++ = *bk++;
		}
		// erase last element which one is copied
		pop_back();
	}
    	return m_it; // return m_it
}

#endif
