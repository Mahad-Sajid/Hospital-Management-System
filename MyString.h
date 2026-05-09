#pragma once
#ifndef MYSTRING_H
#define MYSTRING_H

class MyString {
public:
    // Returns length of null-terminated string
    static int length(const char* s);

    // Copy src into dest (null-terminated)
    static void copy(char* dest, const char* src);

    // Copy at most maxLen chars, always null-terminate
    static void copy(char* dest, const char* src, int maxLen);

    // Append src to end of dest
    static void concat(char* dest, const char* src);

    // Compare: 0 if equal, <0 if a<b, >0 if a>b
    static int compare(const char* a, const char* b);

    // Case-insensitive compare
    static int compareIgnoreCase(const char* a, const char* b);

    // Convert single char to lowercase
    static char toLower(char c);

    // Convert single char to uppercase
    static char toUpper(char c);

    // Convert string to integer
    static int toInt(const char* s);

    // Convert string to float
    static float toFloat(const char* s);

    // Convert integer to string
    static void fromInt(int n, char* buffer);

    // Convert float to string with 2 decimal places
    static void fromFloat(float f, char* buffer);

    // Check if character is a digit
    static bool isDigit(char c);

    // Trim leading and trailing whitespace/newline
    static void trim(char* s);

    // Check if string is empty or null
    static bool isEmpty(const char* s);

    // Split a line by delimiter into fields array
    // Returns number of fields found
    static int split(const char* line, char fields[][500], int maxFields, char delimiter = ',');

    // Find first occurrence of character, returns index or -1
    static int findChar(const char* s, char c);

    // Copy substring from src[start] of length len into dest
    static void substring(const char* src, char* dest, int start, int len);
};

#endif
