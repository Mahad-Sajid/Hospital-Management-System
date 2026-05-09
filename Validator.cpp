// Validator.cpp
// Implementation of input validation logic

#include "Validator.h"
#include <ctime>

// Validate that a string represents a valid positive integer ID
bool Validator::isValidId(const char* input) {
    if (MyString::isEmpty(input)) return false;
    int i = 0;
    while (input[i] != '\0') {
        if (!MyString::isDigit(input[i])) return false;
        i++;
    }
    int val = MyString::toInt(input);
    return val > 0;
}

// Validate date in DD-MM-YYYY format
bool Validator::isValidDate(const char* date) {
    if (MyString::isEmpty(date)) return false;
    if (MyString::length(date) != 10) return false;

    // Check format: DD-MM-YYYY
    if (date[2] != '-' || date[5] != '-') return false;

    // Check all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!MyString::isDigit(date[i])) return false;
    }

    // Extract day, month, year
    char dayStr[3] = { date[0], date[1], '\0' };
    char monthStr[3] = { date[3], date[4], '\0' };
    char yearStr[5] = { date[6], date[7], date[8], date[9], '\0' };

    int day = MyString::toInt(dayStr);
    int month = MyString::toInt(monthStr);
    int year = MyString::toInt(yearStr);

    // Validate ranges
    if (month < 1 || month > 12) return false;
    if (year < getCurrentYear()) return false;

    int maxDay = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        maxDay = 30;
    }
    else if (month == 2) {
        bool leapYear = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
        maxDay = leapYear ? 29 : 28;
    }

    if (day < 1 || day > maxDay) return false;

    return true;
}

// Validate time slot is one of the 8 valid slots
bool Validator::isValidTimeSlot(const char* slot) {
    const char* validSlots[] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"
    };
    for (int i = 0; i < 8; i++) {
        if (MyString::compare(slot, validSlots[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Validate contact: exactly 11 digits, all numeric
bool Validator::isValidContact(const char* contact) {
    if (MyString::isEmpty(contact)) return false;
    if (MyString::length(contact) != 11) return false;
    for (int i = 0; i < 11; i++) {
        if (!MyString::isDigit(contact[i])) return false;
    }
    return true;
}

// Validate password: minimum 6 characters
bool Validator::isValidPassword(const char* password) {
    if (MyString::isEmpty(password)) return false;
    return MyString::length(password) >= 6;
}

// Validate positive float value
bool Validator::isValidPositiveFloat(const char* input) {
    if (MyString::isEmpty(input)) return false;
    bool hasDot = false;
    bool hasDigit = false;
    int i = 0;

    while (input[i] != '\0') {
        if (input[i] == '.') {
            if (hasDot) return false; // Multiple dots
            hasDot = true;
        }
        else if (MyString::isDigit(input[i])) {
            hasDigit = true;
        }
        else {
            return false; // Invalid character
        }
        i++;
    }

    if (!hasDigit) return false;
    float val = MyString::toFloat(input);
    return val > 0.0f;
}

// Validate menu choice within a range
bool Validator::isValidMenuChoice(const char* input, int minChoice, int maxChoice) {
    if (MyString::isEmpty(input)) return false;
    // Check all digits
    int i = 0;
    while (input[i] != '\0') {
        if (!MyString::isDigit(input[i])) return false;
        i++;
    }
    int choice = MyString::toInt(input);
    return choice >= minChoice && choice <= maxChoice;
}

int Validator::getCurrentYear() {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);
    return t.tm_year + 1900;
}

void Validator::getTodayDate(char* buffer) {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);
    strftime(buffer, 11, "%d-%m-%Y", &t);
}

// Get current timestamp for logging: DD-MM-YYYY HH:MM
void Validator::getCurrentTimestamp(char* buffer) {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);

    strftime(buffer, 25, "%d-%m-%Y %H:%M:%S", &t);
}
// Compare two dates in DD-MM-YYYY format
int Validator::compareDates(const char* date1, const char* date2) {
    // Extract year, month, day for both dates
    char y1[5] = { date1[6], date1[7], date1[8], date1[9], '\0' };
    char m1[3] = { date1[3], date1[4], '\0' };
    char d1[3] = { date1[0], date1[1], '\0' };

    char y2[5] = { date2[6], date2[7], date2[8], date2[9], '\0' };
    char m2[3] = { date2[3], date2[4], '\0' };
    char d2[3] = { date2[0], date2[1], '\0' };

    int year1 = MyString::toInt(y1), year2 = MyString::toInt(y2);
    int month1 = MyString::toInt(m1), month2 = MyString::toInt(m2);
    int day1 = MyString::toInt(d1), day2 = MyString::toInt(d2);

    if (year1 != year2) return year1 - year2;
    if (month1 != month2) return month1 - month2;
    return day1 - day2;
}

// Calculate approximate days difference between two DD-MM-YYYY dates
int Validator::daysDifference(const char* date1, const char* date2) {
    // Parse both dates
    char y1[5] = { date1[6], date1[7], date1[8], date1[9], '\0' };
    char m1[3] = { date1[3], date1[4], '\0' };
    char d1[3] = { date1[0], date1[1], '\0' };

    char y2[5] = { date2[6], date2[7], date2[8], date2[9], '\0' };
    char m2[3] = { date2[3], date2[4], '\0' };
    char d2[3] = { date2[0], date2[1], '\0' };

    // Create tm structures
    struct tm t1 = {};
    t1.tm_mday = MyString::toInt(d1);
    t1.tm_mon = MyString::toInt(m1) - 1;
    t1.tm_year = MyString::toInt(y1) - 1900;

    struct tm t2 = {};
    t2.tm_mday = MyString::toInt(d2);
    t2.tm_mon = MyString::toInt(m2) - 1;
    t2.tm_year = MyString::toInt(y2) - 1900;

    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);

    double diff = difftime(time1, time2);
    return (int)(diff / (60 * 60 * 24));
}
