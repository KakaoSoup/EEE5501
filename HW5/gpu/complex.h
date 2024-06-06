#ifndef __COMPLEX_H__
#define __COMPLEX_H__

template <typename T>
class Complex {
public:
    __host__ __device__ Complex(T a, T b) : r(a), i(b) {}
    __host__ __device__ T magnitude2() { return (r*r + i*i); }  // Mag^2
    __host__ __device__ Complex operator*(const Complex &c) { return Complex(r*c.r - i*c.i, i*c.r + r*c.i); }
    __host__ __device__ Complex operator+(const Complex &c) { return Complex(r+c.r, i+c.i); }

private:
    T r, i; // Real and imaginary components of a complex number
};

#endif

