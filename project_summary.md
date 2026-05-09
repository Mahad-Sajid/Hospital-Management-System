# MediCore Hospital Management System — Project Summary

## ✅ Build Status: **COMPILED SUCCESSFULLY** (zero errors, zero warnings)

## Project Structure (49 files)

### Class Files (17 classes, 34 source files)

|Class|Header|Implementation|Description|
|-|-|-|-|
|`MyString`|[MyString.h](file:///S:/OP/MyString.h)|[MyString.cpp](file:///S:/OP/MyString.cpp)|Custom string utilities (replaces all prohibited functions)|
|`Person`|[Person.h](file:///S:/OP/Person.h)|[Person.cpp](file:///S:/OP/Person.cpp)|Abstract base class with 2 pure virtual methods|
|`Patient`|[Patient.h](file:///S:/OP/Patient.h)|[Patient.cpp](file:///S:/OP/Patient.cpp)|Overloads: `+=`, `-=`, `==`, `<<`|
|`Doctor`|[Doctor.h](file:///S:/OP/Doctor.h)|[Doctor.cpp](file:///S:/OP/Doctor.cpp)|Overloads: `==`, `<<`|
|`Admin`|[Admin.h](file:///S:/OP/Admin.h)|[Admin.cpp](file:///S:/OP/Admin.cpp)|Loaded from admin.txt|
|`Appointment`|[Appointment.h](file:///S:/OP/Appointment.h)|[Appointment.cpp](file:///S:/OP/Appointment.cpp)|Overloads: `==` (conflict detection), `<<`|
|`Bill`|[Bill.h](file:///S:/OP/Bill.h)|[Bill.cpp](file:///S:/OP/Bill.cpp)|Billing charges with status tracking|
|`Prescription`|[Prescription.h](file:///S:/OP/Prescription.h)|[Prescription.cpp](file:///S:/OP/Prescription.cpp)|Medicines \& notes per appointment|
|`Storage<T>`|[Storage.h](file:///S:/OP/Storage.h)|*(header-only template)*|Fixed array `T data\\\\\\\[100]`, no vectors|
|`FileHandler`|[FileHandler.h](file:///S:/OP/FileHandler.h)|[FileHandler.cpp](file:///S:/OP/FileHandler.cpp)|Sole class for all file I/O|
|`Validator`|[Validator.h](file:///S:/OP/Validator.h)|[Validator.cpp](file:///S:/OP/Validator.cpp)|Sole class for all input validation|
|`HospitalException`|[HospitalException.h](file:///S:/OP/HospitalException.h)|[HospitalException.cpp](file:///S:/OP/HospitalException.cpp)|Base exception with `char\\\\\\\[200]` message|
|`FileNotFoundException`|[FileNotFoundException.h](file:///S:/OP/FileNotFoundException.h)|[FileNotFoundException.cpp](file:///S:/OP/FileNotFoundException.cpp)|File open failure|
|`InsufficientFundsException`|[InsufficientFundsException.h](file:///S:/OP/InsufficientFundsException.h)|[InsufficientFundsException.cpp](file:///S:/OP/InsufficientFundsException.cpp)|Balance too low|
|`InvalidInputException`|[InvalidInputException.h](file:///S:/OP/InvalidInputException.h)|[InvalidInputException.cpp](file:///S:/OP/InvalidInputException.cpp)|Validation failure|
|`SlotUnavailableException`|[SlotUnavailableException.h](file:///S:/OP/SlotUnavailableException.h)|[SlotUnavailableException.cpp](file:///S:/OP/SlotUnavailableException.cpp)|Time slot conflict|

### Menu \& Entry Point Files

|File|Description|
|-|-|
|[main.cpp](file:///S:/OP/main.cpp)|Entry point — `main()` is function calls only|
|[PatientMenu.h](file:///S:/OP/PatientMenu.h) / [PatientMenu.cpp](file:///S:/OP/PatientMenu.cpp)|All 8 patient operations|
|[DoctorMenu.h](file:///S:/OP/DoctorMenu.h) / [DoctorMenu.cpp](file:///S:/OP/DoctorMenu.cpp)|All 6 doctor operations|
|[AdminMenu.h](file:///S:/OP/AdminMenu.h) / [AdminMenu.cpp](file:///S:/OP/AdminMenu.cpp)|All 10 admin operations|
|[HospitalSystem.h](file:///S:/OP/HospitalSystem.h)|Central data container struct|

### Data Files

|File|Format|
|-|-|
|[patients.txt](file:///S:/OP/patients.txt)|3 sample patients with balances|
|[doctors.txt](file:///S:/OP/doctors.txt)|3 sample doctors|
|[admin.txt](file:///S:/OP/admin.txt)|Admin credentials (ID:1, Pass:admin123)|
|[appointments.txt](file:///S:/OP/appointments.txt)|1 sample completed appointment|
|[prescriptions.txt](file:///S:/OP/prescriptions.txt)|1 sample prescription|
|[bills.txt](file:///S:/OP/bills.txt)|1 sample paid bill|
|security\_log.txt|Initially empty|
|discharged.txt|Initially empty|

## Compilation Command

```bash
g++ -std=c++14 -Wall -o medicore.exe main.cpp MyString.cpp Person.cpp Patient.cpp Doctor.cpp Admin.cpp Appointment.cpp Bill.cpp Prescription.cpp FileHandler.cpp Validator.cpp HospitalException.cpp FileNotFoundException.cpp InsufficientFundsException.cpp InvalidInputException.cpp SlotUnavailableException.cpp PatientMenu.cpp DoctorMenu.cpp AdminMenu.cpp
```

## Requirements Checklist

|Requirement|Status|
|-|-|
|Each class in own .h and .cpp|✅|
|No `std::vector`|✅|
|No `std::string`|✅|
|No `strcmp`, `strlen`, `strtok`, etc.|✅ (all custom in MyString)|
|No memory leaks|✅ (no dynamic allocation — fixed arrays only)|
|No global variables|✅ (HospitalSystem passed by reference)|
|No `goto`|✅|
|Separate input/output functions|✅|
|`main()` is function calls only|✅|
|Data persisted to `.txt` files|✅|
|Operator overloading (`==`, `<<`, `+=`, `-=`)|✅|
|Custom exceptions with try/catch|✅|
|Manual sorting (bubble sort)|✅|
|Case-insensitive compare (manual `toLower`)|✅|
|3-attempt login lockout|✅|
|Security log|✅|
|[README.md](file:///S:/OP/README.md)|✅|

## Login Credentials

|Role|ID|Password|
|-|-|-|
|Admin|1|admin123|
|Patient (Ahmed)|1|pass123|
|Patient (Fatima)|2|pat456|
|Patient (Usman)|3|pat789|
|Doctor (Sara)|1|doc456|
|Doctor (Ali)|2|doc789|
|Doctor (Ayesha)|3|doc321|

> \\\\\\\[!IMPORTANT]
> Remember to paste your GitHub repository link inside the \\\\\\\[README.md](file:///S:/OP/README.md) file before submission.

