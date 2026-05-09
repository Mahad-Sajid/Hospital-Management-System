#pragma once
// HospitalException.h
// Base class for all custom exceptions in the Hospital Management System

#ifndef HOSPITALEXCEPTION_H
#define HOSPITALEXCEPTION_H

#include "MyString.h"

class HospitalException {
protected:
    char message[200]; 

public:
    // Default constructor
    HospitalException();

    // Parameterized constructor
    HospitalException(const char* msg);

    // Virtual destructor for proper cleanup in derived classes
    virtual ~HospitalException();

    // Returns the error message
    virtual const char* what() const;
};

#endif
