
#include "DoctorMenu.h"
#include "FileHandler.h"
#include "Validator.h"
#include <iostream>
#include <limits>
using namespace std;    

void runDoctorMenu(HospitalSystem& sys, Doctor& doctor) {
    char choiceBuf[10];
    bool running = true;

    while (running) {
        cout << "\nWelcome, " << doctor.getName()
            << "  |  Specialization: " << doctor.getSpecialization() << "\n";
        cout << "===============================================\n";
        cout << "1. View Today's Appointments\n";
        cout << "2. Mark Appointment Complete\n";
        cout << "3. Mark Appointment No-Show\n";
        cout << "4. Write Prescription\n";
        cout << "5. View Patient Medical History\n";
        cout << "6. Logout\n";
        cout << "Enter choice: ";
            cin.getline(choiceBuf, 10);
        MyString::trim(choiceBuf);

        if (!Validator::isValidMenuChoice(choiceBuf, 1, 6)) {
            cout << "Invalid choice. Please try again.\n";
            continue;
        }

        int choice = MyString::toInt(choiceBuf);
        switch (choice) {
        case 1: viewTodaysAppointments(sys, doctor); break;
        case 2: markAppointmentComplete(sys, doctor); break;
        case 3: markAppointmentNoShow(sys, doctor); break;
        case 4: writePrescription(sys, doctor); break;
        case 5: viewPatientMedicalHistory(sys, doctor); break;
        case 6: running = false;cout << "Logging out...\n"; break;
        }
    }
}
void viewTodaysAppointments(HospitalSystem& sys, const Doctor& doctor) {
    char today[15];
    Validator::getTodayDate(today);

    // Collect today's appointments for this doctor
    int indices[100];
    int count = 0;
    for (int i = 0; i < sys.appointments.size(); i++) {
        const Appointment& a = sys.appointments[i];
        if (a.getDoctorId() == doctor.getId() &&
            MyString::compare(a.getDate(), today) == 0) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "No appointments scheduled for today.\n";
        return;
    }

    // Bubble sort by time slot ascending
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (MyString::compare(
                sys.appointments[indices[j]].getTimeSlot(),
                sys.appointments[indices[j + 1]].getTimeSlot()) > 0) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }

    cout << "\nToday's Appointments (" << today << "):\n";
    cout << "Appointment ID | Patient Name | Time Slot | Status\n";
    cout << "---------------|--------------|-----------|-------\n";
    for (int i = 0; i < count; i++) {
        const Appointment& a = sys.appointments[indices[i]];
        const Patient* pat = sys.patients.findById(a.getPatientId());
        cout << a.getId() << " | "
            << (pat ? pat->getName() : "Unknown") << " | "
            << a.getTimeSlot() << " | "
            << a.getStatus() << "\n";
    }
}
void markAppointmentComplete(HospitalSystem& sys, const Doctor& doctor) {
    char today[15];
    Validator::getTodayDate(today);

    // Find today's pending appointments for this doctor
    int pendingIds[100];
    int pendingCount = 0;
    for (int i = 0; i < sys.appointments.size(); i++) {
        const Appointment& a = sys.appointments[i];
        if (a.getDoctorId() == doctor.getId() &&
            MyString::compare(a.getDate(), today) == 0 &&
            MyString::compare(a.getStatus(), "pending") == 0) {
            pendingIds[pendingCount++] = a.getId();
        }
    }

    if (pendingCount == 0) {
        cout << "No pending appointments for today.\n";
        return;
    }

    // Display pending
    cout << "\nToday's Pending Appointments:\n";
    cout << "Appointment ID | Patient Name | Time Slot\n";
    cout << "---------------|--------------|----------\n";
    for (int i = 0; i < pendingCount; i++) {
        Appointment* a = sys.appointments.findById(pendingIds[i]);
        if (a) {
            const Patient* pat = sys.patients.findById(a->getPatientId());
            cout << a->getId() << " | "
                << (pat ? pat->getName() : "Unknown") << " | "
                << a->getTimeSlot() << "\n";
        }
    }

    char idBuf[10];
    cout << "Enter Appointment ID: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int apptId = MyString::toInt(idBuf);

    Appointment* appt = sys.appointments.findById(apptId);
    if (!appt || appt->getDoctorId() != doctor.getId() ||
        MyString::compare(appt->getDate(), today) != 0 ||
        MyString::compare(appt->getStatus(), "pending") != 0) {
        cout << "Invalid appointment ID.\n";
        return;
    }

    appt->setStatus("completed");
    FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);
    cout << "Appointment marked as completed.\n";
}
void markAppointmentNoShow(HospitalSystem& sys, const Doctor& doctor) {
    char today[15];
    Validator::getTodayDate(today);

    // Find today's pending appointments for this doctor
    int pendingIds[100];
    int pendingCount = 0;
    for (int i = 0; i < sys.appointments.size(); i++) {
        const Appointment& a = sys.appointments[i];
        if (a.getDoctorId() == doctor.getId() &&
            MyString::compare(a.getDate(), today) == 0 &&
            MyString::compare(a.getStatus(), "pending") == 0) {
            pendingIds[pendingCount++] = a.getId();
        }
    }

    if (pendingCount == 0) {
        cout << "No pending appointments for today.\n";
        return;
    }

    // Display pending
    cout << "\nToday's Pending Appointments:\n";
    cout << "Appointment ID | Patient Name | Time Slot\n";
    cout << "---------------|--------------|----------\n";
    for (int i = 0; i < pendingCount; i++) {
        Appointment* a = sys.appointments.findById(pendingIds[i]);
        if (a) {
            const Patient* pat = sys.patients.findById(a->getPatientId());
                    cout << a->getId() << " | "
                << (pat ? pat->getName() : "Unknown") << " | "
                << a->getTimeSlot() << "\n";
        }
    }

    char idBuf[10];
    cout << "Enter Appointment ID: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int apptId = MyString::toInt(idBuf);

    Appointment* appt = sys.appointments.findById(apptId);
    if (!appt || appt->getDoctorId() != doctor.getId() ||
        MyString::compare(appt->getDate(), today) != 0 ||
        MyString::compare(appt->getStatus(), "pending") != 0) {
        cout << "Invalid appointment ID.\n";
        return;
    }

    // Mark as noshow
    appt->setStatus("noshow");
    FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);

    // Cancel the corresponding bill (no refund)
    for (int i = 0; i < sys.bills.size(); i++) {
        Bill& b = sys.bills.getAt(i);
        if (b.getAppointmentId() == apptId) {
            b.setStatus("cancelled");
            break;
        }
    }
    FileHandler::saveBills(FilePaths::BILLS, sys.bills);

    cout << "Appointment marked as no-show.\n";
}
void writePrescription(HospitalSystem& sys, const Doctor& doctor) {
    char idBuf[10];
    cout << "Enter Appointment ID: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int apptId = MyString::toInt(idBuf);

    // Validate: belongs to this doctor and is completed
    Appointment* appt = sys.appointments.findById(apptId);
    if (!appt || appt->getDoctorId() != doctor.getId() ||
        MyString::compare(appt->getStatus(), "completed") != 0) {
        cout << "Invalid appointment ID or appointment not completed.\n";
        return;
    }

    // Check if prescription already exists for this appointment
    for (int i = 0; i < sys.prescriptions.size(); i++) {
        if (sys.prescriptions[i].getAppointmentId() == apptId) {
            cout << "Prescription already written for this appointment.\n";
            return;
        }
    }

    // Get medicines
    char medicines[500];
    cout << "Enter medicines (format: MedicineName Dosage; e.g. Paracetamol 500mg;Amoxicillin 250mg): ";
    cin.getline(medicines, 500);
    if (cin.fail()) {
        cin.clear();
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }

    // Get notes
    char notes[300];
    cout << "Enter notes (max 300 chars): ";
    cin.getline(notes, 300);
    if (cin.fail()) {
        cin.clear();
            cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }

    // Generate prescription ID
    int newPrescId = sys.prescriptions.getMaxId() + 1;

    // Create prescription
    Prescription newPresc(newPrescId, apptId, appt->getPatientId(),
        doctor.getId(), appt->getDate(), medicines, notes);
    sys.prescriptions.add(newPresc);
    FileHandler::savePrescriptions(FilePaths::PRESCRIPTIONS, sys.prescriptions);

        cout << "Prescription saved.\n";
}
void viewPatientMedicalHistory(HospitalSystem& sys, const Doctor& doctor) {
    char idBuf[10];
    cout << "Enter Patient ID: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int patientId = MyString::toInt(idBuf);

    // Check patient exists
    Patient* pat = sys.patients.findById(patientId);
    if (!pat) {
        cout << "Access denied. You can only view records of your own patients.\n";
        return;
    }

    // Check if patient has at least one completed appointment with this doctor
    bool hasCompleted = false;
    for (int i = 0; i < sys.appointments.size(); i++) {
        const Appointment& a = sys.appointments[i];
        if (a.getPatientId() == patientId &&
            a.getDoctorId() == doctor.getId() &&
            MyString::compare(a.getStatus(), "completed") == 0) {
            hasCompleted = true;
            break;
        }
    }

    if (!hasCompleted) {
        cout << "Access denied. You can only view records of your own patients.\n";
        return;
    }

    // Collect prescriptions for this patient written by this doctor
    int indices[100];
    int count = 0;
    for (int i = 0; i < sys.prescriptions.size(); i++) {
        if (sys.prescriptions[i].getPatientId() == patientId &&
            sys.prescriptions[i].getDoctorId() == doctor.getId()) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "No prescriptions found for this patient.\n";
        return;
    }

    // Bubble sort by date descending
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (Validator::compareDates(
                sys.prescriptions[indices[j]].getDate(),
                sys.prescriptions[indices[j + 1]].getDate()) < 0) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }

    cout << "\nMedical History for " << pat->getName() << ":\n";
    cout << "Date | Medicines | Notes\n";
    cout << "-----|-----------|------\n";
    for (int i = 0; i < count; i++) {
        const Prescription& p = sys.prescriptions[indices[i]];
        cout << p.getDate() << " | "
            << p.getMedicines() << " | "
            << p.getNotes() << "\n";
    }
}
