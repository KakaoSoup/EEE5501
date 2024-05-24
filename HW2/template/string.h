#include <cstring>
#include <iostream>

// Minimal implementation of string
class string_t {
public:
    string_t();                                     // Constructor
    string_t(const string_t &m_string);             // Copy constructor
    string_t(string_t &&m_string);                  // Move constructor
    string_t(const char *m_char);                   // Constructor
    ~string_t();                                    // Destructor

    string_t& operator=(const string_t &m_string);  // Assignment operator
    string_t& operator=(const char *m_char);        // Assignment operator
    string_t& operator=(string_t &&m_string);       // Move operator
    bool operator<(const string_t &m_string) const; // Comparator
    bool operator>(const string_t &m_string) const; // Comparator
    char& operator[](const size_t m_index) const;   // Subscript operator
    size_t size() const { return length; }          // String length

private:
    char *ptr;
    size_t length;
    string_t *self;

friend std::ostream& operator<<(std::ostream &m_os, const string_t &m_string);
};

inline bool string_t::operator<(const string_t &m_string) const {
    return strcmp(ptr, m_string.ptr) < 0;
}

inline bool string_t::operator>(const string_t &m_string) const {
    return strcmp(ptr, m_string.ptr) > 0;
}

inline char& string_t::operator[](const size_t m_index) const {
    return ptr[m_index];
}

inline std::ostream& operator<<(std::ostream &m_os, const string_t &m_string) {
    return m_os << m_string.ptr;
}
