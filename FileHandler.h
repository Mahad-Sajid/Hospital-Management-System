#pragma once
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"

class FileHandler {
public:
    // Load functions — read all records from files into Storage objects
    static void loadPatients(const char* filename, Storage<Patient>& storage);
    static void loadDoctors(const char* filename, Storage<Doctor>& storage);
    static void loadAdmins(const char* filename, Storage<Admin>& storage);
    static void loadAppointments(const char* filename, Storage<Appointment>& storage);
    static void loadBills(const char* filename, Storage<Bill>& storage);
    static void loadPrescriptions(const char* filename, Storage<Prescription>& storage);

    // Save all records — rewrite entire file from Storage
    static void savePatients(const char* filename, const Storage<Patient>& storage);
    static void saveDoctors(const char* filename, const Storage<Doctor>& storage);
    static void saveAdmins(const char* filename, const Storage<Admin>& storage);
    static void saveAppointments(const char* filename, const Storage<Appointment>& storage);
    static void saveBills(const char* filename, const Storage<Bill>& storage);
    static void savePrescriptions(const char* filename, const Storage<Prescription>& storage);

    // Append a single record to a file
    static void appendPatient(const char* filename, const Patient& p);
    static void appendDoctor(const char* filename, const Doctor& d);
    static void appendAppointment(const char* filename, const Appointment& a);
    static void appendBill(const char* filename, const Bill& b);
    static void appendPrescription(const char* filename, const Prescription& p);

    // Append a raw line to a file (for security log, discharged, etc.)
    static void appendLine(const char* filename, const char* line);

    // Read and display all lines from a file (for security log)
    static bool readAndDisplayFile(const char* filename);

    // Check if file exists/can be opened
    static bool fileExists(const char* filename);

    // Create empty file if it doesn't exist
    static void ensureFileExists(const char* filename);
};

#endif
