#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <memory>
#include <cmath>

using namespace std;

// Helper functions for input validation
bool isValidPhoneNumber(const string &phone) {
    return phone.length() == 10 && all_of(phone.begin(), phone.end(), ::isdigit);
}

bool isValidPassword(const string &password) {
    regex pattern("^(?=.*[A-Za-z])(?=.*\\d)(?=.*[@$!%*#?&])[A-Za-z\\d@$!%*#?&]{6,}$");
    return regex_match(password, pattern);
}

// Class to store User information
class User {
public:
    string name, email, phone, username, password;

    // Default constructor
    User() = default;

    // Parameterized constructor
    User(string n, string e, string p, string u, string pass)
        : name(n), email(e), phone(p), username(u), password(pass) {}
};

// Class to manage user login/registration using in-memory hash map
class UserManager {
private:
    unordered_map<string, User> users; // username -> User object
    string filename = "users.txt";

    void loadUsersFromFile() {
        ifstream file(filename);
        if (file.is_open()) {
            string name, email, phone, username, password;
            while (file >> name >> email >> phone >> username >> password) {
                users[username] = User(name, email, phone, username, password);
            }
            file.close();
        }
    }

    void saveUserToFile(const User& user) {
        ofstream file(filename, ios::app); // Append new users to file
        if (file.is_open()) {
            file << user.name << " " << user.email << " " << user.phone << " " 
                 << user.username << " " << user.password << endl;
            file.close();
        }
    }

public:
    UserManager() {
        loadUsersFromFile(); // Load all users at startup
    }

    bool registerUser() {
        string name, email, phone, username, password;
        cout << "Enter your name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter your email: ";
        cin >> email;

        // Validate phone number
        do {
            cout << "Enter your phone number (10 digits): ";
            cin >> phone;
            if (!isValidPhoneNumber(phone)) {
                cout << "Invalid phone number! Please enter a valid 10-digit number.\n";
            }
        } while (!isValidPhoneNumber(phone));

        // Check if username already exists
        do {
            cout << "Enter a username: ";
            cin >> username;
            if (users.find(username) != users.end()) {
                cout << "Username already exists! Choose a different one.\n";
            }
        } while (users.find(username) != users.end());

        // Password validation loop
        do {
            cout << "Enter a password (min 6 characters, with at least one number and one special character): ";
            cin >> password;
            if (!isValidPassword(password)) {
                cout << "Invalid password! Ensure it contains at least one number, one special character, and one alphabet.\n";
            }
        } while (!isValidPassword(password));

        // Create user and add to map
        User newUser(name, email, phone, username, password);
        users[username] = newUser;

        // Save user to file
        saveUserToFile(newUser);

        cout << "Registration successful!\n";
        return true;
    }

    bool loginUser() {
        string username, password;
        cout << "Enter your username: ";
        cin >> username;
        cout << "Enter your password: ";
        cin >> password;

        if (users.find(username) != users.end() && users[username].password == password) {
            cout << "Login successful!\n";
            return true;
        } else {
            cout << "Invalid username or password!\n";
            return false;
        }
    }
};

// Vehicle class
class Vehicle {
private:
    string vehicleNumber;
    float batteryPercentage;

public:
    Vehicle(string vNumber, float batteryPercent) {
        if (!isValidVehicleNumber(vNumber)) {
            throw invalid_argument("Vehicle number must be in format: 'XX XX XX XXXX' (e.g., 'MH 12 AB 1234').");
        }
        if (batteryPercent < 0.0 || batteryPercent > 100.0) {
            throw invalid_argument("Battery percentage must be between 0 and 100.");
        }
        vehicleNumber = vNumber;
        batteryPercentage = batteryPercent;
    }

    static bool isValidVehicleNumber(const string& vNumber) {
        return regex_match(vNumber, regex("[A-Za-z]{2} [0-9]{2} [A-Za-z]{2} [0-9]{4}"));
    }

    string getVehicleNumber() const {
        return vehicleNumber;
    }

    float getBatteryPercentage() const {
        return batteryPercentage;
    }

    void displayVehicle() const {
        cout << "Vehicle Number: " << vehicleNumber << "\nBattery Percentage: " << batteryPercentage << "%\n";
    }
};

// Charging station class
class ChargingStation {
public:
    string stationName, address, stationType;
    bool availability;

    ChargingStation(string name, string addr, string type, bool available)
        : stationName(name), address(addr), stationType(type), availability(available) {}

    static void displayStations(const vector<ChargingStation>& stations) {
        cout << "Available Charging Stations:\n";
        for (size_t i = 0; i < stations.size(); ++i) {
            cout << i + 1 << ". " << stations[i].stationName << " - " << stations[i].address
                 << " (" << stations[i].stationType << ") "
                 << (stations[i].availability ? "[Available]" : "[Not Available]") << endl;
        }
    }

    static int chooseStation(const vector<ChargingStation>& stations) {
        while (true) {
            displayStations(stations);
            int choice;
            cout << "Select a station (or 0 to exit): ";
            cin >> choice;

            if (choice == 0) return -1;
            if (choice > 0 && choice <= stations.size() && stations[choice - 1].availability) {
                return choice - 1;
            } else {
                cout << "Invalid choice or station not available.\n";
            }
        }
    }
};

// Booking class
class Booking {
public:
    static void bookChargingStation(const Vehicle& vehicle, const ChargingStation& station) {
        cout << "Booking successful! Vehicle " << vehicle.getVehicleNumber()
             << " will be charged at " << station.stationName << " located at " << station.address << ".\n";
    }
};

int main() {
    UserManager userManager;

    bool userLoggedIn = false;
    while (!userLoggedIn) {
        int option;
        cout << "\n--- EV Charging Management System ---\n";
        cout << "1. Register\n2. Login\n3. Exit\nChoose an option: ";
        cin >> option;

        switch (option) {
            case 1:
                userLoggedIn = userManager.registerUser();
                break;
            case 2:
                userLoggedIn = userManager.loginUser();
                break;
            case 3:
                cout << "Exiting system.\n";
                return 0;
            default:
                cout << "Invalid option. Try again.\n";
        }
    }

    // Booking process
    vector<ChargingStation> stations = {
        ChargingStation("EV Fast Charge 1", "123 Green St", "Fast", true),
        ChargingStation("EV Charge Hub 2", "456 Electric Ave", "Slow", true),
        ChargingStation("Eco Station 3", "789 Solar Rd", "Fast", false),
        ChargingStation("Clean Energy Station 4", "101 Tesla Way", "Fast", true)
    };

    while (true) {
        int actionChoice;
        cout << "\n--- EV Booking System ---\n";
        cout << "1. Book a charging station\n2. Exit\nChoose an action: ";
        cin >> actionChoice;

        if (actionChoice == 1) {
            // Get vehicle information
            string vehicleNumber;
            float batteryPercent;

            // Input Vehicle number
            while (true) {
                cout << "Enter your vehicle number (XX XX XX XXXX): ";
                cin >> ws;  // To ignore leading whitespaces
                getline(cin, vehicleNumber);
                if (Vehicle::isValidVehicleNumber(vehicleNumber)) {
                    break;
                } else {
                    cout << "Invalid vehicle number! Please follow the format 'XX XX XX XXXX'.\n";
                }
            }

            // Input Battery percentage
            while (true) {
                cout << "Enter battery percentage (0-100): ";
                cin >> batteryPercent;
                if (cin.fail() || batteryPercent < 0 || batteryPercent > 100) {
                    cout << "Invalid input! Please enter a number between 0 and 100.\n";
                    cin.clear();  // Clear error flags
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                } else {
                    break;
                }
            }

            try {
                Vehicle vehicle(vehicleNumber, batteryPercent);
                int selectedStation = ChargingStation::chooseStation(stations);
                if (selectedStation != -1) {
                    Booking::bookChargingStation(vehicle, stations[selectedStation]);
                } else {
                    cout << "Booking cancelled.\n";
                }
            } catch (invalid_argument& e) {
                cout << "Error: " << e.what() << endl;
            }
        } else if (actionChoice == 2) {
            cout << "Exiting booking system.\n";
            break;
        } else {
            cout << "Invalid action. Try again.\n";
        }
    }

    return 0;
}
