#pragma once
// Patient.h
// Represents a registered patient, inherits from Person

#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"
using namespace std;
class Patient : public Person {
private:
    int age;
    char gender[5];
    float balance;

public:
    // Constructors
    Patient();
    Patient(int id, const char* name, int age, const char* gender,
        const char* contact, const char* password, float balance);

    // Destructor
    ~Patient();

    // Pure virtual implementations
    void display(ostream& os) const override;
    void toCSV(char* buffer) const override;

    // Parse from CSV line
    void fromCSV(const char* line);

    // Getters
    int getAge() const;
    const char* getGender() const;
    float getBalance() const;

    // Setters
    void setAge(int age);
    void setGender(const char* gender);
    void setBalance(float balance);

    // Operator overloads
    // += to add to balance
    Patient& operator+=(float amount);

    // -= to deduct from balance
    Patient& operator-=(float amount);

    // == to compare by ID
    bool operator==(const Patient& other) const;

    // << for formatted console output
    friend ostream& operator<<(ostream& os, const Patient& p);
};

#endif
