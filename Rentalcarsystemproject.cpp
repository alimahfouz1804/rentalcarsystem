



#include <iostream>
#include <string>
#include<iomanip>
#include <sstream>
#include <limits>
#include <cmath>
#include <cstdlib>
using namespace std;


const int MAX_CARS = 50;
const int MAX_CUSTOMERS = 50;
const int MAX_TRANSACTIONS = 50;
const int MAX_CAR_YEAR = 2025;


int carID[MAX_CARS];
string carMake[MAX_CARS];
string carModel[MAX_CARS];
int carYear[MAX_CARS];
double rentalRate[MAX_CARS];
bool isAvailable[MAX_CARS];
int carCount = 0;


int customerID[MAX_CUSTOMERS];
string customerName[MAX_CUSTOMERS];
string customerPhone[MAX_CUSTOMERS];
string driverLicense[MAX_CUSTOMERS];
int customerCount = 0;



int transactionID[MAX_TRANSACTIONS];
int rentedCarID[MAX_TRANSACTIONS];
int rentingCustomerID[MAX_TRANSACTIONS];
int rentalDays[MAX_TRANSACTIONS];
double totalPayment[MAX_TRANSACTIONS];
double bookedRentalRate[MAX_TRANSACTIONS];
int transactionCount = 0;
// This counter is never reduced when a reservation is cancelled.
long long nextTransactionID = 1;

// Read a whole line so invalid input cannot leave the menu stream stuck.
bool readText(const string& prompt, string& value) {
    string line;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) return false;
        size_t first = line.find_first_not_of(" \t\r\n\f\v");
        if (first != string::npos) {
            size_t last = line.find_last_not_of(" \t\r\n\f\v");
            value = line.substr(first, last - first + 1);
            return true;
        }
        cout << "Input cannot be empty.\n";
    }
}

bool readInteger(const string& prompt, int& value, int minimum = 1,
    int maximum = numeric_limits<int>::max()) {
    string line;
    while (readText(prompt, line)) {
        istringstream input(line);
        int number;
        char extra;
        if ((input >> number) && !(input >> extra) &&
            number >= minimum && number <= maximum) {
            value = number;
            return true;
        }
        cout << "Invalid input. Enter a whole number from " << minimum
            << " to " << maximum << ".\n";
    }
    return false;
}

bool readRate(const string& prompt, double& value) {
    string line;
    while (readText(prompt, line)) {
        istringstream input(line);
        double number;
        char extra;
        if ((input >> number) && !(input >> extra) &&
            isfinite(number) && number > 0.0) {
            value = number;
            return true;
        }
        cout << "Invalid rate. Enter a positive, finite number.\n";
    }
    return false;
}

bool readPhone(const string& prompt, string& value) {
    string phone;
    while (readText(prompt, phone)) {
        size_t firstDigit = (phone[0] == '+') ? 1 : 0;
        size_t digits = phone.size() - firstDigit;
        if (digits >= 7 && digits <= 15 &&
            phone.find_first_not_of("0123456789", firstDigit) == string::npos) {
            value = phone;
            return true;
        }
        cout << "Invalid number. Enter 7 to 15 digits, optionally starting with +.\n";
    }
    return false;
}

bool calculatePayment(double rate, int days, double& payment) {
    if (!isfinite(rate) || rate <= 0.0 || days <= 0 ||
        rate > numeric_limits<double>::max() / days) {
        cout << "Invalid rental days or rate, or total cost is too large.\n";
        return false;
    }
    payment = rate * days;
    if (!isfinite(payment) || payment <= 0.0) {
        cout << "Invalid total cost.\n";
        return false;
    }
    return true;
}

// A rented car must have exactly one reservation owned by this customer.
int findReservation(int id, int customerIDInput) {
    int found = -1;
    for (int t = 0; t < transactionCount; t++) {
        if (rentedCarID[t] == id) {
            if (found != -1) {
                cout << "Inconsistent reservation records. No changes made.\n";
                return -1;
            }
            found = t;
        }
    }
    if (found == -1) {
        cout << "No matching transaction found. No changes made.\n";
        return -1;
    }
    if (rentingCustomerID[found] != customerIDInput) {
        cout << "This reservation does not belong to that customer.\n";
        return -1;
    }
    return found;
}





void addCar() {
    if (carCount >= MAX_CARS) {
        cout << "Car inventory is full.\n";
        return;
    }

    int newID;
    if (!readInteger("Enter Car ID: ", newID)) return;


    for (int i = 0; i < carCount; i++) {
        if (carID[i] == newID) {
            cout << "Error: Car ID already exists. Please use a unique ID.\n";
            return;
        }
    }

    string make, model;
    int year;
    double rate;
    if (!readText("Enter car country make: ", make) ||
        !readText("Enter car model: ", model) ||
        !readInteger("Enter Year: ", year, 1886, MAX_CAR_YEAR) ||
        !readRate("Enter Rental Rate: ", rate)) return;

    carID[carCount] = newID;
    carMake[carCount] = make;
    carModel[carCount] = model;
    carYear[carCount] = year;
    rentalRate[carCount] = rate;
    isAvailable[carCount] = true;

    carCount++;
    cout << "Car added successfully.\n";
    cout << endl;
}



void updateCar() {
    int id;
    if (!readInteger("Enter Car ID to update: ", id)) return;
    for (int i = 0; i < carCount; i++) {
        if (carID[i] == id) {
            string make, model;
            int year;
            double rate;
            if (!readText("Enter new Make: ", make) ||
                !readText("Enter new model: ", model) ||
                !readInteger("Enter new Year: ", year, 1886, MAX_CAR_YEAR) ||
                !readRate("Enter new Rental Rate: ", rate)) return;

            carMake[i] = make;
            carModel[i] = model;
            carYear[i] = year;
            rentalRate[i] = rate;
            cout << "Car updated successfully.\n";
            cout << endl;
            return;
        }
    }
    cout << "Car not found.\n";
}



void removeCar() {
    int id;
    if (!readInteger("Enter Car ID to remove: ", id)) return;
    for (int i = 0; i < carCount; i++) {
        if (carID[i] == id) {
            if (!isAvailable[i]) {
                cout << "Cannot remove a car that is currently rented.\n";
                return;
            }
            for (int t = 0; t < transactionCount; t++) {
                if (rentedCarID[t] == id) {
                    cout << "Cannot remove a car with an active reservation.\n";
                    return;
                }
            }
            for (int j = i; j < carCount - 1; j++) {
                carID[j] = carID[j + 1];
                carMake[j] = carMake[j + 1];
                carModel[j] = carModel[j + 1];
                carYear[j] = carYear[j + 1];
                rentalRate[j] = rentalRate[j + 1];
                isAvailable[j] = isAvailable[j + 1];
            }
            carCount--;
            cout << "Car removed Succesfully.\n";
            cout << endl;

            return;
        }

    }
    cout << "Car not found.\n";
}



void displayCar() {
    cout << left << setw(10) << "ID" << setw(15) << "Model" << setw(10) << "Year" << setw(12) << "Cost/Day" << "\n";

    for (int i = 0; i < carCount; i++) {
        if (isAvailable[i]) {
            cout << left << setw(10) << carID[i] << setw(15) << carModel[i] << setw(10) << carYear[i] << "$" << setw(11) << fixed << setprecision(2) << rentalRate[i] << "\n";
        }
    }
}






void cancelReservation() {
    int id;
    if (!readInteger("Enter the ID of the car you want to cancel the reservation for: ", id)) return;

    for (int i = 0; i < carCount; i++) {
        if (carID[i] == id) {
            if (isAvailable[i]) {
                cout << "This car is already available and not reserved.\n";
                return;
            }

            int customerIDInput;
            if (!readInteger("Enter your Customer ID: ", customerIDInput)) return;
            int t = findReservation(id, customerIDInput);
            if (t == -1) return;

            double refund = totalPayment[t];
            for (int k = t; k < transactionCount - 1; k++) {
                transactionID[k] = transactionID[k + 1];
                rentedCarID[k] = rentedCarID[k + 1];
                rentingCustomerID[k] = rentingCustomerID[k + 1];
                rentalDays[k] = rentalDays[k + 1];
                totalPayment[k] = totalPayment[k + 1];
                bookedRentalRate[k] = bookedRentalRate[k + 1];
            }
            transactionCount--;
            isAvailable[i] = true;

            cout << "Reservation for car ID " << id << " has been cancelled.\n";
            cout << "Refund Amount: $" << fixed << setprecision(2) << refund << "\n";
            return;
        }
    }
    cout << "Car not found.\n";
}


void modifyReservation() {
    int id;
    if (!readInteger("Enter the Car ID of your current reservation: ", id)) return;

    for (int i = 0; i < carCount; i++) {
        if (carID[i] == id) {
            if (isAvailable[i]) {
                cout << "This car is not currently reserved.\n";
                return;
            }

            int customerIDInput;
            if (!readInteger("Enter your Customer ID: ", customerIDInput)) return;
            int t = findReservation(id, customerIDInput);
            if (t == -1) return;

            // Keep the original reservation until every change is validated.
            int newCarIndex = i;
            int newDays = rentalDays[t];
            double rate = bookedRentalRate[t];
            int changeCar;
            if (!readInteger("Do you want to change your reserved car? (1 for Yes, 0 for No): ",
                changeCar, 0, 1)) return;

            if (changeCar == 1) {
                cout << "\nAvailable Cars:\n";
                displayCar();
                int newCarID;
                if (!readInteger("\nEnter the ID of the car you want to reserve instead: ", newCarID)) return;

                bool carFound = false;
                for (int j = 0; j < carCount; j++) {
                    if (carID[j] == newCarID && isAvailable[j]) {
                        newCarIndex = j;
                        rate = rentalRate[j];
                        carFound = true;
                        break;
                    }
                }
                if (!carFound) {
                    cout << "Selected car is not available or invalid.\n";
                    return;
                }
                for (int k = 0; k < transactionCount; k++) {
                    if (rentedCarID[k] == newCarID) {
                        cout << "Selected car already has an active reservation.\n";
                        return;
                    }
                }
            }

            int modifyDays;
            if (!readInteger("Do you want to change the number of rental days? (1 for Yes, 0 for No): ",
                modifyDays, 0, 1)) return;
            if (modifyDays == 1 &&
                !readInteger("Enter new number of rental days: ", newDays)) return;

            double total;
            if (!calculatePayment(rate, newDays, total)) return;
            double difference = total - totalPayment[t];

            isAvailable[i] = true;
            isAvailable[newCarIndex] = false;
            rentedCarID[t] = carID[newCarIndex];
            rentalDays[t] = newDays;
            bookedRentalRate[t] = rate;
            totalPayment[t] = total;

            if (changeCar == 1) {
                cout << "Car changed successfully to ID " << carID[newCarIndex] << ".\n";
            }
            cout << "Updated total cost: $" << fixed << setprecision(2) << total << "\n";
            if (difference > 0.0) {
                cout << "Additional payment processed: $" << difference << "\n";
            }
            else if (difference < 0.0) {
                cout << "Refund Amount: $" << -difference << "\n";
            }
            cout << "Reservation modification complete.\n";
            return;
        }
    }
    cout << "Car ID not found.\n";
}


void addCustomer() {
    if (customerCount >= MAX_CUSTOMERS) {
        cout << "Customer list is full.\n";
        return;
    }

    int newID;
    bool isUnique;
    do {
        newID = rand() % MAX_CUSTOMERS + 1;
        isUnique = true;
        for (int i = 0; i < customerCount; i++) {
            if (customerID[i] == newID) {
                isUnique = false;
                break;
            }
        }
    } while (!isUnique);

    cout << "Generated Customer ID: " << newID << endl;
    string name, phone, license;
    if (!readText("Enter Name: ", name) ||
        !readPhone("Enter Phone Number: ", phone) ||
        !readText("Enter Driver's License Number: ", license)) return;

    customerID[customerCount] = newID;
    customerName[customerCount] = name;
    customerPhone[customerCount] = phone;
    driverLicense[customerCount] = license;
    customerCount++;
    cout << "Customer added successfully.\n";
}


void updateCustomer() {
    int id;
    if (!readInteger("Enter Customer ID to update: ", id)) return;
    for (int i = 0; i < customerCount; i++) {
        if (customerID[i] == id) {
            string name, phone, license;
            if (!readText("Enter new Name: ", name) ||
                !readPhone("Enter new Phone Number: ", phone) ||
                !readText("Enter new Driver's License Number: ", license)) return;

            customerName[i] = name;
            customerPhone[i] = phone;
            driverLicense[i] = license;
            cout << "Customer updated successfully.\n";
            return;
        }
    }
    cout << "Customer not found.\n";
}


void processPayment(int carIndex, int customerIDInput, int days) {
    if (transactionCount >= MAX_TRANSACTIONS) {
        cout << "Transaction log is full.\n";
        return;
    }
    if (nextTransactionID <= 0 || nextTransactionID > numeric_limits<int>::max()) {
        cout << "No more transaction IDs are available.\n";
        return;
    }
    if (carIndex < 0 || carIndex >= carCount || !isAvailable[carIndex]) {
        cout << "Car not available or not found.\n";
        return;
    }

    bool customerFound = false;
    for (int i = 0; i < customerCount; i++) {
        if (customerID[i] == customerIDInput) {
            customerFound = true;
            break;
        }
    }
    if (!customerFound) {
        cout << "Customer ID not found. Please register first.\n";
        return;
    }
    for (int t = 0; t < transactionCount; t++) {
        if (rentedCarID[t] == carID[carIndex]) {
            cout << "This car already has an active reservation.\n";
            return;
        }
    }

    double payment;
    if (!calculatePayment(rentalRate[carIndex], days, payment)) return;
    transactionID[transactionCount] = static_cast<int>(nextTransactionID++);
    rentedCarID[transactionCount] = carID[carIndex];
    rentingCustomerID[transactionCount] = customerIDInput;
    rentalDays[transactionCount] = days;
    totalPayment[transactionCount] = payment;
    bookedRentalRate[transactionCount] = rentalRate[carIndex];
    isAvailable[carIndex] = false;
    transactionCount++;

    cout << "Payment of $" << fixed << setprecision(2) << payment << " processed successfully.\n";
    cout << "Transaction ID: " << transactionID[transactionCount - 1] << "\n";
}


void reserveCar() {
    if (carCount == 0) {
        cout << "There are no cars to reserve.\n";
        return;
    }
    if (transactionCount >= MAX_TRANSACTIONS) {
        cout << "Transaction log is full.\n";
        return;
    }

    int customerIDInput;
    if (!readInteger("Enter your Customer ID: ", customerIDInput)) return;
    bool customerFound = false;
    for (int i = 0; i < customerCount; i++) {
        if (customerID[i] == customerIDInput) {
            customerFound = true;
            break;
        }
    }
    if (!customerFound) {
        cout << "Customer ID not found. Please register first.\n";
        return;
    }

    int id;
    if (!readInteger("Enter the ID of the car you would like to reserve: ", id)) return;
    for (int i = 0; i < carCount; i++) {
        if (carID[i] == id && isAvailable[i]) {
            cout << "Rental rate per day of this car is: $" << fixed << setprecision(2) << rentalRate[i] << endl;
            int days;
            if (!readInteger("Enter how many days you want to rent the car for: ", days)) return;
            double cost;
            if (!calculatePayment(rentalRate[i], days, cost)) return;
            cout << "Total cost: $" << cost << "\n";
            int confirm;
            if (!readInteger("Enter 1 to confirm reservation or 0 to cancel: ", confirm, 0, 1)) return;
            if (confirm == 1) {
                processPayment(i, customerIDInput, days);
            }
            else {
                cout << "Reservation cancelled.\n";
            }
            return;
        }
    }
    cout << "Car not available or not found.\n";
}


void searchCarByModel() {
    string model;
    if (!readText("Enter car model to search: ", model)) return;
    bool found = false;
    for (int i = 0; i < carCount; i++) {
        if (carModel[i] == model && carID[i] != 0) {
            cout << "ID: " << carID[i]
                << ", Make: " << carMake[i]
                << ", Year: " << carYear[i]
                << ", Rate: $" << rentalRate[i]
                << ", Available: " << (isAvailable[i] ? "Yes" : "No") << "\n";
            found = true;
        }
    }
    if (!found) cout << "No cars with that model found.\n";
}



void displayTransactionReport() {
    if (transactionCount == 0) {
        cout << "No transactions yet.\n";
        return;
    }

    cout << fixed << setprecision(2);
    cout << "TransactionID\tCarID\tCustomerID\tDays\tTotal Paid\n";
    for (int i = 0; i < transactionCount; i++) {
        cout << transactionID[i] << "\t\t"
            << rentedCarID[i] << "\t"
            << rentingCustomerID[i] << "\t\t"
            << rentalDays[i] << "\t"
            << "$" << totalPayment[i] << "\n";
    }
}




void adminPanel() {
    const int correctPassword = 1234;
    int password;
    if (!readInteger("Enter access password: ", password, 0)) return;
    if (password != correctPassword) {
        cout << "Incorrect password. \n";
        return;
    }
    if (password == correctPassword) {
        int i;
        do {
            cout << "Choose from the below: \n";
            cout << "1. Add car\n2. Remove Car\n3. Update Car\n0. Exit \n";
            if (!readInteger("Choose an option: ", i, 0, 3)) return;
            cout << endl;
            switch (i) {
            case 1: addCar(); break;
            case 2: removeCar(); break;
            case 3: updateCar(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid option.\n";
            }
        } while (i != 0);

    }
}











int main() {
    cout << " Welcome to Rent-A-Car ";
    int choice;
    do {
        cout << "\n----- Main Menu -----\n";
        cout << "1. Admin Control Center\n";
        cout << "2. New User\n";
        cout << "3. Update User\n";
        cout << "4. Display Available Cars\n";
        cout << "5. Reserve Cars \n";
        cout << "6. Cancel Reservation\n";
        cout << "7. Modify Reservation\n";
        cout << "8. Search By Car Model\n";
        cout << "9. View Transaction Reports\n";
        cout << "0. Exit\n";
        if (!readInteger("Select an option: ", choice, 0, 9)) {
            cout << "\nExiting...\n";
            break;
        }
        cout << endl;

        switch (choice) {
        case 1:adminPanel(); break;
        case 2: addCustomer(); break;
        case 3: updateCustomer(); break;
        case 4: displayCar(); break;
        case 5: reserveCar(); break;
        case 6: cancelReservation();break;
        case 7: modifyReservation();break;
        case 8:searchCarByModel();break;
        case 9:displayTransactionReport(); break;
        case 0: cout << "Exiting...\n"; break;
        default: cout << "Invalid option.\n";
        }
    } while (choice != 0);

    return 0;
}
