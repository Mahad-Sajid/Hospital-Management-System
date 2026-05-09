#pragma once
#ifndef DOCTOR_H
#define DOCTOR_H

#include "Person.h"

class Doctor : public Person {
private:
    char specialization[50];
    float fee;

public:
    // Constructors
    Doctor();
    Doctor(int id, const char* name, const char* specialization,
        const char* contact, const char* password, float fee);

    // Destructor
    ~Doctor();

    // Pure virtual implementations
    void display(std::ostream& os) const override;
    void toCSV(char* buffer) const override;

    // Parse from CSV line
    void fromCSV(const char* line);

    // Getters
    const char* getSpecialization() const;
    float getFee() const;

    // Setters
    void setSpecialization(const char* spec);
    void setFee(float fee);

    // Operator overloads
    // == to compare by ID
    bool operator==(const Doctor& other) const;

    //operator overloading if << for output function
    friend std::ostream& operator<<(std::ostream& os, const Doctor& d);
};

#endif
