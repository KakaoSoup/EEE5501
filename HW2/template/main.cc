#include <iostream>
#include <fstream>
#include "string.h"
#include "vector.h"

#define WORD_SIZE 16

using namespace std;

// Print the information of vector_t<string_t>.
void print(vector_t<string_t> &m_vector) {
    cout << "capacity()="   << m_vector.capacity()  << endl
         << "size()="       << m_vector.size()      << endl
         << "array=[ ";
    for(vector_t<string_t>::iterator it = m_vector.begin();
        it != m_vector.end(); it++)  { cout << *it  << " "; }
    cout << "]"                             << endl << endl;
}

int main(int argc, char **argv) {
    // Help message
    if(argc > 2) { cerr << "Usage: " << argv[0] << " <input_file>" << endl; exit(1); }

    // Open a text file.
    ifstream input_file;
    input_file.open(argc == 2 ? argv[1] : "input");
    if(!input_file.is_open()) { cerr << "Error: failed to open " << argv[1] << endl; exit(1); }

    // Create string vectors.
    vector_t<string_t> words, sorted;

    // Push elements to the "words" vector.
    char line[WORD_SIZE];
    while(input_file.getline(line, WORD_SIZE)) { words.push_back(line); }
    cout << "#1. words (after reading from the input):" << endl; print(words);

    // Close the text file.
    input_file.close();

    // Remove words starting with 'a', 'e', 'i', 'o', 'u'.
    for(vector_t<string_t>::iterator it = words.begin(); it != words.end(); it++) {
        if(((*it)[0] == 'a') || ((*it)[0] == 'e') || ((*it)[0] == 'i') ||
           ((*it)[0] == 'o') || ((*it)[0] == 'u')) { (it = words.erase(it))--; }
    }
    cout << "#2. words (after removing vowel words):" << endl; print(words);

    // Insert elements to the "sorted" vector in the ascending order.
    sorted.reserve(words.size());
    for(size_t i = 0; i < words.size(); i++) {
        vector_t<string_t>::iterator it = sorted.begin();
        while((it != sorted.end()) && (words[i] > *it)) { it++; }
        sorted.insert(it, words[i]);
    }
    cout << "#3. sorted (after sorting):" << endl; print(sorted);

    // Remove words starting with 'z'.
    while(sorted.size() && (sorted[sorted.size()-1][0] == 'z')) { sorted.pop_back(); }
    cout << "#4. sorted (after removing words starting with z):" << endl; print(sorted);

    // Resize the "words" vector. 
    words.resize(50);
    cout << "#5. words (after resizing):" << endl; print(words);

    // Resize the "sorted" vector.
    sorted.resize(80, "seahawk");
    cout << "#6. sorted (after resizing):" << endl; print(sorted);

    // Resize the "sorted" vector.
    sorted.resize(85, "seahawk");
    cout << "#7. sorted (after another resizing):" << endl; print(sorted);

    // Blow up everything.
    sorted.clear();
    cout << "#8. sorted (after clearing):" << endl; print(sorted);

    // Copy the empty vector.
    words = sorted;
    cout << "#9. words (after copying from sorted):" << endl; print(words);

    return 0;
}
