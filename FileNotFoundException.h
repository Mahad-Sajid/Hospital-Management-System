#pragma once
// FileNotFoundException.h
// Exception thrown when a required file cannot be opened on startup
#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H

#include "HospitalException.h"

class FileNotFoundException : public HospitalException {
public:
    // Constructor takes the filename that was not found
    FileNotFoundException(const char* filename);
    ~FileNotFoundException();
};

#endif
