// InsufficientFundsException.h
// Exception thrown when patient's balance is less than the required amount

#ifndef INSUFFICIENTFUNDSEXCEPTION_H
#define INSUFFICIENTFUNDSEXCEPTION_H

#include "HospitalException.h"

class InsufficientFundsException : public HospitalException {
public:
    // i have imlemented this constructor in cpp file 
    InsufficientFundsException(float required, float available);

    // Keep the defaulted destructor (inline); remove the out-of-class definition.
    ~InsufficientFundsException() override = default;
};

#endif