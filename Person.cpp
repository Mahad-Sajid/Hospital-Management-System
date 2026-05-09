// Person.cpp
// Implementation of Person base class

#include "Person.h"

// Default constructor
Person::Person() : id(0) {
    name[0] = '\0';
    password[0] = '\0';
    contact[0] = '\0';
}

// Parameterized constructor
Person::Person(int id, const char* name, const char* password, const char* contact) : id(id) {
    MyString::copy(this->name, name, 50);
    MyString::copy(this->password, password, 50);
    MyString::copy(this->contact, contact, 15);
}

// Virtual destructor
Person::~Person() {}

// Getters
int Person::getId() const { return id; }
const char* Person::getName() const { return name; }
const char* Person::getPassword() const { return password; }
const char* Person::getContact() const { return contact; }

// Setters
void Person::setId(int id) { this->id = id; }
void Person::setName(const char* name) { MyString::copy(this->name, name, 50); }
void Person::setPassword(const char* password) { MyString::copy(this->password, password, 50); }
void Person::setContact(const char* contact) { MyString::copy(this->contact, contact, 15); }

// Validate password match
bool Person::checkPassword(const char* pwd) const {
    return MyString::compare(password, pwd) == 0;
}
