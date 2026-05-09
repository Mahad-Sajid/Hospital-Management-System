# MediCore Hospital Management System

## GitHub Repository
**Repository Link:** [Paste your GitHub repository link here]

## Description
A comprehensive Hospital Management System built in C++ using Object-Oriented Programming principles. The system manages patients, doctors, appointments, prescriptions, billing, and medical records for a fictional hospital called **MediCore**.

## Features
- **Multi-Role Login System**: Patient, Doctor, and Admin roles with 3-attempt lockout security
- **Appointment Management**: Book, cancel, view, and manage appointments
- **Billing System**: Automatic bill generation, payment, and tracking with OVERDUE detection
- **Prescription Management**: Doctors can write prescriptions for completed appointments
- **Medical Records**: Complete patient medical history tracking
- **Admin Panel**: Full administrative control including daily reports, doctor management, and patient discharge
- **Data Persistence**: All data stored in CSV text files and survives program restarts
- **Security Logging**: Failed login attempts are logged with timestamps

## Class Structure

### Inheritance Hierarchy
```
Person (Abstract Base Class)
├── Patient
├── Doctor
└── Admin

HospitalException (Base Exception)
├── FileNotFoundException
├── InsufficientFundsException
├── InvalidInputException
└── SlotUnavailableException
```

### Other Classes
- **Appointment** — Represents a scheduled appointment
- **Bill** — Represents a billing charge
- **Prescription** — Represents medical prescriptions
- **Storage\<T\>** — Generic template container (fixed-size array, no vectors)
- **FileHandler** — Sole class for all file I/O operations
- **Validator** — Sole class for all input validation logic
- **HospitalSystem** — Central data container struct

## Operator Overloading
| Operator | Class | Purpose |
|----------|-------|---------|
| `==` | Appointment | Detect scheduling conflicts |
| `<<` | Patient, Doctor, Appointment | Formatted console output |
| `+=` | Patient | Add to balance |
| `-=` | Patient | Deduct from balance |
| `==` | Patient | Compare by ID |
| `==` | Doctor | Compare by ID |
| `==` | Bill | Compare by ID |
| `[]` | Storage\<T\> | Array-style element access |

## File Formats
All files use comma-separated values (CSV):
- `patients.txt` — patient_id, name, age, gender, contact, password, balance
- `doctors.txt` — doctor_id, name, specialization, contact, password, fee
- `admin.txt` — admin_id, name, password
- `appointments.txt` — appointment_id, patient_id, doctor_id, date, time_slot, status
- `prescriptions.txt` — prescription_id, appointment_id, patient_id, doctor_id, date, medicines, notes
- `bills.txt` — bill_id, patient_id, appointment_id, amount, status, date
- `security_log.txt` — timestamp, role, entered_id, result
- `discharged.txt` — archived patient records

## Compilation
```bash
g++ -o medicore main.cpp MyString.cpp Person.cpp Patient.cpp Doctor.cpp Admin.cpp Appointment.cpp Bill.cpp Prescription.cpp FileHandler.cpp Validator.cpp HospitalException.cpp FileNotFoundException.cpp InsufficientFundsException.cpp InvalidInputException.cpp SlotUnavailableException.cpp PatientMenu.cpp DoctorMenu.cpp AdminMenu.cpp
```

## Sample Login Credentials
| Role | ID | Password |
|------|----|----------|
| Admin | 1 | admin123 |
| Patient | 1 | pass123 |
| Patient | 2 | pat456 |
| Doctor | 1 | doc456 |
| Doctor | 2 | doc789 |

## Technical Constraints Met
- ✅ No `std::vector` used anywhere
- ✅ No `std::string` used anywhere
- ✅ No built-in string functions (strcmp, strlen, strtok, etc.)
- ✅ No global variables
- ✅ No `goto` instructions
- ✅ No memory leaks (no dynamic allocation needed — uses fixed-size arrays)
- ✅ Each class in separate .h and .cpp files
- ✅ main() is a sequence of function calls only
- ✅ Custom exceptions with try/catch
- ✅ Data persisted to .txt files
- ✅ Separate input/output functions
- ✅ Manual sorting (bubble sort)
- ✅ Case-insensitive comparison using manual toLower
