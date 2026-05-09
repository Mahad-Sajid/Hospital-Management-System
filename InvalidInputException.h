#pragma once
// InvalidInputException.h
// Exception thrown when user input fails validation

#ifndef INVALIDINPUTEXCEPTION_H
#define INVALIDINPUTEXCEPTION_H

#include "HospitalException.h"

class InvalidInputException : public HospitalException {
public:
    InvalidInputException(const char* msg);
    ~InvalidInputException();
};

#endif
