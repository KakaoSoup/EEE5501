#include <iostream>

// Minimal implementation of string
class string_t {
public:
    string_t();
    string_t(const string_t& m_string);
    string_t(const char* m_char);
    ~string_t();

    string_t& operator=(const char* m_char);
    size_t size() const { return length; }

private:
    // Disable a direct assignment operator.
    string_t& operator=(const string_t& m_string) = delete;

    char* ptr;
    size_t length;
    // Self pointer to invalidate a simply memcpy of string_t
    string_t* self;

friend std::ostream& operator<<(std::ostream& m_os, const string_t& m_string);
};

inline std::ostream& operator<<(std::ostream& m_os, const string_t& m_string) {
    return m_os << m_string.ptr;
}
