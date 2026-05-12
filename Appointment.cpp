#include "Appointment.h"
using namespace std;
// Default constructor
Appointment::Appointment() : appointmentId(0), patientId(0), doctorId(0) {
    date[0] = '\0';
    timeSlot[0] = '\0';
    MyString::copy(status, "pending");
}

// Parameterized constructor
Appointment::Appointment(int appointmentId, int patientId, int doctorId,
    const char* date, const char* timeSlot, const char* status)
    : appointmentId(appointmentId), patientId(patientId), doctorId(doctorId) {
    MyString::copy(this->date, date, 15);
    MyString::copy(this->timeSlot, timeSlot, 10);
    MyString::copy(this->status, status, 20);
}

// Destructor
Appointment::~Appointment() {}

// comma separated values for file storage
void Appointment::toCSV(char* buffer) const {
    char idBuf[10], pIdBuf[10], dIdBuf[10];
    MyString::fromInt(appointmentId, idBuf);
    MyString::fromInt(patientId, pIdBuf);
    MyString::fromInt(doctorId, dIdBuf);
    MyString::copy(buffer, idBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, pIdBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, dIdBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, date);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, timeSlot);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, status);
}

// Parse from CSV line
void Appointment::fromCSV(const char* line) {
    char fields[6][500];
    int count = MyString::split(line, fields, 6);
    if (count >= 6) {
        appointmentId = MyString::toInt(fields[0]);
        patientId = MyString::toInt(fields[1]);
        doctorId = MyString::toInt(fields[2]);
        MyString::copy(date, fields[3], 15);
        MyString::copy(timeSlot, fields[4], 10);
        MyString::copy(status, fields[5], 20);
    }
}

// Getters
int Appointment::getId() const { return appointmentId; }
int Appointment::getPatientId() const { return patientId; }
int Appointment::getDoctorId() const { return doctorId; }
const char* Appointment::getDate() const { return date; }
const char* Appointment::getTimeSlot() const { return timeSlot; }
const char* Appointment::getStatus() const { return status; }

// Setters
void Appointment::setStatus(const char* status) { MyString::copy(this->status, status, 20); }

// == checks scheduling conflict: same doctor, same date, same time, neither cancelled
bool Appointment::operator==(const Appointment& other) const {
    if (MyString::compare(status, "cancelled") == 0) return false;
    if (MyString::compare(other.status, "cancelled") == 0) return false;

    // Check same doctor, same date, same time slot
    return (doctorId == other.doctorId &&
        MyString::compare(date, other.date) == 0 &&
        MyString::compare(timeSlot, other.timeSlot) == 0);
}

// << for formatted output
ostream& operator<<(ostream& os, const Appointment& a) {
    os << "Appointment ID: " << a.appointmentId
        << " | Patient ID: " << a.patientId
        << " | Doctor ID: " << a.doctorId
        << " | Date: " << a.date
        << " | Time: " << a.timeSlot
        << " | Status: " << a.status;
    return os;
}
