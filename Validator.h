#pragma once
// Validator.h
// The only class permitted to contain input validation logic

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "MyString.h"

class Validator {
public:
    // Validate that a string represents a valid positive integer ID
    static bool isValidId(const char* input);

    // Validate date in DD-MM-YYYY format
    static bool isValidDate(const char* date);

    // Validate time slot is one of the 8 valid slots
    static bool isValidTimeSlot(const char* slot);

    // Validate contact number: exactly 11 digits, all numeric
    static bool isValidContact(const char* contact);

    // Validate password: minimum 6 characters
    static bool isValidPassword(const char* password);

    // Validate positive float value
    static bool isValidPositiveFloat(const char* input);

    // Validate menu choice within a range
    static bool isValidMenuChoice(const char* input, int minChoice, int maxChoice);

    // Get current year from system time
    static int getCurrentYear();

    // Get today's date in DD-MM-YYYY format
    static void getTodayDate(char* buffer);

    // Get current timestamp for logging
    static void getCurrentTimestamp(char* buffer);

    // Compare two dates in DD-MM-YYYY format
    static int compareDates(const char* date1, const char* date2);

    // Calculate days difference between two dates (approximate)
    static int daysDifference(const char* date1, const char* date2);
};

#endif
    
