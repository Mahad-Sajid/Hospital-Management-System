#pragma once
// Represents a charge generated from an appointment
#ifndef BILL_H
#define BILL_H

#include <iostream>
#include "MyString.h"
using namespace std;
class Bill {
private:
    int billId;
    int patientId;
    int appointmentId;
    float amount;
    char status[20]; 
    char date[15];    

public:
    // Constructors
    Bill();
    Bill(int billId, int patientId, int appointmentId,float amount, const char* status, const char* date);

    // Destructor
    ~Bill();

    // Serialize to CSV
    void toCSV(char* buffer) const;

    // Parse from CSV line
    void fromCSV(const char* line);

    // Getters
    int getId() const;
    int getPatientId() const;
    int getAppointmentId() const;
    float getAmount() const;
    const char* getStatus() const;
    const char* getDate() const;

    // Setters
    void setStatus(const char* status);

    // == to compare by ID
    bool operator==(const Bill& other) const;   

    // << for formatted output
    friend ostream& operator<<(ostream& os, const Bill& b);
};

#endif
