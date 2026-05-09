// InsufficientFundsException.cpp   
#include "InsufficientFundsException.h"

InsufficientFundsException::InsufficientFundsException(float required, float available) {
    char reqBuf[20], avBuf[20];
    MyString::fromFloat(required, reqBuf);
    MyString::fromFloat(available, avBuf);
    MyString::copy(message, "Insufficient funds. Required: PKR ");
    MyString::concat(message, reqBuf);
    MyString::concat(message, ", Available: PKR ");
    MyString::concat(message, avBuf);
}

// defaulted destructor is made in the header file, so we don't need to define it here.