#include <iostream>
#include "array.h"

const char* msg[10] = {"This", "is", "the", "day",
                       "that", "the", "Lord", "has", "made.", 0};

int main(int argc, char** argv) {
    // Create a class instance, X.
    array_t X;

    // Push back input strings into X.
    for(int i = 0; msg[i]; i++) { X.push_back(msg[i]); }

    // Create a class instance, Y, as a copy of X.
    array_t Y = X;

    // Replace the last string in Y, and add a few more strings.
    if(Y.size()) { Y[Y.size()-1] = "made;"; }
    Y.push_back("let");
    Y.push_back("us");
    Y.push_back("rejoice,");
    Y.push_back("and");
    Y.push_back("be");
    Y.push_back("glad");
    Y.push_back("in");
    Y.push_back("it.");

    // Print the string message stored in X and Y.
    std::cout << "X (size=" << X.size() << ", capacity=" << X.capacity() << "): ";
    for(size_t i = 0; i < X.size(); i++) { std::cout << X[i] << " "; }
    std::cout << std::endl;

    std::cout << "Y (size=" << Y.size() << ", capacity=" << Y.capacity() << "): ";
    for(size_t i = 0; i < Y.size(); i++) { std::cout << Y[i] << " "; }
    std::cout << std::endl;

    return 0;
}
