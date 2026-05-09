#pragma once
// Person.h
// Abstract base class for all user types (Patient, Doctor, Admin)

#ifndef PERSON_H
#define PERSON_H

#include <iostream>
#include "MyString.h"

class Person {
protected:
    int id;
    char name[50];
    char password[50];
    char contact[15];

public:
    // Constructors
    Person();
    Person(int id, const char* name, const char* password, const char* contact);

    // Virtual destructor
    virtual ~Person();

    // Pure virtual methods
    virtual void display(std::ostream& os) const = 0;
    virtual void toCSV(char* buffer) const = 0;

    // Getters
    int getId() const;
    const char* getName() const;
    const char* getPassword() const;
    const char* getContact() const;

    // Setters
    void setId(int id);
    void setName(const char* name);
    void setPassword(const char* password);
    void setContact(const char* contact);

    // Validate password
    bool checkPassword(const char* pwd) const;
};

#endif
