#pragma once
#ifndef DOCTORMENU_H
#define DOCTORMENU_H

#include "HospitalSystem.h"

// Display doctor menu and handle choices
void runDoctorMenu(HospitalSystem& sys, Doctor& doctor);

// Individual menu actions
void viewTodaysAppointments(HospitalSystem& sys, const Doctor& doctor);
void markAppointmentComplete(HospitalSystem& sys, const Doctor& doctor);
void markAppointmentNoShow(HospitalSystem& sys, const Doctor& doctor);
void writePrescription(HospitalSystem& sys, const Doctor& doctor);
void viewPatientMedicalHistory(HospitalSystem& sys, const Doctor& doctor);

#endif
