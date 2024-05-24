#include <cstdlib>
#include "array.h"

// Class constructor
array_t::array_t() :
    ptr(0),
    num_elements(0),
    array_size(0) {
}


/* Assignment 1: C++ Class
   Write a copy constructor and destructor of the class,
   and complete reserve() and push_back() functions.
  */

// Assignment 1 : Class Copy Constructor with Original Array 'm_array'
array_t::array_t(const array_t& m_array) :
	ptr(0),
	num_elements(m_array.num_elements),
	array_size(m_array.array_size) 
{
	// Allocating Memory Space by malloc()
	// not to create instances
	ptr = (data_t *)malloc(array_size * sizeof(data_t));

	// allocate data instance to copy array
	for (size_t i = 0; i < num_elements; i++) {
		new(&ptr[i]) data_t(m_array.ptr[i]);
	}
}

// Assignment 1 : Class Destructor
array_t::~array_t() {
	// Deallocate memory space pointed by ptr
	for(size_t i = 0; i < num_elements; i++) 
		ptr[i].~data_t();

	// Deallocate pointer 'ptr'
	free(ptr);
}


// Allocate a memory space for the specified number of elements.
void array_t::reserve(const size_t m_array_size) {
	// if function allocates a larger memory space than the current array size
	if(m_array_size > array_size) {
		// pointer pointing new memory space region
		data_t *new_ptr;

		// Allocate Memory Space to new_ptr with new size of array
		new_ptr = (data_t *)malloc(m_array_size * sizeof(data_t));

		// copies the elements from old memory to new region
		// and deallocate instances of old memory block
		for(size_t i = 0; i < num_elements; i++) {
			new(&new_ptr[i]) data_t(ptr[i]);
			ptr[i].~data_t();
		}

		// Deallocate old memory block
		free(ptr);

		// ptr points to new allocated memory region
		ptr = new_ptr;
		
		// resizes the array
		array_size = m_array_size;
	}
}

// Add a new element at the end of the array.
void array_t::push_back(const data_t m_value) {
	// if there is no empty space for array, resizes the array
	if(num_elements + 1 > array_size) {
		// allocate new memory space with 1 to empty array
		if(array_size == 0)
			array_t::reserve(1);
		// allocate double space to array
		else
			array_t::reserve(array_size * 2);
	}

	// inserts a new valeu at the end of the array
	new(&ptr[num_elements++]) data_t(m_value);
}
