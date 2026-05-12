#pragma once
// Admin.h
// Represents the system administrator

#ifndef ADMIN_H
#define ADMIN_H
using namespace std;
#include "Person.h"

class Admin : public Person {
public:
    // Constructors
    Admin();
    Admin(int id, const char* name, const char* password);

    // Destructor
    ~Admin();

    // Pure virtual implementations
    void display(ostream& os) const override;
    void toCSV(char* buffer) const override;

    // Parse from CSV line
    void fromCSV(const char* line);
};

#endif
