// InvalidInputException.cpp
#include "InvalidInputException.h"

InvalidInputException::InvalidInputException(const char* msg)
    : HospitalException(msg) {
}

InvalidInputException::~InvalidInputException() {}
