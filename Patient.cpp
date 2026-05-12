// Patient.cpp
// Implementation of Patient class

#include "Patient.h"
using namespace std;
// Default constructor
Patient::Patient() : Person(), age(0), balance(0.0f) {
    gender[0] = '\0';
}

// Parameterized constructor
Patient::Patient(int id, const char* name, int age, const char* gender,
    const char* contact, const char* password, float balance)
    : Person(id, name, password, contact), age(age), balance(balance) {
    MyString::copy(this->gender, gender, 5);
}

// Destructor
Patient::~Patient() {}

// Display patient info to output stream
void Patient::display(std::ostream& os) const {
    os << "Patient ID: " << id << "\n"
        << "Name: " << name << "\n"
        << "Age: " << age << "\n"
        << "Gender: " << gender << "\n"
        << "Contact: " << contact << "\n";
    char balBuf[20];
    MyString::fromFloat(balance, balBuf);
    os << "Balance: PKR " << balBuf << "\n";
}

// Serialize to CSV format
void Patient::toCSV(char* buffer) const {
    char idBuf[10], ageBuf[10], balBuf[20];
    MyString::fromInt(id, idBuf);
    MyString::fromInt(age, ageBuf);
    MyString::fromFloat(balance, balBuf);

    MyString::copy(buffer, idBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, name);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, ageBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, gender);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, contact);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, password);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, balBuf);
}

// Parse from CSV line
void Patient::fromCSV(const char* line) {
    char fields[7][500];
    int count = MyString::split(line, fields, 7);
    if (count >= 7) {
        id = MyString::toInt(fields[0]);
        MyString::copy(name, fields[1], 50);
        age = MyString::toInt(fields[2]);
        MyString::copy(gender, fields[3], 5);
        MyString::copy(contact, fields[4], 15);
        MyString::copy(password, fields[5], 50);
        balance = MyString::toFloat(fields[6]);
    }
}

// Getters
int Patient::getAge() const { return age; }
const char* Patient::getGender() const { return gender; }
float Patient::getBalance() const { return balance; }

// Setters
void Patient::setAge(int age) { this->age = age; }
void Patient::setGender(const char* gender) { MyString::copy(this->gender, gender, 5); }
void Patient::setBalance(float balance) { this->balance = balance; }

// += to add to balance
Patient& Patient::operator+=(float amount) {
    balance += amount;
    return *this;
}

// -= to deduct from balance
Patient& Patient::operator-=(float amount) {
    balance -= amount;
    return *this;
}

// == to compare by ID
bool Patient::operator==(const Patient& other) const {
    return id == other.id;
}

// << for formatted console output
ostream& operator<<(ostream& os, const Patient& p) {
    p.display(os);
    return os;
}
