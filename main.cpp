#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <memory>
#include "Employee.h"

using namespace std;

int main(int argc, char* argv[])
{
    string XMLFileName = "";
    ifstream f;
    if (argc == 2) {
        XMLFileName = argv[1];
        f.open(XMLFileName);
        if (f.fail()) {
            cout << "File " << XMLFileName << " could not be opened." << endl;
            return -1;
        }
    }
    else {
        cout << "Usage: ./\"Program 3\" <XMLFileName>" << endl;
        return -1;
    }

    vector<unique_ptr<Employee>> employeeVector;

    while(!f.eof()) {
        try {
            employeeVector.push_back(unique_ptr<Employee>(Employee::fromXML((f))));

            if (employeeVector.back() == nullptr) {
                employeeVector.pop_back();
            }
        }
        catch (runtime_error e) {
            cout << e.what() << endl;
            return -1;
        }
    }

    f.close();

    for (int i = 0; i < employeeVector.size(); i++) {
        employeeVector[i]->display(cout);
    }

    ofstream empWrite("employee.bin", ios::out | ios::binary); // Can't use ios::in with a nonexisting file.

    for (int i = 0; i < employeeVector.size(); i++) {
        employeeVector[i]->write(empWrite);
    }

    employeeVector.clear();
    empWrite.close();

    fstream empRW("employee.bin", ios::in | ios::out | ios::binary);

    bool foundNull = false;
    while (foundNull == false) {
        employeeVector.push_back(unique_ptr<Employee>(Employee::read((empRW))));

        if (employeeVector.back() == nullptr) {
            employeeVector.pop_back();
            foundNull = true;
        }
    }

    for (int i = 0; i < employeeVector.size(); i++) {
        employeeVector[i]->toXML(cout);
    }

    unique_ptr<Employee> emp(Employee::retrieve(empRW, 12345));
    cout << "\nFound:" << endl;
    emp->display(cout);

    emp->salary = 150000.0;
    emp->store(empRW);

    unique_ptr<Employee> empUpdated(Employee::retrieve(empRW, 12345));
    cout << empUpdated->salary << "\n" << endl;

    empRW.close();

    fstream empApp("employee.bin", ios::in | ios::out | ios::binary | ios::app);

    string name = "Guido van Rossum";
    int id = 1956;
    string address = "1234 Central Market";
    string city = "San Francisco";
    string state = "CA";
    string country = "USA";
    string phone = "555-987-6543";
    double salary = 300000.00;
    unique_ptr<Employee> myEmp(new Employee(name, id, address, city, state, country, phone, salary));

    myEmp->store(empApp);

    unique_ptr<Employee> Guido(Employee::retrieve(empApp, id));
    cout << "Found:" << endl;
    Guido->display(cout);

    return 0;
}
