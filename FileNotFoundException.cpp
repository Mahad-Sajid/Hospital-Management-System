// FileNotFoundException.cpp
#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException(const char* filename) {
    MyString::copy(message, "File not found: ");
    MyString::concat(message, filename);
}

FileNotFoundException::~FileNotFoundException() {}
