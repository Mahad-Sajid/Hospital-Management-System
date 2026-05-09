#include "Bill.h"

// Default constructor
Bill::Bill() : billId(0), patientId(0), appointmentId(0), amount(0.0f) {
    MyString::copy(status, "unpaid");
    date[0] = '\0';
}

// Parameterized constructor
Bill::Bill(int billId, int patientId, int appointmentId,
    float amount, const char* status, const char* date)
    : billId(billId), patientId(patientId), appointmentId(appointmentId), amount(amount) {
    MyString::copy(this->status, status, 20);
    MyString::copy(this->date, date, 15);
}

// Destructor
Bill::~Bill() {}

// Serialize to CSV
void Bill::toCSV(char* buffer) const {
    char idBuf[10], pIdBuf[10], aIdBuf[10], amtBuf[20];
    MyString::fromInt(billId, idBuf);
    MyString::fromInt(patientId, pIdBuf);
    MyString::fromInt(appointmentId, aIdBuf);
    MyString::fromFloat(amount, amtBuf);

    MyString::copy(buffer, idBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, pIdBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, aIdBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, amtBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, status);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, date);
}

// Parse from CSV line
void Bill::fromCSV(const char* line) {
    char fields[6][500];
    int count = MyString::split(line, fields, 6);
    if (count >= 6) {
        billId = MyString::toInt(fields[0]);
        patientId = MyString::toInt(fields[1]);
        appointmentId = MyString::toInt(fields[2]);
        amount = MyString::toFloat(fields[3]);
        MyString::copy(status, fields[4], 20);
        MyString::copy(date, fields[5], 15);
    }
}

// Getters
int Bill::getId() const { return billId; }
int Bill::getPatientId() const { return patientId; }
int Bill::getAppointmentId() const { return appointmentId; }
float Bill::getAmount() const { return amount; }
const char* Bill::getStatus() const { return status; }
const char* Bill::getDate() const { return date; }

// Setters
void Bill::setStatus(const char* status) { MyString::copy(this->status, status, 20); }

// == to compare by ID
bool Bill::operator==(const Bill& other) const {
    return billId == other.billId;
}

// << for formatted output
std::ostream& operator<<(std::ostream& os, const Bill& b) {
    char amtBuf[20];
    MyString::fromFloat(b.amount, amtBuf);
    os << "Bill ID: " << b.billId
        << " | Patient ID: " << b.patientId
        << " | Appointment ID: " << b.appointmentId
        << " | Amount: PKR " << amtBuf
        << " | Status: " << b.status
        << " | Date: " << b.date;
    return os;
}
