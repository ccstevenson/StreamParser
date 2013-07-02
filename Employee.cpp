#include "Employee.h"
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <cstring>
#include <ostream>

Employee::Employee() {
    name = "";
    id = -1;
    address = "";
    city = "";
    state = "";
    country = "";
    phone = "";
    salary = 0.0;
}

Employee::Employee(string name, int id, string address, string city, string state, string country, string phone, double salary) :
    name(name), address(address), city(city), state(state), country(country), phone(phone), id(id), salary(salary) {}

void Employee::display(std::ostream& s) const { // Write readable Employee representation to a stream
    s << "id: " << id << endl;
    s << "name: " << name << endl;
    s << "address: " << address << endl;
    s << "city: " << city << endl;
    s << "state: " << state << endl;
    s << "country: " << country << endl;
    s << "phone: " << phone << endl;
    s << "salary: " << salary << endl;
    s << endl;
}

void Employee::write(std::ostream& s) const { // Write fixed-length record to current file position
    emprec rec;
    strncpy(rec.name, name.c_str(), 31)[31] = '\0';
    rec.id = id;
    strncpy(rec.address, address.c_str(), 26)[26] = '\0';
    strncpy(rec.city, city.c_str(), 21)[21] = '\0';
    strncpy(rec.state, state.c_str(), 21)[21] = '\0';
    strncpy(rec.country, country.c_str(), 21)[21] = '\0';
    strncpy(rec.phone, phone.c_str(), 21)[21] = '\0';
    rec.salary = salary;

    //cout << sizeof rec << endl;
    //cout << reinterpret_cast<const char*>(&rec) << endl;
    //cout << rec.id << endl;

    s.write(reinterpret_cast<const char*>(&rec), sizeof rec);
}

void Employee::store(std::iostream& f) const { // Overwrite (or append) record in (to) file
    f.clear();
    f.seekg(0, ios::beg);

    ios::pos_type pos;
    while(!f.eof()) {
        pos = f.tellg(); // Save the position of the record we're about to read.
        unique_ptr<Employee> emp(Employee::read(f));

        if (emp != nullptr && emp->id == id) { // The record already exists. Overwrite it.
            f.seekp(pos, ios::beg); // Return to pos
            write(f);
            return;
        }
    }
    f.clear(); // Revert EOF flag.
    f.seekp(pos, ios::beg); // Return to pos
    write(f); // The record wasn't found. Append it.
                // Note that this does nothing if the file isn't opened in append mode!
                // The user (main()) will have to check to see if their record was added--I'm not aware of a method for checking
                // or changing the file open modes after the file is opened. f.setf(ios::app); doesn't seem to work.
}

void Employee::toXML(std::ostream& s) const { // Write XML record for Employee
    s << "<Employee>\n\t<Name>" << name << "</Name>\n\t<ID>" << id << "</ID>";
    if (address != "")
        s << "\n\t<Address>" << address << "</Address>";
    if (city != "")
        s << "\n\t<City>" << city << "</City>";
    if (state != "")
        s << "\n\t<State>" << state << "</State>";
    if (country != "")
        s << "\n\t<Country>" << country << "</Country>";
    if (phone != "")
        s << "\n\t<Phone>" << phone << "</Phone>";
    if (salary != 0.0)
        s << "\n\t<Salary>" << salary << "</Salary>";
    s << "\n</Employee>\n";
}



