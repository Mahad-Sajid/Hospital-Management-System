
#include "Doctor.h"

// Default constructor
Doctor::Doctor() : Person(), fee(0.0f) {
    specialization[0] = '\0';
}

// Parameterized constructor
Doctor::Doctor(int id, const char* name, const char* specialization,
    const char* contact, const char* password, float fee)
    : Person(id, name, password, contact), fee(fee) {
    MyString::copy(this->specialization, specialization, 50);
}

// Destructor
Doctor::~Doctor() {}

// Display doctor info
void Doctor::display(std::ostream& os) const {
    char feeBuf[20];
    MyString::fromFloat(fee, feeBuf);
    os << "Doctor ID: " << id << "\n"
        << "Name: " << name << "\n"
        << "Specialization: " << specialization << "\n"
        << "Contact: " << contact << "\n"
        << "Fee: PKR " << feeBuf << "\n";
}

// Serialize to CSV
void Doctor::toCSV(char* buffer) const {
    char idBuf[10], feeBuf[20];
    MyString::fromInt(id, idBuf);
    MyString::fromFloat(fee, feeBuf);
    MyString::copy(buffer, idBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, name);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, specialization);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, contact);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, password);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, feeBuf);
}

// Parse from CSV line
void Doctor::fromCSV(const char* line) {
    char fields[6][500];
    int count = MyString::split(line, fields, 6);
    if (count >= 6) {
        id = MyString::toInt(fields[0]);
        MyString::copy(name, fields[1], 50);
        MyString::copy(specialization, fields[2], 50);
        MyString::copy(contact, fields[3], 15);
        MyString::copy(password, fields[4], 50);
        fee = MyString::toFloat(fields[5]);
    }
}

// Getters
const char* Doctor::getSpecialization() const { return specialization; }
float Doctor::getFee() const { return fee; }

// Setters
void Doctor::setSpecialization(const char* spec) { MyString::copy(specialization, spec, 50); }
void Doctor::setFee(float fee) { this->fee = fee; }

// == to compare by ID
bool Doctor::operator==(const Doctor& other) const {
    return id == other.id;
}

// << for formatted console output
std::ostream& operator<<(std::ostream& os, const Doctor& d) {
    d.display(os);
    return os;
}
