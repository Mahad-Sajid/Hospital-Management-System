// HospitalException.cpp
// Implementation of base exception class

#include "HospitalException.h"

// Default constructor
HospitalException::HospitalException() {
    MyString::copy(message, "An error occurred.");
}

// Parameterized constructor
HospitalException::HospitalException(const char* msg) {
    MyString::copy(message, msg, 200);
}

// destructor
HospitalException::~HospitalException() {}

// Returns the error message
const char* HospitalException::what() const {
    return message;
}
