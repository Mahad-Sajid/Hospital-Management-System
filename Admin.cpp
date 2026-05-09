// Admin.cpp
// Implementation of Admin class

#include "Admin.h"

// Default constructor
Admin::Admin() : Person() {}

// Parameterized constructor — admin has no contact field in file
Admin::Admin(int id, const char* name, const char* password)
    : Person(id, name, password, "") {
}

// Destructor
Admin::~Admin() {}

// Display admin info
void Admin::display(std::ostream& os) const {
    os << "Admin ID: " << id << "\n"
        << "Name: " << name << "\n";
}

// Serialize to CSV
void Admin::toCSV(char* buffer) const {
    char idBuf[10];
    MyString::fromInt(id, idBuf);

    MyString::copy(buffer, idBuf);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, name);
    MyString::concat(buffer, ",");
    MyString::concat(buffer, password);
}

// Parse from CSV line
void Admin::fromCSV(const char* line) {
    char fields[3][500];
    int count = MyString::split(line, fields, 3);
    if (count >= 3) {
        id = MyString::toInt(fields[0]);
        MyString::copy(name, fields[1], 50);
        MyString::copy(password, fields[2], 50);
    }
}
