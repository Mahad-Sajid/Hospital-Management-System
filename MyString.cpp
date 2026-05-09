// MyString.cpp
// Implementation of custom string utility functions

#include "MyString.h"

// Returns length of null-terminated string
int MyString::length(const char* s) {
    if (s == nullptr) return 0;
    int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

// Copy src into dest (null-terminated)
void MyString::copy(char* dest, const char* src) {
    if (dest == nullptr || src == nullptr) return;
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Copy at most maxLen chars, always null-terminate
void MyString::copy(char* dest, const char* src, int maxLen) {
    if (dest == nullptr || src == nullptr) return;
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Append src to end of dest
void MyString::concat(char* dest, const char* src) {
    if (dest == nullptr || src == nullptr) return;
    int destLen = length(dest);
    int i = 0;
    while (src[i] != '\0') {
        dest[destLen + i] = src[i];
        i++;
    }
    dest[destLen + i] = '\0';
}

// Compare: 0 if equal, <0 if a<b, >0 if a>b
int MyString::compare(const char* a, const char* b) {
    if (a == nullptr && b == nullptr) return 0;
    if (a == nullptr) return -1;
    if (b == nullptr) return 1;
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return a[i] - b[i];
        i++;
    }
    return a[i] - b[i];
}

// Case-insensitive compare using manual toLower
int MyString::compareIgnoreCase(const char* a, const char* b) {
    if (a == nullptr && b == nullptr) return 0;
    if (a == nullptr) return -1;
    if (b == nullptr) return 1;
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        char la = toLower(a[i]);
        char lb = toLower(b[i]);
        if (la != lb) return la - lb;
        i++;
    }
    return toLower(a[i]) - toLower(b[i]);
}

// Convert single char to lowercase
char MyString::toLower(char c) {
    if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
    return c;
}

// Convert single char to uppercase
char MyString::toUpper(char c) {
    if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
    return c;
}

// Convert string to integer
int MyString::toInt(const char* s) {
    if (s == nullptr) return 0;
    int result = 0;
    int i = 0;
    bool negative = false;

    // Skip whitespace
    while (s[i] == ' ' || s[i] == '\t') i++;

    if (s[i] == '-') {
        negative = true;
        i++;
    }
    else if (s[i] == '+') {
        i++;
    }

    while (s[i] != '\0' && isDigit(s[i])) {
        result = result * 10 + (s[i] - '0');
        i++;
    }

    return negative ? -result : result;
}

// Convert string to float
float MyString::toFloat(const char* s) {
    if (s == nullptr) return 0.0f;
    float result = 0.0f;
    float decimal = 0.0f;
    float divisor = 1.0f;
    int i = 0;
    bool negative = false;
    bool hasDecimal = false;

    // Skip whitespace
    while (s[i] == ' ' || s[i] == '\t') i++;

    if (s[i] == '-') {
        negative = true;
        i++;
    }
    else if (s[i] == '+') {
        i++;
    }

    while (s[i] != '\0') {
        if (s[i] == '.') {
            hasDecimal = true;
            i++;
            continue;
        }
        if (!isDigit(s[i])) break;

        if (hasDecimal) {
            divisor *= 10.0f;
            decimal += (s[i] - '0') / divisor;
        }
        else {
            result = result * 10.0f + (s[i] - '0');
        }
        i++;
    }

    result += decimal;
    return negative ? -result : result;
}

// Convert integer to string
void MyString::fromInt(int n, char* buffer) {
    if (buffer == nullptr) return;

    if (n == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    bool negative = false;
    if (n < 0) {
        negative = true;
        n = -n;
    }

    // Build digits in reverse
    char temp[20];
    int i = 0;
    while (n > 0) {
        temp[i++] = '0' + (n % 10);
        n /= 10;
    }

    int j = 0;
    if (negative) buffer[j++] = '-';

    // Reverse the digits
    for (int k = i - 1; k >= 0; k--) {
        buffer[j++] = temp[k];
    }
    buffer[j] = '\0';
}

// Convert float to string with 2 decimal places
void MyString::fromFloat(float f, char* buffer) {
    if (buffer == nullptr) return;

    if (f < 0) {
        buffer[0] = '-';
        fromFloat(-f, buffer + 1);
        return;
    }

    int intPart = (int)f;
    // Round to 2 decimal places
    float decPart = f - intPart;
    int decInt = (int)(decPart * 100 + 0.5f);

    // Handle rounding overflow (e.g., 0.999... -> 1.00)
    if (decInt >= 100) {
        intPart++;
        decInt -= 100;
    }

    fromInt(intPart, buffer);
    int len = length(buffer);
    buffer[len] = '.';
    buffer[len + 1] = '0' + (decInt / 10);
    buffer[len + 2] = '0' + (decInt % 10);
    buffer[len + 3] = '\0';
}

// Check if character is a digit
bool MyString::isDigit(char c) {
    return c >= '0' && c <= '9';
}

// Trim leading and trailing whitespace/newline
void MyString::trim(char* s) {
    if (s == nullptr) return;

    int len = length(s);
    if (len == 0) return;

    // Remove trailing whitespace
    int end = len - 1;
    while (end >= 0 && (s[end] == ' ' || s[end] == '\t' || s[end] == '\n' || s[end] == '\r')) {
        end--;
    }
    s[end + 1] = '\0';

    // Remove leading whitespace
    int start = 0;
    while (s[start] == ' ' || s[start] == '\t' || s[start] == '\n' || s[start] == '\r') {
        start++;
    }

    if (start > 0) {
        int i = 0;
        while (s[start + i] != '\0') {
            s[i] = s[start + i];
            i++;
        }
        s[i] = '\0';
    }
}

// Check if string is empty or null
bool MyString::isEmpty(const char* s) {
    return s == nullptr || s[0] == '\0';
}

// Split a line by delimiter into fields array
int MyString::split(const char* line, char fields[][500], int maxFields, char delimiter) {
    if (line == nullptr || maxFields <= 0) return 0;

    int fieldCount = 0;
    int i = 0;
    int j = 0;

    while (line[i] != '\0' && fieldCount < maxFields) {
        if (line[i] == delimiter) {
            fields[fieldCount][j] = '\0';
            trim(fields[fieldCount]);
            fieldCount++;
            j = 0;
            i++;
        }
        else {
            if (j < 499) {
                fields[fieldCount][j] = line[i];
                j++;
            }
            i++;
        }
    }

    // Last field
    if (fieldCount < maxFields) {
        fields[fieldCount][j] = '\0';
        trim(fields[fieldCount]);
        fieldCount++;
    }

    return fieldCount;
}

// Find first occurrence of character, returns index or -1
int MyString::findChar(const char* s, char c) {
    if (s == nullptr) return -1;
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == c) return i;
        i++;
    }
    return -1;
}

// Copy substring from src[start] of length len into dest
void MyString::substring(const char* src, char* dest, int start, int len) {
    if (src == nullptr || dest == nullptr) return;
    int srcLen = length(src);
    if (start >= srcLen) {
        dest[0] = '\0';
        return;
    }
    int i = 0;
    while (i < len && src[start + i] != '\0') {
        dest[i] = src[start + i];
        i++;
    }
    dest[i] = '\0';
}
