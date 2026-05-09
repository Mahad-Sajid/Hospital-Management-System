// Prescription.cpp
// Implementation of Prescription class

#include "Prescription.h"

// Default constructor
Prescription::Prescription()
    : prescriptionId(0), appointmentId(0), patientId(0), doctorId(0) {
    date[0] = '\0';
    medicines[0] = '\0';
    notes[0] = '\0';
}

// Parameterized constructor
Prescription::Prescription(int prescriptionId, int appointmentId, int patientId, int doctorId,
    const char* date, const char* medicines, const char* notes)
    : prescriptionId(prescriptionId), appointmentId(appointmentId),
    patientId(patientId), doctorId(doctorId) {
    MyString::copy(this->date, date, 15);
    MyString::copy(this->medicines, medicines, 500);
    MyString::copy(this->notes, notes, 300);
}

// Destructor
Prescription::~Prescription() {}

// Serialize to CSV
void Prescription::toCSV(char* buffer) const {
    char idBuf[10], aIdBuf[10], pIdBuf[10], dIdBuf[10];
    MyString::fromInt(prescriptionId, idBuf);
    MyString::fromInt(appointmentId, aIdBuf);
    MyString::fromInt(patientId, pIdBuf);
    MyString::fromInt(doctorId, dIdBuf);

    MyString::copy(buffer, idBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, aIdBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, pIdBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, dIdBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, date);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, medicines);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, notes);
}

// Parse from CSV line 
void Prescription::fromCSV(const char* line) {
    char fields[7][500];
    int count = MyString::split(line, fields, 7);
    if (count >= 7) {
        prescriptionId = MyString::toInt(fields[0]);
        appointmentId = MyString::toInt(fields[1]);
        patientId = MyString::toInt(fields[2]);
        doctorId = MyString::toInt(fields[3]);
        MyString::copy(date, fields[4], 15);
        MyString::copy(medicines, fields[5], 500);
        MyString::copy(notes, fields[6], 300);
    }
}

// Getters
int Prescription::getId() const { return prescriptionId; }
int Prescription::getAppointmentId() const { return appointmentId; }
int Prescription::getPatientId() const { return patientId; }
int Prescription::getDoctorId() const { return doctorId; }
const char* Prescription::getDate() const { return date; }
const char* Prescription::getMedicines() const { return medicines; }
const char* Prescription::getNotes() const { return notes; }

// == to compare by ID
bool Prescription::operator==(const Prescription& other) const {
    return prescriptionId == other.prescriptionId;
}

// << for formatted output
std::ostream& operator<<(std::ostream& os, const Prescription& p) {
    os << "Prescription ID: " << p.prescriptionId
        << " | Appointment ID: " << p.appointmentId
        << " | Date: " << p.date
        << " | Medicines: " << p.medicines
        << " | Notes: " << p.notes;
    return os;
}
