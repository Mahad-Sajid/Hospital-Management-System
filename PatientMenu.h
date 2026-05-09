#pragma once

#ifndef PATIENTMENU_H
#define PATIENTMENU_H

#include "HospitalSystem.h"

// Display patient menu and handle choices
void runPatientMenu(HospitalSystem& sys, Patient& patient);

// Individual menu actions
void bookAppointment(HospitalSystem& sys, Patient& patient);
void cancelAppointment(HospitalSystem& sys, Patient& patient);
void viewMyAppointments(HospitalSystem& sys, const Patient& patient);
void viewMyMedicalRecords(HospitalSystem& sys, const Patient& patient);
void viewMyBills(HospitalSystem& sys, const Patient& patient);
void payBill(HospitalSystem& sys, Patient& patient);
void topUpBalance(HospitalSystem& sys, Patient& patient);

#endif
