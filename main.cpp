#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <iterator>
#include <numeric>
#include <algorithm> // Added for std::all_of
#include <cctype>    // Added for std::isalpha

namespace EmployeeManagement {

    class Employee {
    protected:
        std::string empName;
        std::string empDepartment;
        std::string empDesignation;
        int years;

    public:
        Employee(const std::string& name, const std::string& department, const std::string& designation, int experience)
            : empName(name), empDepartment(department), empDesignation(designation), years(experience) {}

        virtual void inputDetails() = 0;
        virtual void displayDetails(std::ostream& output) const = 0;

        virtual ~Employee() = default;

        inline void display() const {
            std::cout << empName << " - " << empDepartment << " - " << empDesignation << " - " << years << std::endl;
        }

        friend void displayEmployeeDetails(const Employee& emp);
    };

    void displayEmployeeDetails(const Employee& emp) {
        std::cout << "Details using friend function: " << emp.empName << " - " << emp.empDepartment << " - "
                  << emp.empDesignation << " - " << emp.years << std::endl;
    }

    class ReasonManager {
    private:
        static std::vector<std::string> reasons;

    public:
        static void changeReasons();
        static const std::vector<std::string>& getReasons() { return reasons; }
    };

    std::vector<std::string> ReasonManager::reasons = {"Personal Reasons", "Career Growth", "Relocation", "Other"};

    void ReasonManager::changeReasons() {
        int choice;
        std::cout << "Do you want to change/add to the reasons list?" << std::endl;
        std::cout << "1. Yes\n2. No" << std::endl;
        std::cin >> choice;

        if (choice == 1) {
            std::string adminKey;
            std::cout << "Enter administrator key: ";
            std::cin >> adminKey;

            if (adminKey == "AdminKey123") {
                auto displayReasons = []() {
                    std::cout << "Reasons:" << std::endl;
                    const std::vector<std::string>& reasons = getReasons();
                    for (size_t j = 0; j < reasons.size(); ++j) {
                        std::cout << j + 1 << ". " << reasons[j] << std::endl;
                    }
                };

                displayReasons();

                std::cout << "Enter new reasons to add (type 'done' to stop):" << std::endl;
                std::cin.ignore();

                std::string newReason;
                while (std::getline(std::cin, newReason) && newReason != "done") {
                    reasons.push_back(newReason);
                }
                std::cout << "Reasons updated." << std::endl;
            } else {
                std::cout << "Invalid administrator key. Cannot change reasons." << std::endl;
            }
        }
    }

    class PermanentEmployee : public Employee {
    private:
        std::string reason;

    public:
        PermanentEmployee(const std::string& name, const std::string& department, const std::string& designation, int experience)
            : Employee(name, department, designation, experience) {}

        void inputDetails() override {
            std::cout << "Select a reason for leaving from the available options:" << std::endl;
            const std::vector<std::string>& reasons = ReasonManager::getReasons();
            for (size_t j = 0; j < reasons.size(); ++j) {
                std::cout << j + 1 << ". " << reasons[j] << std::endl;
            }

            int reasonChoice;
            std::cin >> reasonChoice;

            if (reasonChoice >= 1 && reasonChoice <= reasons.size()) {
                reason = reasons[reasonChoice - 1];
            } else {
                std::cout << "Invalid reason choice. Using 'Other' as reason." << std::endl;
                std::cout << "Enter reason: ";
                std::cin.ignore();
                std::getline(std::cin, reason);
            }
        }

        std::string getReason() const {
            return reason;
        }

        void displayDetails(std::ostream& output) const override {
            output << empName << " " << empDepartment << " " << empDesignation << " " << years << " " << reason << std::endl;
        }
    };

    class EmployeeData {
    private:
        std::vector<Employee*> employees;
        std::string fileName;

    public:
        EmployeeData(const std::string& file) : fileName(file) {}

        void addEmployee(Employee* emp) {
            employees.push_back(emp);
        }

        void saveEmployeeDetails() {
            std::ofstream outputFile(fileName, std::ios::app);

            for (const auto& emp : employees) {
                emp->displayDetails(outputFile);
            }

            outputFile.close();
            std::cout << "Employee details saved to file." << std::endl;
        }

        void displayEmployeesByReason() const {
            std::unordered_map<std::string, std::vector<Employee*>> employeesByReason;

            for (const auto& emp : employees) {
                employeesByReason[static_cast<PermanentEmployee*>(emp)->getReason()].push_back(emp);
            }

            for (const auto& entry : employeesByReason) {
                std::cout << "Reason: " << entry.first << std::endl;
                for (const auto& emp : entry.second) {
                    emp->display();
                }
                std::cout << std::endl;
            }
        }

        void storeEmployeesByReasonToFile() const {
            std::ofstream outputFile("reasons.txt",std::ios::app);
            if (!outputFile.is_open()) {
                std::cerr << "Unable to create/open file." << std::endl;
                return;
            }

            std::stringstream ss;
            std::streambuf *psbuf, *backup;

            backup = std::cout.rdbuf();     // backup cout's streambuf

            std::cout.rdbuf(outputFile.rdbuf()); // Assign reasons.txt to cout

            displayEmployeesByReason(); // Display employees grouped by reason

            std::cout.rdbuf(backup);        // restore cout's original streambuf

            outputFile.close();
            std::cout << "Employees grouped by reason stored in 'reasons.txt' file." << std::endl;
        }

        ~EmployeeData() {
            for (auto& emp : employees) {
                delete emp;
            }
        }
    };

    void manageEmployeeData() {
        std::string fileName = "employee_data.txt";
        int numInputs;

        EmployeeData empData(fileName);
        ReasonManager::changeReasons();

        std::cout << "How many employee details do you want to input? ";
        std::cin >> numInputs;

        for (int i = 0; i < numInputs; ++i) {
            std::string empDepartment, empDesignation;
            int years;

            std::string empName;
            bool validName = false;
            bool validDepartment = false;
            bool validDesignation = false;

            while (!validName) {
                std::cout << "Enter employee name: ";
                std::cin >> empName;

                if (std::all_of(empName.begin(), empName.end(), [](char c) { return std::isalpha(c); })) {
                    validName = true;
                } else {
                    std::cout << "Invalid name: Name should only consist of alphabetic characters. Please enter again." << std::endl;
                }
            }

            while (!validDepartment) {
                std::cout << "Enter department: ";
                std::cin >> empDepartment;

                if (std::all_of(empDepartment.begin(), empDepartment.end(), [](char c) { return std::isalpha(c); })) {
                    validDepartment = true;
                } else {
                    std::cout << "Invalid department: Department should only consist of alphabetic characters. Please enter again." << std::endl;
                }
            }

            while (!validDesignation) {
                std::cout << "Enter designation: ";
                std::cin >> empDesignation;

                if (std::all_of(empDesignation.begin(), empDesignation.end(), [](char c) { return std::isalpha(c); })) {
                    validDesignation = true;
                } else {
                    std::cout << "Invalid designation: Designation should only consist of alphabetic characters. Please enter again." << std::endl;
                }
            }

            std::cout << "Years Worked: ";
            try {
                std::cin >> years;
                if (std::cin.fail()) {
                    throw std::runtime_error("Invalid input: Enter a valid number for years.");
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                std::cin.clear();
                while (std::cin.get() != '\n') ; // Clear the input buffer
                continue;
            }

            Employee* emp = new PermanentEmployee(empName, empDepartment, empDesignation, years);
            emp->inputDetails();
            empData.addEmployee(emp);
        }

        empData.saveEmployeeDetails();
        empData.storeEmployeesByReasonToFile();
    }
} // namespace EmployeeManagement

int main() {
    EmployeeManagement::manageEmployeeData();
    return 0;
}
