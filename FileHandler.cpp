// Implementation of all file I/O operations

#include "FileHandler.h"
#include <fstream>
#include <iostream>
using namespace std;
// LOAD FUNCTIONS

void FileHandler::loadPatients(const char* filename, Storage<Patient>& storage) {
    ifstream file(filename);
    if (!file.is_open()) return;

    char line[1024];
    while (file.getline(line, 1024)) {
        MyString::trim(line);
        if (MyString::isEmpty(line)) continue;
        Patient p;
        p.fromCSV(line);
        if (p.getId() > 0) {
            storage.add(p);
        }
    }
    file.close();
}

void FileHandler::loadDoctors(const char* filename, Storage<Doctor>& storage) {
    ifstream file(filename);
    if (!file.is_open()) return;

    char line[1024];
    while (file.getline(line, 1024)) {
        MyString::trim(line);
        if (MyString::isEmpty(line)) continue;
        Doctor d;
        d.fromCSV(line);
        if (d.getId() > 0) {
            storage.add(d);
        }
    }
    file.close();
}

void FileHandler::loadAdmins(const char* filename, Storage<Admin>& storage) {
    ifstream file(filename);
    if (!file.is_open()) return;

    char line[1024];
    while (file.getline(line, 1024)) {
        MyString::trim(line);
        if (MyString::isEmpty(line)) continue;
        Admin a;
        a.fromCSV(line);
        if (a.getId() > 0) {
            storage.add(a);
        }
    }
    file.close();
}

void FileHandler::loadAppointments(const char* filename, Storage<Appointment>& storage) {
    ifstream file(filename);
    if (!file.is_open()) return;

    char line[1024];
    while (file.getline(line, 1024)) {
        MyString::trim(line);
        if (MyString::isEmpty(line)) continue;
        Appointment a;
        a.fromCSV(line);
        if (a.getId() > 0) {
            storage.add(a);
        }
    }
    file.close();
}

void FileHandler::loadBills(const char* filename, Storage<Bill>& storage) {
    ifstream file(filename);
    if (!file.is_open()) return;

    char line[1024];
    while (file.getline(line, 1024)) {
        MyString::trim(line);
        if (MyString::isEmpty(line)) continue;
        Bill b;
        b.fromCSV(line);
        if (b.getId() > 0) {
            storage.add(b);
        }
    }
    file.close();
}

void FileHandler::loadPrescriptions(const char* filename, Storage<Prescription>& storage) {
    ifstream file(filename);
    if (!file.is_open()) return;

    char line[2048];
    while (file.getline(line, 2048)) {
        MyString::trim(line);
        if (MyString::isEmpty(line)) continue;
        Prescription p;
        p.fromCSV(line);
        if (p.getId() > 0) {
            storage.add(p);
        }
    }
    file.close();
}

// SAVE ALL FUNCTIONS 

void FileHandler::savePatients(const char* filename, const Storage<Patient>& storage) {
    ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return;

    char buffer[1024];
    for (int i = 0; i < storage.size(); i++) {
        storage[i].toCSV(buffer);
        file << buffer << "\n";
    }
    file.close();
}

void FileHandler::saveDoctors(const char* filename, const Storage<Doctor>& storage) {
    ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return;

    char buffer[1024];
    for (int i = 0; i < storage.size(); i++) {
        storage[i].toCSV(buffer);
        file << buffer << "\n";
    }
    file.close();
}

void FileHandler::saveAdmins(const char* filename, const Storage<Admin>& storage) {
    ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return;

    char buffer[1024];
    for (int i = 0; i < storage.size(); i++) {
        storage[i].toCSV(buffer);
        file << buffer << "\n";
    }
    file.close();
}

void FileHandler::saveAppointments(const char* filename, const Storage<Appointment>& storage) {
    ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return;

    char buffer[1024];
    for (int i = 0; i < storage.size(); i++) {
        storage[i].toCSV(buffer);
        file << buffer << "\n";
    }
    file.close();
}

void FileHandler::saveBills(const char* filename, const Storage<Bill>& storage) {
    ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return;

    char buffer[1024];
    for (int i = 0; i < storage.size(); i++) {
        storage[i].toCSV(buffer);
        file << buffer << "\n";
    }
    file.close();
}

void FileHandler::savePrescriptions(const char* filename, const Storage<Prescription>& storage) {
    ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) return;

    char buffer[2048];
    for (int i = 0; i < storage.size(); i++) {
        storage[i].toCSV(buffer);
        file << buffer << "\n";
    }
    file.close();
}

//APPEND FUNCTIONS

void FileHandler::appendPatient(const char* filename, const Patient& p) {
    ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;
    char buffer[1024];
    p.toCSV(buffer);
    file << buffer << "\n";
    file.close();
}

void FileHandler::appendDoctor(const char* filename, const Doctor& d) {
    ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;
    char buffer[1024];
    d.toCSV(buffer);
    file << buffer << "\n";
    file.close();
}

void FileHandler::appendAppointment(const char* filename, const Appointment& a) {
    ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;
    char buffer[1024];
    a.toCSV(buffer);
    file << buffer << "\n";
    file.close();
}

void FileHandler::appendBill(const char* filename, const Bill& b) {
    ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;
    char buffer[1024];
    b.toCSV(buffer);
    file << buffer << "\n";
    file.close();
}

void FileHandler::appendPrescription(const char* filename, const Prescription& p) {
    ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;
    char buffer[2048];
    p.toCSV(buffer);
    file << buffer << "\n";
    file.close();
}

// Append a raw text line to a file
void FileHandler::appendLine(const char* filename, const char* line) {
    ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;
    file << line << "\n";
    file.close();
}

// Read and display all lines from a file
bool FileHandler::readAndDisplayFile(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    char line[1024];
    bool hasContent = false;
    while (file.getline(line, 1024)) {
        MyString::trim(line);
        if (!MyString::isEmpty(line)) {
            std::cout << line << "\n";
            hasContent = true;
        }
    }
    file.close();
    return hasContent;
}

// Check if file exists
bool FileHandler::fileExists(const char* filename) {
  ifstream file(filename);
    bool exists = file.is_open();
    if (exists) file.close();
    return exists;
}

// Create empty file if it doesn't exist
void FileHandler::ensureFileExists(const char* filename) {
    if (!fileExists(filename)) {
        ofstream file(filename);
        file.close();
    }
}
