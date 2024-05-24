#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "string.h"

typedef string_t data_t;        // data_t represents string_t.

class array_t {
public:
    // Class constructor
    array_t();


    /* Assignment 1: C++ Class
       Write a copy constructor and destructor of the class.
     */

    // Class Copy Constructor with original array
    array_t (const array_t& m_array);
    // Class Destructor
    ~array_t();


    // Allocate a memory space for the specified number of elements.
    void reserve(const size_t m_array_size);
    // Add a new element at the end of the array.
    void push_back(const data_t m_value);
    // Reference operator
    data_t& operator[](const size_t m_index) const { return ptr[m_index]; }
    // Get the number of elements in the array.
    size_t size() const { return num_elements; }
    // Get the size of allocated memory space.
    size_t capacity() const { return array_size; }

private:
    data_t *ptr;                // Pointer to an array
    size_t num_elements;        // Actual number of elements in the array
    size_t array_size;          // Allocated array size
};

#endif
