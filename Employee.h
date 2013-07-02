#ifndef EMPLOYEE_H_INCLUDED
#define EMPLOYEE_H_INCLUDED
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <cstring>
#include <vector>

using namespace std;

class Employee {
    struct emprec { // Written/read when doing I/O with binary files.
        char name[31];
        int id;
        char address[26];
        char city[21];
        char state[21];
        char country[21];
        char phone[21];
        double salary;
    };

public:
    // Data members are public to easily change salary from main().
    string name;
    int id;
    string address;
    string city;
    string state;
    string country;
    string phone;
    double salary;

    Employee();
    Employee(string, int, string, string, string, string, string, double);
    void display(std::ostream&) const; // Write readable Employee representation to a stream
    void write(std::ostream&) const; // Write fixed-length record to current file position
    void store(std::iostream&) const; // Overwrite (or append) record in (to) file
    void toXML(std::ostream&) const; // Write XML record for Employee

    static Employee* read(std::istream& s) { // Read record from current file position
        emprec rec;
        s.read(reinterpret_cast<char*>(&rec), sizeof rec);
        unique_ptr<Employee> emp(new Employee);
        if (s) {
            emp->name = rec.name;
            emp->id = rec.id;
            emp->address = rec.address;
            emp->city = rec.city;
            emp->state = rec.state;
            emp->country = rec.country;
            emp->phone = rec.phone;
            emp->salary = rec.salary;
            return emp.release();
        }
        return nullptr;
    }

    static Employee* retrieve(std::istream& f,int desiredID) { // Search file for record by id
        f.clear();
        f.seekg(0, ios::beg);

        while(!f.eof()) {
            unique_ptr<Employee> emp(Employee::read(f));
            if (emp->id == desiredID) {
                return emp.release();
            }
        }
        return nullptr;
    }

    static Employee* fromXML(std::istream& f) { // Read the XML record from a stream
            if (!f.eof()) {
                unique_ptr<Employee> emp(new Employee);

                bool empParsed = false; // Set to true once the closing Employee tag is reached.
                string currentTag;

                string firstTag = Employee::getNextTag(f);
                if (firstTag == "") { // No tag was returned. EOF was reached.
                    return nullptr;
                }
                else if (firstTag != "<Employee>") {
                    string err = "Missing <Employee> tag."; // Found " + firstTag + " instead.";
                    throw runtime_error(err);
                }

                while (empParsed == false) {
                    currentTag = Employee::getNextTag(f);

                    if (Employee::equalsIgnoreCase(currentTag, "</Employee>")) { // Check for a premature occurrence of the </Employee> tag.
                        empParsed = Employee::isEmpComplete(*emp);
                        if (empParsed == false) {
                            // emp->printFields(); // For debug.
                            if (emp->name == "") {
                                throw runtime_error("Missing <Name> tag");
                            }
                            else if (emp->id == -1) {
                                throw runtime_error("Missing <Id> tag");
                            }
                            // throw runtime_error("</Employee> tag found before all fields were entered.");
                        }
                        else // The Employee has at least a name and an id. It is complete.
                            empParsed = true;
                    }
                    else if (equalsIgnoreCase(currentTag, "<Id>")) {
                        if (emp->id != -1)
                            throw runtime_error("Multiple <Id> tags");
                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        int idVal;
                        stringstream(getNextValue(f)) >> idVal;
                        emp->id = idVal;

                        // cout << "Debug: id tag found. Value found is: " << idVal << " ." << endl;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</Id>");
                    }
                    else if (equalsIgnoreCase(currentTag, "<Salary>")) {
                        if (emp->salary != 0.0)
                            throw runtime_error("Multiple </Salary> tags");

                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        double salaryVal;
                        stringstream(getNextValue(f)) >> salaryVal;
                        emp->salary = salaryVal;

                        // cout << "Debug: salary tag found. Value found is: " << salaryVal << " ." << endl;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</Salary>");
                    }
                    else if (equalsIgnoreCase(currentTag, "<Name>")) {
                        if (emp->name != "")
                            throw runtime_error("Multiple </Name> tags");

                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        string nameVal = getNextValue(f);
                        emp->name = nameVal;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</Name>");
                    }
                    else if (equalsIgnoreCase(currentTag, "<Address>")) {
                        if (emp->address != "")
                            throw runtime_error("Multiple </Address> tags");

                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        string addressVal = getNextValue(f);
                        emp->address = addressVal;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</Address>");
                    }
                    else if (equalsIgnoreCase(currentTag, "<City>")) {
                        if (emp->city != "")
                            throw runtime_error("Multiple <City> tags");

                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        string cityVal = getNextValue(f);
                        emp->city = cityVal;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</City>");
                    }
                    else if (equalsIgnoreCase(currentTag, "<State>")) {
                        if (emp->state != "")
                            throw runtime_error("Multiple <State> tags");

                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        string stateVal = getNextValue(f);
                        emp->state = stateVal;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</State>");
                    }
                    else if (equalsIgnoreCase(currentTag, "<Country>")) {
                        if (emp->country != "")
                            throw runtime_error("Multiple <Country> tags");

                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        string countryVal = getNextValue(f);
                        emp->country = countryVal;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</Country>");
                    }
                    else if (equalsIgnoreCase(currentTag, "<Phone>")) {
                        if (emp->phone != "")
                            throw runtime_error("Multiple <Phone> tags");

                        currentTag = currentTag.substr(1, currentTag.size()-2); // Remove the '<' and '/>' from the tag.
                        string phoneVal = getNextValue(f);
                        emp->phone = phoneVal;

                        string closingTag = getNextTag(f);
                        closingTagCompare(closingTag, "</Phone>");
                    }
                    else { // The tag was unknown.
                        string err = "Invalid tag: " + currentTag;
                        throw runtime_error(err);
                    }
                    // empParsed = isEmpComplete(*emp);
                }

                //string closingEmployeeTag = getNextTag(f);
                //closingTagCompare(closingEmployeeTag, "</Employee>");

                return emp.release();
            }
        return nullptr;
    }

    static string getNextTag(std::istream& f) {
        bool tagParsed = false;
        bool ignore = true;
        string tag = "";

        while (tagParsed == false && !f.eof()) {
            char c;

            //f.get(c, numeric_limits<streamsize>::max(), '>');
            c = f.get();

            if (c == '<') {
                ignore = false;
            }
            else if (c == '>') {
                tagParsed = true;
            }

            if (ignore == false) {
                tag += c;
            }
        }
        return tag;
    }

    static string getNextValue(std::istream& f) { // Gets the values between tags.
        string val = "";
        char c;

        c = f.get();
        while (c != '<') {
            val += c;
            c = f.get();
        }

        f.unget(); // Unget the '<' that is part of the next tag.
        return val;
    }

    static bool isEmpComplete(Employee emp) { // True if all fields have been modified. False otherwise.
        if (emp.id != -1 && emp.name != "")
            return true;
        else
            return false;
    }

    static void closingTagCompare(string closingTag, string desiredTag) {
        int val = stricmp(closingTag.c_str(), desiredTag.c_str());

        if (val == 0) {
            return;
        }
        else {
            string err = "Missing " + desiredTag + " tag."; // Found \"" + closingTag + "\" instead.\n";
            throw runtime_error(err);
        }
    }

    static bool equalsIgnoreCase(string tag, string desiredTag) {
        int val = stricmp(tag.c_str(), desiredTag.c_str());

        if (val == 0) {
            return true;
        }
        else
            return false;
    }

    static int stricmp(const char* p, const char* q) {
        while (*p && *q) {
            if (tolower(*p) != tolower(*q))
                break;
            ++p;
            ++q;
        }
        return *p - *q;
    }
};

#endif // EMPLOYEE_H_INCLUDED
