#include "HospitalSystem.h"
#include "FileHandler.h"
#include "GUI.h"
#include<windows.h>

// FilePaths definitions
const char* FilePaths::PATIENTS = "patients.txt";
const char* FilePaths::DOCTORS = "doctors.txt";
const char* FilePaths::ADMINS = "admin.txt";
const char* FilePaths::APPOINTMENTS = "appointments.txt";
const char* FilePaths::BILLS = "bills.txt";
const char* FilePaths::PRESCRIPTIONS = "prescriptions.txt";
const char* FilePaths::SECURITY_LOG = "security_log.txt";
const char* FilePaths::DISCHARGED = "discharged.txt";

int main() {
    SetConsoleOutputCP(65001);
    HospitalSystem sys;

    try {
        FileHandler::ensureFileExists(FilePaths::PATIENTS);
        FileHandler::ensureFileExists(FilePaths::DOCTORS);
        FileHandler::ensureFileExists(FilePaths::ADMINS);
        FileHandler::ensureFileExists(FilePaths::APPOINTMENTS);
        FileHandler::ensureFileExists(FilePaths::BILLS);
        FileHandler::ensureFileExists(FilePaths::PRESCRIPTIONS);
        FileHandler::ensureFileExists(FilePaths::SECURITY_LOG);
        FileHandler::ensureFileExists(FilePaths::DISCHARGED);

        FileHandler::loadPatients(FilePaths::PATIENTS, sys.patients);
        FileHandler::loadDoctors(FilePaths::DOCTORS, sys.doctors);
        FileHandler::loadAdmins(FilePaths::ADMINS, sys.admins);
        FileHandler::loadAppointments(FilePaths::APPOINTMENTS, sys.appointments);
        FileHandler::loadBills(FilePaths::BILLS, sys.bills);
        FileHandler::loadPrescriptions(FilePaths::PRESCRIPTIONS, sys.prescriptions);
    }
    catch (...) {}

    GUI gui(sys);
    gui.run();

    return 0;
}