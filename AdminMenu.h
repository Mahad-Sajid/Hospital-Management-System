#pragma once
// AdminMenu.h
// Admin menu functions

#ifndef ADMINMENU_H
#define ADMINMENU_H

#include "HospitalSystem.h"

// Display admin menu and handle choices
void runAdminMenu(HospitalSystem& sys);

// Individual menu actions
void addDoctor(HospitalSystem& sys);
void removeDoctor(HospitalSystem& sys);
void viewAllPatients(HospitalSystem& sys);
void viewAllDoctors(HospitalSystem& sys);
void viewAllAppointments(HospitalSystem& sys);
void viewUnpaidBills(HospitalSystem& sys);
void dischargePatient(HospitalSystem& sys);
void viewSecurityLog();
void generateDailyReport(HospitalSystem& sys);

#endif
