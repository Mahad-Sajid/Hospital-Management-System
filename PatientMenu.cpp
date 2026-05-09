// PatientMenu.cpp
// Implementation of all patient menu operations

#include "PatientMenu.h"
#include "FileHandler.h"
#include "Validator.h"
#include "InsufficientFundsException.h"
#include "InvalidInputException.h"
#include "SlotUnavailableException.h"
#include <iostream>
using namespace std;
// Display patient menu and handle choices
void runPatientMenu(HospitalSystem& sys, Patient& patient) {
    char choiceBuf[10];
    bool running = true;

    while (running) {
        // Display menu header with patient info
        char balBuf[20];
        MyString::fromFloat(patient.getBalance(), balBuf);

        cout << "\nWelcome, " << patient.getName() << "\n";
        cout << "Balance: PKR " << balBuf << "\n";
        cout << "========================\n";
        cout << "1. Book Appointment\n";
        cout << "2. Cancel Appointment\n";
        cout << "3. View My Appointments\n";
        cout << "4. View My Medical Records\n";
        cout << "5. View My Bills\n";
        cout << "7. Top Up Balance\n";
        cout << "8. Logout\n";
        cout << "Enter choice: ";
        cin.getline(choiceBuf, 10);
        MyString::trim(choiceBuf);

        if (!Validator::isValidMenuChoice(choiceBuf, 1, 8)) {
            cout << "Invalid choice. Please try again.\n";
            continue;
        }

        int choice = MyString::toInt(choiceBuf);
        switch (choice) {
        case 1: bookAppointment(sys, patient); break;
        case 2: cancelAppointment(sys, patient); break;
        case 3: viewMyAppointments(sys, patient); break;
        case 4: viewMyMedicalRecords(sys, patient); break;
        case 5: viewMyBills(sys, patient); break;
        case 6: payBill(sys, patient); break;
        case 7: topUpBalance(sys, patient); break;
        case 8: running = false; cout << "Logging out...\n"; break;
        }
    }
}
// 1. Book Appointment
void bookAppointment(HospitalSystem& sys, Patient& patient) {
    // Step 1: Get specialization
    char specBuf[50];
    cout << "\nEnter specialization to search (e.g. Cardiology): ";
    cin.getline(specBuf, 50);
    MyString::trim(specBuf);

    // Step 2: Find matching doctors (case-insensitive)
    int matchedIds[100];
    int matchCount = 0;

    for (int i = 0; i < sys.doctors.size(); i++) {
        if (MyString::compareIgnoreCase(sys.doctors[i].getSpecialization(), specBuf) == 0) {
            matchedIds[matchCount++] = sys.doctors[i].getId();
        }
    }

    if (matchCount == 0) {
        cout << "No doctors available for that specialization.\n";
        return;
    }

    // Display matched doctors
        cout << "\nAvailable Doctors:\n";
    cout << "ID | Name | Fee\n";
    cout << "---|------|----\n";
    for (int i = 0; i < matchCount; i++) {
        Doctor* doc = sys.doctors.findById(matchedIds[i]);
        if (doc) {
            char feeBuf[20];
            MyString::fromFloat(doc->getFee(), feeBuf);
            cout << doc->getId() << " | " << doc->getName()
                << " | PKR " << feeBuf << "\n";
        }
    }

    // Step 3: Select doctor
    char docIdBuf[10];
    cout << "Enter Doctor ID: ";
    cin.getline(docIdBuf, 10);
    MyString::trim(docIdBuf);

    int docId = MyString::toInt(docIdBuf);
    Doctor* selectedDoc = sys.doctors.findById(docId);
    if (!selectedDoc) {
        std::cout << "Doctor not found.\n";
        return;
    }

    // Step 4: Get date with validation (3 attempts)
    char dateBuf[15];
    bool validDate = false;
    for (int attempt = 0; attempt < 3; attempt++) {
        cout << "Enter date (DD-MM-YYYY): ";
        cin.getline(dateBuf, 15);
        MyString::trim(dateBuf);

        if (Validator::isValidDate(dateBuf)) {
            validDate = true;
            break;
        }
        cout << "Invalid date. Use format DD-MM-YYYY.\n";
    }
    if (!validDate) {
        cout << "Too many invalid attempts. Returning to menu.\n";
        return;
    }

    // Step 5: Display available time slots
    const char* allSlots[] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"
    };

    // Determine which slots are available
    auto displayAvailableSlots = [&]() {
        cout << "\nAvailable time slots for " << dateBuf << ":\n";
        bool anyAvailable = false;
        for (int s = 0; s < 8; s++) {
            bool taken = false;
            for (int a = 0; a < sys.appointments.size(); a++) {
                const Appointment& appt = sys.appointments[a];
                if (appt.getDoctorId() == docId &&
                    MyString::compare(appt.getDate(), dateBuf) == 0 &&
                    MyString::compare(appt.getTimeSlot(), allSlots[s]) == 0 &&
                    MyString::compare(appt.getStatus(), "cancelled") != 0) {
                    taken = true;
                    break;
                }
            }
            if (!taken) {
                cout << "  " << allSlots[s] << "\n";
                anyAvailable = true;
            }
        }
        return anyAvailable;
        };

    if (!displayAvailableSlots()) {
        cout << "No available slots for this date.\n";
        return;
    }

    // Step 6: Select time slot
    char slotBuf[10];
    bool slotSelected = false;
    for (int attempt = 0; attempt < 3; attempt++) {
        cout << "Enter time slot (e.g. 09:00): ";
        cin.getline(slotBuf, 10);
        MyString::trim(slotBuf);

        if (!Validator::isValidTimeSlot(slotBuf)) {
            cout << "Invalid time slot.\n";
            continue;
        }

        // Check if slot is taken using Appointment == operator
        Appointment testAppt(0, patient.getId(), docId, dateBuf, slotBuf, "pending");
        bool conflict = false;
        for (int a = 0; a < sys.appointments.size(); a++) {
            if (sys.appointments[a] == testAppt) {
                conflict = true;
                break;
            }
        }

        if (conflict) {
            try {
                throw SlotUnavailableException(slotBuf, dateBuf);
            }
            catch (const SlotUnavailableException& e) {
                cout << e.what() << "\n";
                displayAvailableSlots();
                continue;
            }
        }

        slotSelected = true;
        break;
    }

    if (!slotSelected) {
            cout << "Too many invalid attempts. Returning to menu.\n";
        return;
    }

    // Step 7: Get doctor fee for the unpaid bill
    float fee = selectedDoc->getFee();

    // Step 8: Create appointment
    int newApptId = sys.appointments.getMaxId() + 1;
    Appointment newAppt(newApptId, patient.getId(), docId, dateBuf, slotBuf, "pending");
    sys.appointments.add(newAppt);
    FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);

    // Step 9: Create bill
    int newBillId = sys.bills.getMaxId() + 1;
    Bill newBill(newBillId, patient.getId(), newApptId, fee, "unpaid", dateBuf);
    sys.bills.add(newBill);
    FileHandler::saveBills(FilePaths::BILLS, sys.bills);

    std::cout << "Appointment booked successfully. Appointment ID: " << newApptId << ".\n";
}

// 2. Cancel Appointment
void cancelAppointment(HospitalSystem& sys, Patient& patient) {
    // Find pending appointments for this patient
    int pendingIds[100];
    int pendingCount = 0;

    for (int i = 0; i < sys.appointments.size(); i++) {
        const Appointment& a = sys.appointments[i];
        if (a.getPatientId() == patient.getId() &&
            MyString::compare(a.getStatus(), "pending") == 0) {
            pendingIds[pendingCount++] = a.getId();
        }
    }

    if (pendingCount == 0) {
        cout << "You have no pending appointments.\n";
        return;
    }

    // Display pending appointments
        cout << "\nPending Appointments:\n";
       cout << "Appointment ID | Doctor Name | Date | Time Slot\n";
    cout << "---------------|-------------|------|----------\n";
    for (int i = 0; i < pendingCount; i++) {
        Appointment* a = sys.appointments.findById(pendingIds[i]);
        if (a) {
            Doctor* doc = sys.doctors.findById(a->getDoctorId());
            const char* docName = doc ? doc->getName() : "Unknown";
            cout << a->getId() << " | " << docName
                << " | " << a->getDate() << " | " << a->getTimeSlot() << "\n";
        }
    }

    // Get appointment ID to cancel
    char idBuf[10];
    cout << "Enter Appointment ID to cancel: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int apptId = MyString::toInt(idBuf);

    // Validate: belongs to this patient and is pending
    Appointment* appt = sys.appointments.findById(apptId);
    if (!appt || appt->getPatientId() != patient.getId() ||
        MyString::compare(appt->getStatus(), "pending") != 0) {
        cout << "Invalid appointment ID.\n";
        return;
    }

    // Update appointment status to cancelled
    appt->setStatus("cancelled");
    FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);

    // Update bill status to cancelled
    for (int i = 0; i < sys.bills.size(); i++) {
        Bill& b = sys.bills.getAt(i);
        if (b.getAppointmentId() == apptId) {
            b.setStatus("cancelled");
            break;
        }
    }
    FileHandler::saveBills(FilePaths::BILLS, sys.bills);

    cout << "Appointment cancelled successfully.\n";
}
// 3. View My Appointments (sorted by date ascending)
void viewMyAppointments(HospitalSystem& sys, const Patient& patient) {
    // Collect patient's appointments
    int indices[100];
    int count = 0;
    for (int i = 0; i < sys.appointments.size(); i++) {
        if (sys.appointments[i].getPatientId() == patient.getId()) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "No appointments found.\n";
        return;
    }

    // Bubble sort by date ascending
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (Validator::compareDates(
                sys.appointments[indices[j]].getDate(),
                sys.appointments[indices[j + 1]].getDate()) > 0) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }

    cout << "\nYour Appointments:\n";
    cout << "ID | Doctor Name | Specialization | Date | Time Slot | Status\n";
    cout << "---|-------------|----------------|------|-----------|-------\n";
    for (int i = 0; i < count; i++) {
        const Appointment& a = sys.appointments[indices[i]];
        const Doctor* doc = sys.doctors.findById(a.getDoctorId());
        cout << a.getId() << " | "
            << (doc ? doc->getName() : "Unknown") << " | "
            << (doc ? doc->getSpecialization() : "Unknown") << " | "
            << a.getDate() << " | "
            << a.getTimeSlot() << " | "
            << a.getStatus() << "\n";
    }
}

// 4. View My Medical Records (sorted by date descending)
void viewMyMedicalRecords(HospitalSystem& sys, const Patient& patient) {
    // Collect patient's prescriptions
    int indices[100];
    int count = 0;
    for (int i = 0; i < sys.prescriptions.size(); i++) {
        if (sys.prescriptions[i].getPatientId() == patient.getId()) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "No medical records found.\n";
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

    cout << "\nYour Medical Records:\n";
    cout << "Date | Doctor Name | Medicines | Notes\n";
    cout << "-----|-------------|-----------|------\n";
    for (int i = 0; i < count; i++) {
        const Prescription& p = sys.prescriptions[indices[i]];
        const Doctor* doc = sys.doctors.findById(p.getDoctorId());
        cout << p.getDate() << " | "
            << (doc ? doc->getName() : "Unknown") << " | "
            << p.getMedicines() << " | "
            << p.getNotes() << "\n";
    }
}
// 5. View My Bills
void viewMyBills(HospitalSystem& sys, const Patient& patient) {
    int count = 0;
    float totalUnpaid = 0.0f;

    cout << "\nYour Bills:\n";
    cout << "Bill ID | Appointment ID | Amount (PKR) | Status | Date\n";
    cout << "--------|----------------|--------------|--------|-----\n";

    for (int i = 0; i < sys.bills.size(); i++) {
        const Bill& b = sys.bills[i];
        if (b.getPatientId() == patient.getId()) {
            char amtBuf[20];
            MyString::fromFloat(b.getAmount(), amtBuf);
            cout << b.getId() << " | " << b.getAppointmentId()
                << " | PKR " << amtBuf
                << " | " << b.getStatus()
                << " | " << b.getDate() << "\n";
            if (MyString::compare(b.getStatus(), "unpaid") == 0) {
                totalUnpaid += b.getAmount();
            }
            count++;
        }
    }

    if (count == 0) {
        cout << "No bills found.\n";
    }
    else {
        char totalBuf[20];
        MyString::fromFloat(totalUnpaid, totalBuf);
        cout << "\nTotal Outstanding: PKR " << totalBuf << "\n";
    }
}
// 6. Pay Bill
void payBill(HospitalSystem& sys, Patient& patient) {
    // Display unpaid bills
    int unpaidIds[100];
    int unpaidCount = 0;

    for (int i = 0; i < sys.bills.size(); i++) {
        const Bill& b = sys.bills[i];
        if (b.getPatientId() == patient.getId() &&
            MyString::compare(b.getStatus(), "unpaid") == 0) {
            unpaidIds[unpaidCount++] = b.getId();
        }
    }

    if (unpaidCount == 0) {
        cout << "No unpaid bills.\n";
        return;
    }

    cout << "\nUnpaid Bills:\n";
    cout << "Bill ID | Amount (PKR) | Date\n";
    cout << "--------|--------------|-----\n";
    for (int i = 0; i < unpaidCount; i++) {
        Bill* b = sys.bills.findById(unpaidIds[i]);
        if (b) {
            char amtBuf[20];
            MyString::fromFloat(b->getAmount(), amtBuf);
            cout << b->getId() << " | PKR " << amtBuf
                << " | " << b->getDate() << "\n";
        }
    }

    // Get bill ID
    char idBuf[10];
    cout << "Enter Bill ID to pay: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int billId = MyString::toInt(idBuf);

    Bill* bill = sys.bills.findById(billId);
    if (!bill || bill->getPatientId() != patient.getId() ||
        MyString::compare(bill->getStatus(), "unpaid") != 0) {
        cout << "Invalid Bill ID.\n";
        return;
    }

    // Check balance
    if (patient.getBalance() < bill->getAmount()) {
        try {
            throw InsufficientFundsException(bill->getAmount(), patient.getBalance());
        }
        catch (const InsufficientFundsException& e) {
            cout << e.what() << "\n";
            return;
        }
    }

    // Deduct using -= operator
    patient -= bill->getAmount();
    sys.patients.update(patient);
    FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);

    // Update bill status
    bill->setStatus("paid");
    FileHandler::saveBills(FilePaths::BILLS, sys.bills);

    char balBuf[20];
    MyString::fromFloat(patient.getBalance(), balBuf);
    cout << "Bill paid successfully. Remaining balance: PKR " << balBuf << ".\n";
}
// 7. Top Up Balance
void topUpBalance(HospitalSystem& sys, Patient& patient) {
    char amtBuf[20];

    for (int attempt = 0; attempt < 3; attempt++) {
        cout << "Enter amount to add (PKR): ";
        cin.getline(amtBuf, 20);
        MyString::trim(amtBuf);

        try {
            if (!Validator::isValidPositiveFloat(amtBuf)) {
                throw InvalidInputException("Invalid amount. Please enter a positive number.");
            }

            float amount = MyString::toFloat(amtBuf);
            // Use += operator
            patient += amount;
            sys.patients.update(patient);
            FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);

            char balBuf[20];
            MyString::fromFloat(patient.getBalance(), balBuf);
            cout << "Balance updated. New balance: PKR " << balBuf << "\n";
            return;

        }
        catch (const InvalidInputException& e) {
            cout << e.what() << "\n";
        }
    }
    cout << "Too many invalid attempts. Returning to menu.\n";
}
