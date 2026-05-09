// SlotUnavailableException.h
// Exception thrown when a patient attempts to book an already-occupied time slot

#pragma once

#ifndef SLOTUNAVAILABLEEXCEPTION_H
#define SLOTUNAVAILABLEEXCEPTION_H

#include "HospitalException.h"

class SlotUnavailableException : public HospitalException {
public:
	// Accepts a message string for checking in case we want to provide more details in the future
    SlotUnavailableException(const char* msg) : HospitalException(msg) {}
    // Optionally, keep the original signature for future use
    SlotUnavailableException(const char* slot, const char* date);
    ~SlotUnavailableException();
};

#endif