#pragma once
// Prescription.h
// Represents medicines and notes written by a doctor for a completed appointment

#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

#include <iostream>
#include "MyString.h"
using namespace std;
class Prescription {
private:
    int prescriptionId;
    int appointmentId;
    int patientId;
    int doctorId;
    char date[15];         
    char medicines[500];  
    char notes[300];       

public:
    // Constructors
    Prescription();
    Prescription(int prescriptionId, int appointmentId, int patientId, int doctorId,
        const char* date, const char* medicines, const char* notes);

    // Destructor
    ~Prescription();

    // Serialize to CSV
    void toCSV(char* buffer) const;

    // Parse from CSV line
    void fromCSV(const char* line);

    // Getters
    int getId() const;
    int getAppointmentId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getMedicines() const;
    const char* getNotes() const;

    // == to compare by ID
    bool operator==(const Prescription& other) const;

    // << for formatted output
    friend ostream& operator<<(ostream& os, const Prescription& p);
};

#endif
