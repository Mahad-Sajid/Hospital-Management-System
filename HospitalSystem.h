#pragma once
// HospitalSystem.h
// Central struct holding all Storage objects for the system

#ifndef HOSPITALSYSTEM_H
#define HOSPITALSYSTEM_H

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"

// File path constants
class FilePaths {
    public:
    static const char* PATIENTS;
    static const char* DOCTORS;
    static const char* ADMINS;
    static const char* APPOINTMENTS;
    static const char* BILLS;
    static const char* PRESCRIPTIONS;
    static const char* SECURITY_LOG;
    static const char* DISCHARGED;
};

// Central data container for the entire hospital system
class HospitalSystem {
    public:
    Storage<Patient> patients;
    Storage<Doctor> doctors;
    Storage<Admin> admins;
    Storage<Appointment> appointments;
    Storage<Bill> bills;
    Storage<Prescription> prescriptions;
};

#endif
