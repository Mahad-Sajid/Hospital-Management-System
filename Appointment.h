#pragma once
//it will represent a single appointment between a doctor and a patient
#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>
#include "MyString.h"
using namespace std;
class Appointment {
private:
    int appointmentId;
    int patientId;
    int doctorId;
    char date[15];      
    char timeSlot[10];   
    char status[20];   

public:
    // Constructors
    Appointment();
    Appointment(int appointmentId, int patientId, int doctorId,
        const char* date, const char* timeSlot, const char* status);

    // Destructor
    ~Appointment();

    // Serialize to CSV
    void toCSV(char* buffer) const;

    // Parse from CSV line
    void fromCSV(const char* line);

    // Getters
    int getId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    const char* getStatus() const;

    // Setters
    void setStatus(const char* status);

    // Operator overloads
    // == to detect scheduling conflict (same doctor, same date, same time, neither cancelled)
    bool operator==(const Appointment& other) const;

    // << for formatted output
    friend ostream& operator<<(ostream& os, const Appointment& a);
};

#endif
