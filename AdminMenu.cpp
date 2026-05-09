#include "AdminMenu.h"
#include "FileHandler.h"
#include "Validator.h"
#include "InvalidInputException.h"
#include <iostream>
using namespace std;
void runAdminMenu(HospitalSystem& sys) {
    char choiceBuf[10];
    bool running = true;

    while (running) {
        cout << "\nAdmin Panel - MediCore\n";
        cout << "======================\n";
        cout << "1.  Add Doctor\n";
        cout << "2.  Remove Doctor\n";
        cout << "3.  View All Patients\n";
        cout << "4.  View All Doctors\n";
        cout << "5.  View All Appointments\n";
        cout << "6.  View Unpaid Bills\n";
        cout << "7.  Discharge Patient\n";
        cout << "8.  View Security Log\n";
        cout << "9.  Generate Daily Report\n";
        cout << "10.  Add Patient\n";
		cout << "11.  Remove Patient\n";
        cout << "12. Logout\n";
        cout << "Enter choice: ";
            cin.getline(choiceBuf, 10);
        MyString::trim(choiceBuf);

        if (!Validator::isValidMenuChoice(choiceBuf, 1, 12)) {
            cout << "Invalid choice. Please try again.\n";
            continue;
        }

        int choice = MyString::toInt(choiceBuf);
        switch (choice) {
        case 1: addDoctor(sys); break;
        case 2: removeDoctor(sys); break;
        case 3: viewAllPatients(sys); break;
        case 4: viewAllDoctors(sys); break;
        case 5: viewAllAppointments(sys); break;
        case 6: viewUnpaidBills(sys); break;
        case 7: dischargePatient(sys); break;
        case 8: viewSecurityLog(); break;
        case 9: generateDailyReport(sys); break;
        case 10: running = false; cout << "Logging out...\n"; break;
        }
    }
}
//add doctor
void addDoctor(HospitalSystem& sys) {
    char nameBuf[50], specBuf[50], contactBuf[15], passBuf[50], feeBuf[20];

    // Name
    std::cout << "Enter doctor name (max 50 chars): ";
    cin.getline(nameBuf, 50);
    MyString::trim(nameBuf);
    if (MyString::isEmpty(nameBuf)) {
        cout << "Name cannot be empty.\n";
        return;
    }

    // Specialization
    cout << "Enter specialization (max 50 chars): ";
    cin.getline(specBuf, 50);
    MyString::trim(specBuf);
    if (MyString::isEmpty(specBuf)) {
        cout << "Specialization cannot be empty.\n";
        return;
    }

    // Contact (exactly 11 digits)
    cout << "Enter contact (11 digits): ";
    cin.getline(contactBuf, 15);
    MyString::trim(contactBuf);
    if (!Validator::isValidContact(contactBuf)) {
        cout << "Invalid contact number. Must be exactly 11 digits.\n";
        return;
    }

    // Password (min 6 chars)
    cout << "Enter password (min 6 chars): ";
    cin.getline(passBuf, 50);
    MyString::trim(passBuf);
    if (!Validator::isValidPassword(passBuf)) {
        cout << "Invalid password. Must be at least 6 characters.\n";
        return;
    }

    // Fee (positive float)
    cout << "Enter consultation fee: ";
    cin.getline(feeBuf, 20);
    MyString::trim(feeBuf);
    if (!Validator::isValidPositiveFloat(feeBuf)) {
         cout << "Invalid fee. Must be a positive number.\n";
        return;
    }

    float fee = MyString::toFloat(feeBuf);
    int newId = sys.doctors.getMaxId() + 1;

    Doctor newDoc(newId, nameBuf, specBuf, contactBuf, passBuf, fee);
    sys.doctors.add(newDoc);
    FileHandler::saveDoctors(FilePaths::DOCTORS, sys.doctors);

    std::cout << "Doctor added successfully. ID: " << newId << ".\n";
}
// Remove Doctor

void removeDoctor(HospitalSystem& sys) {
    if (sys.doctors.size() == 0) {
        cout << "No doctors in the system.\n";
        return;
    }

    // Display all doctors
    cout << "\nAll Doctors:\n";
    cout << "ID | Name | Specialization | Fee\n";
    cout << "---|------|----------------|----\n";
    for (int i = 0; i < sys.doctors.size(); i++) {
        const Doctor& d = sys.doctors[i];
        char feeBuf[20];
        MyString::fromFloat(d.getFee(), feeBuf);
        cout << d.getId() << " | " << d.getName()
            << " | " << d.getSpecialization()
            << " | PKR " << feeBuf << "\n";
    }

    char idBuf[10];
    cout << "Enter Doctor ID to remove: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int docId = MyString::toInt(idBuf);

    Doctor* doc = sys.doctors.findById(docId);
    if (!doc) {
        cout << "Doctor not found.\n";
        return;
    }

    // Check for pending appointments
    for (int i = 0; i < sys.appointments.size(); i++) {
        if (sys.appointments[i].getDoctorId() == docId &&
            MyString::compare(sys.appointments[i].getStatus(), "pending") == 0) {
            cout << "Cannot remove doctor with pending appointments. Cancel or reassign them first.\n";
            return;
        }
    }

    sys.doctors.removeById(docId);
    FileHandler::saveDoctors(FilePaths::DOCTORS, sys.doctors);
    cout << "Doctor removed.\n";
}

// 3. View All Patients
void viewAllPatients(HospitalSystem& sys) {
    if (sys.patients.size() == 0) {
        cout << "No patients in the system.\n";
        return;
    }

    cout << "\nAll Patients:\n";
    cout << "ID | Name | Age | Gender | Contact | Balance | Unpaid Bills\n";
    cout << "---|------|-----|--------|---------|---------|------------\n";
    for (int i = 0; i < sys.patients.size(); i++) {
        const Patient& p = sys.patients[i];

        // Count unpaid bills
        int unpaidCount = 0;
        for (int j = 0; j < sys.bills.size(); j++) {
            if (sys.bills[j].getPatientId() == p.getId() &&
                MyString::compare(sys.bills[j].getStatus(), "unpaid") == 0) {
                unpaidCount++;
            }
        }

        char balBuf[20];
        MyString::fromFloat(p.getBalance(), balBuf);
        cout << p.getId() << " | " << p.getName()
            << " | " << p.getAge()
            << " | " << p.getGender()
            << " | " << p.getContact()
            << " | PKR " << balBuf
            << " | " << unpaidCount << "\n";
    }
}
// View All Doctors

void viewAllDoctors(HospitalSystem& sys) {
    if (sys.doctors.size() == 0) {
        cout << "No doctors in the system.\n";
        return;
    }

    cout << "\nAll Doctors:\n";
    cout << "ID | Name | Specialization | Contact | Fee\n";
    cout << "---|------|----------------|---------|----\n";
    for (int i = 0; i < sys.doctors.size(); i++) {
        const Doctor& d = sys.doctors[i];
        char feeBuf[20];
        MyString::fromFloat(d.getFee(), feeBuf);
        cout << d.getId() << " | " << d.getName()
            << " | " << d.getSpecialization()
            << " | " << d.getContact()
            << " | PKR " << feeBuf << "\n";
    }
}

//  View All Appointments (sorted by date descending)

void viewAllAppointments(HospitalSystem& sys) {
    if (sys.appointments.size() == 0) {
        cout << "No appointments in the system.\n";
        return;
    }

    // Create index array and sort by date descending
    int indices[100];
    int count = sys.appointments.size();
    for (int i = 0; i < count; i++) indices[i] = i;

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (Validator::compareDates(
                sys.appointments[indices[j]].getDate(),
                sys.appointments[indices[j + 1]].getDate()) < 0) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }

    cout << "\nAll Appointments:\n";
    cout << "ID | Patient Name | Doctor Name | Date | Time Slot | Status\n";
    cout << "---|--------------|-------------|------|-----------|-------\n";
    for (int i = 0; i < count; i++) {
        const Appointment& a = sys.appointments[indices[i]];
        const Patient* pat = sys.patients.findById(a.getPatientId());
        const Doctor* doc = sys.doctors.findById(a.getDoctorId());
        cout << a.getId() << " | "
            << (pat ? pat->getName() : "Unknown") << " | "
            << (doc ? doc->getName() : "Unknown") << " | "
            << a.getDate() << " | "
            << a.getTimeSlot() << " | "
            << a.getStatus() << "\n";
    }
}
// 6. View Unpaid Bills also tell which is overdue (more than 7 days old) 
void viewUnpaidBills(HospitalSystem& sys) {
    char today[15];
    Validator::getTodayDate(today);

    int count = 0;
    cout << "\nUnpaid Bills:\n";
    cout << "Bill ID | Patient Name | Amount (PKR) | Date\n";
    cout << "--------|--------------|--------------|-----\n";

    for (int i = 0; i < sys.bills.size(); i++) {
        const Bill& b = sys.bills[i];
        if (MyString::compare(b.getStatus(), "unpaid") == 0) {
            const Patient* pat = sys.patients.findById(b.getPatientId());
            char amtBuf[20];
            MyString::fromFloat(b.getAmount(), amtBuf);

            cout << b.getId() << " | "
                << (pat ? pat->getName() : "Unknown") << " | PKR "
                << amtBuf << " | " << b.getDate();

            // Check if overdue (more than 7 days old)
            int diff = Validator::daysDifference(today, b.getDate());
            if (diff > 7) {
                cout << " [OVERDUE]";
            }
            cout << "\n";
            count++;
        }
    }

    if (count == 0) {
        cout << "No unpaid bills.\n";
    }
}
//  Discharge Patient
void dischargePatient(HospitalSystem& sys) {
    char idBuf[10];
    cout << "Enter Patient ID: ";
    cin.getline(idBuf, 10);
    MyString::trim(idBuf);
    int patientId = MyString::toInt(idBuf);

    Patient* pat = sys.patients.findById(patientId);
    if (!pat) {
        cout << "Patient not found.\n";
        return;
    }

    // Check unpaid bills
    for (int i = 0; i < sys.bills.size(); i++) {
        if (sys.bills[i].getPatientId() == patientId &&
            MyString::compare(sys.bills[i].getStatus(), "unpaid") == 0) {
            std::cout << "Cannot discharge patient with unpaid bills.\n";
            return;
        }
    }

    // Check pending appointments
    for (int i = 0; i < sys.appointments.size(); i++) {
        if (sys.appointments[i].getPatientId() == patientId &&
            MyString::compare(sys.appointments[i].getStatus(), "pending") == 0) {
            cout << "Cannot discharge patient with pending appointments.\n";
            return;
        }
    }
	//adding it to discharged file
    char buffer[2048];
    pat->toCSV(buffer);
    FileHandler::appendLine(FilePaths::DISCHARGED, buffer);

    // Archive appointment records
    for (int i = 0; i < sys.appointments.size(); i++) {
        if (sys.appointments[i].getPatientId() == patientId) {
            sys.appointments[i].toCSV(buffer);
            FileHandler::appendLine(FilePaths::DISCHARGED, buffer);
        }
    }

    // Archive prescription records
    for (int i = 0; i < sys.prescriptions.size(); i++) {
        if (sys.prescriptions[i].getPatientId() == patientId) {
            sys.prescriptions[i].toCSV(buffer);
            FileHandler::appendLine(FilePaths::DISCHARGED, buffer);
        }
    }

    // Archive bill records
    for (int i = 0; i < sys.bills.size(); i++) {
        if (sys.bills[i].getPatientId() == patientId) {
            sys.bills[i].toCSV(buffer);
            FileHandler::appendLine(FilePaths::DISCHARGED, buffer);
        }
    }

    // Remove from active storage (reverse order to avoid index issues)
    // Remove bills
    for (int i = sys.bills.size() - 1; i >= 0; i--) {
        if (sys.bills[i].getPatientId() == patientId) {
            sys.bills.removeById(sys.bills[i].getId());
        }
    }

    // Remove prescriptions
    for (int i = sys.prescriptions.size() - 1; i >= 0; i--) {
        if (sys.prescriptions[i].getPatientId() == patientId) {
            sys.prescriptions.removeById(sys.prescriptions[i].getId());
        }
    }

    // Remove appointments
    for (int i = sys.appointments.size() - 1; i >= 0; i--) {
        if (sys.appointments[i].getPatientId() == patientId) {
            sys.appointments.removeById(sys.appointments[i].getId());
        }
    }

    // Remove patient
    sys.patients.removeById(patientId);

    // Save all files
    FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);
    FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);
    FileHandler::savePrescriptions(FilePaths::PRESCRIPTIONS, sys.prescriptions);
    FileHandler::saveBills(FilePaths::BILLS, sys.bills);

    cout << "Patient discharged and archived successfully.\n";
}

// 8. View Security Log
void viewSecurityLog() {
    std::cout << "\nSecurity Log:\n";
    std::cout << "=============\n";
    if (!FileHandler::readAndDisplayFile(FilePaths::SECURITY_LOG)) {
        std::cout << "No security events logged.\n";
    }
}
// 9. Generate Daily Report
void generateDailyReport(HospitalSystem& sys) {
    char today[15];
    Validator::getTodayDate(today);

    cout << "\n========== Daily Report for " << today << " ==========\n\n";

    // Count appointments by status
    int totalToday = 0, pendingCount = 0, completedCount = 0, noshowCount = 0, cancelledCount = 0;
    for (int i = 0; i < sys.appointments.size(); i++) {
        if (MyString::compare(sys.appointments[i].getDate(), today) == 0) {
            totalToday++;
            if (MyString::compare(sys.appointments[i].getStatus(), "pending") == 0) pendingCount++;
            else if (MyString::compare(sys.appointments[i].getStatus(), "completed") == 0) completedCount++;
            else if (MyString::compare(sys.appointments[i].getStatus(), "noshow") == 0) noshowCount++;
            else if (MyString::compare(sys.appointments[i].getStatus(), "cancelled") == 0) cancelledCount++;
        }
    }

    cout << "Total appointments today: " << totalToday
        << " (Pending: " << pendingCount
        << " Completed: " << completedCount
        << " No-show: " << noshowCount
        << " Cancelled: " << cancelledCount << ")\n\n";

    // Revenue collected today (paid bills with today's date)
    float revenue = 0.0f;
    for (int i = 0; i < sys.bills.size(); i++) {
        if (MyString::compare(sys.bills[i].getDate(), today) == 0 &&
            MyString::compare(sys.bills[i].getStatus(), "paid") == 0) {
            revenue += sys.bills[i].getAmount();
        }
    }
    char revBuf[20];
    MyString::fromFloat(revenue, revBuf);
    cout << "Revenue collected today (paid bills): PKR " << revBuf << "\n\n";

    // Patients with outstanding unpaid bills
    cout << "Patients with outstanding unpaid bills:\n";
    cout << "Patient Name | Total Owed\n";
    cout << "-------------|----------\n";
    bool anyUnpaid = false;

    for (int i = 0; i < sys.patients.size(); i++) {
        float totalOwed = 0.0f;
        for (int j = 0; j < sys.bills.size(); j++) {
            if (sys.bills[j].getPatientId() == sys.patients[i].getId() &&
                MyString::compare(sys.bills[j].getStatus(), "unpaid") == 0) {
                totalOwed += sys.bills[j].getAmount();
            }
        }
        if (totalOwed > 0.0f) {
            char owedBuf[20];
            MyString::fromFloat(totalOwed, owedBuf);
            cout << sys.patients[i].getName() << " | PKR " << owedBuf << "\n";
            anyUnpaid = true;
        }
    }
    if (!anyUnpaid) cout << "  None\n";
    // Doctor-wise summary for today
    cout << "\nDoctor-wise summary for today:\n";
    cout << "Doctor Name | Completed | Pending | No-show\n";
    cout << "------------|-----------|---------|--------\n";
    for (int i = 0; i < sys.doctors.size(); i++) {
        int dCompleted = 0, dPending = 0, dNoshow = 0;
        for (int j = 0; j < sys.appointments.size(); j++) {
            if (sys.appointments[j].getDoctorId() == sys.doctors[i].getId() &&
                MyString::compare(sys.appointments[j].getDate(), today) == 0) {
                if (MyString::compare(sys.appointments[j].getStatus(), "completed") == 0) dCompleted++;
                else if (MyString::compare(sys.appointments[j].getStatus(), "pending") == 0) dPending++;
                else if (MyString::compare(sys.appointments[j].getStatus(), "noshow") == 0) dNoshow++;
            }
        }
        if (dCompleted > 0 || dPending > 0 || dNoshow > 0) {
            cout << sys.doctors[i].getName() << " | "
                << dCompleted << " | " << dPending << " | " << dNoshow << "\n";
        }
    }

   cout << "\n================================================\n";
}
