#include <cstdlib>
#include <cstring>
#include "string.h"

// Minimal implementation of string
string_t::string_t() : ptr(0), length(0), self(this) {
}

string_t::string_t(const string_t& m_string) :
    ptr(0),
    length(m_string.length),
    self(this) {
    if(length) {
        ptr = (char*)malloc(length+1);
        strcpy(ptr, m_string.ptr);
    }
}

string_t::string_t(const char* m_char) :
    ptr(0),
    length(strlen(m_char)),
    self(this) {
    if(length) {
        ptr = (char*)malloc(length+1);
        strcpy(ptr, m_char);
    }
}

string_t::~string_t() {
    free(self->ptr);
}

string_t& string_t::operator=(const char* m_char) {
    if(m_char != ptr) {
        free(ptr);
        length = strlen(m_char);
        ptr = (char*)malloc(length+1);
        strcpy(ptr, m_char);
    }
    return *this;
}
