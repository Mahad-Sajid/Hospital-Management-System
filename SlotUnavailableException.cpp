// SlotUnavailableException.cpp
#include "SlotUnavailableException.h"

SlotUnavailableException::SlotUnavailableException(const char* slot, const char* date) {
    MyString::copy(message, "Time slot ");
    MyString::concat(message, slot);
    MyString::concat(message, " is already booked on ");
    MyString::concat(message, date);
    MyString::concat(message, ".");
}

SlotUnavailableException::~SlotUnavailableException() {}
