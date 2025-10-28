#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream> // For stringstream to handle file loading

using namespace std;

// --- Member Class ---
// Stores data for a single gym member.
class Member {
private:
    int id;
    string name;
    int age;
    string membership_type; // e.g., "Monthly", "Yearly"
    double fees_paid;

public:
    // Default constructor for loading from file
    Member() : id(0), age(0), fees_paid(0.0) {}

    // Parameterized constructor for new members
    Member(int new_id, string n, int a, string type, double fees)
        : id(new_id), name(n), age(a), membership_type(type), fees_paid(fees) {}

    // Getters for display and file saving
    int getId() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getMembershipType() const { return membership_type; }
    double getFeesPaid() const { return fees_paid; }

    // Display member details in a formatted table row
    void display() const {
        cout << setw(5) << id
             << setw(20) << name
             << setw(5) << age
             << setw(15) << membership_type
             << setw(15) << fixed << setprecision(2) << fees_paid << endl;
    }

    // Function to write member data to an output stream (saves as CSV-like format)
    void save(ostream& os) const {
        os << id << "," << name << "," << age << "," << membership_type << "," << fees_paid << "\n";
    }

    // Function to read member data from an input stream
    bool load(istream& is) {
        // Read the entire line from the file
        string line;
        if (!getline(is, line)) return false;

        stringstream ss(line);
        string segment;
        vector<string> parts;

        // Split the line by comma
        while(getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        // Check if all 5 parts were successfully read
        if (parts.size() < 5) return false;

        try {
            // Convert strings to appropriate types
            id = stoi(parts[0]);
            name = parts[1];
            age = stoi(parts[2]);
            membership_type = parts[3];
            fees_paid = stod(parts[4]);
            return true;
        } catch (...) {
            // Catch conversion errors (stoi/stod)
            return false;
        }
    }
};

// --- GymManager Class ---
// Manages the collection of members and system operations.
class GymManager {
private:
    vector<Member> members;
    const string DATA_FILE = "gym_members.txt";
    int next_member_id = 1001;

    // Helper to clear input buffer
    // FIXED: Added 'const' since this function does not modify class members.
    void clearInput() const {
        // Use cin.ignore to clear up to the max size or until a newline is found
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Generates the next unique ID
    int generateNextId() {
        return next_member_id++;
    }

    // Finds the maximum existing ID to ensure new IDs are unique and increment correctly
    void updateNextId() {
        int max_id = 1000; // Start checking from base ID
        for (const auto& member : members) {
            if (member.getId() > max_id) {
                max_id = member.getId();
            }
        }
        next_member_id = max_id + 1;
    }

public:
    GymManager() {
        loadMembers(); // Load data on startup
        updateNextId(); // Ensure new IDs are unique
    }

    ~GymManager() {
        saveMembers(); // Save data on shutdown
    }

    // 1. Add New Member
    void addMember() {
        string name, type;
        int age;
        double fees;

        cout << "\n--- Add New Member ---\n";
        cout << "Enter Member Name: ";
        clearInput();
        getline(cin, name);

        cout << "Enter Age: ";
        while (!(cin >> age) || age <= 0) {
            cout << "Invalid age. Please enter a positive number: ";
            cin.clear();
            clearInput();
        }

        cout << "Enter Membership Type (e.g., Monthly/Yearly): ";
        clearInput();
        getline(cin, type);

        cout << "Enter Fees Paid ($): ";
        while (!(cin >> fees) || fees < 0) {
            cout << "Invalid fee. Please enter a non-negative number: ";
            cin.clear();
            clearInput();
        }

        int new_id = generateNextId();
        // Use emplace_back to efficiently construct the Member object in place
        members.emplace_back(new_id, name, age, type, fees);

        cout << "\n✅ Member **" << name << "** added successfully with ID: **" << new_id << "**\n";
        saveMembers(); // Save immediately after adding
    }

    // 2. View All Members
    void viewMembers() const {
        if (members.empty()) {
            cout << "\n❌ No members registered yet.\n";
            return;
        }

        cout << "\n--- Member List ---\n";
        cout << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(5) << "Age"
             << setw(15) << "Type"
             << setw(15) << "Fees Paid" << endl;
        cout << "----------------------------------------------------------------\n";

        for (const auto& member : members) {
            member.display();
        }
        cout << "----------------------------------------------------------------\n";
    }

    // 3. Search Member by ID
    void searchMember() const {
        int id_to_find;
        cout << "\n--- Search Member ---\n";
        cout << "Enter Member ID to search: ";
        if (!(cin >> id_to_find)) {
            cout << "\n❌ Invalid input. ID must be a number.\n";
            cin.clear();
            clearInput();
            return;
        }

        for (const auto& member : members) {
            if (member.getId() == id_to_find) {
                cout << "\n✅ Member Found:\n";
                cout << setw(5) << "ID"
                     << setw(20) << "Name"
                     << setw(5) << "Age"
                     << setw(15) << "Type"
                     << setw(15) << "Fees Paid" << endl;
                cout << "----------------------------------------------------------------\n";
                member.display();
                return;
            }
        }
        cout << "\n❌ Member with ID **" << id_to_find << "** not found.\n";
    }
    
    // 4. Delete Member by ID
    void deleteMember() {
        int id_to_delete;
        cout << "\n--- Delete Member ---\n";
        cout << "Enter Member ID to delete: ";
        if (!(cin >> id_to_delete)) {
            cout << "\n❌ Invalid input. ID must be a number.\n";
            cin.clear();
            clearInput();
            return;
        }

        // Search for the member using an iterator
        for (auto it = members.begin(); it != members.end(); ++it) {
            if (it->getId() == id_to_delete) {
                cout << "\n[CONFIRM] Are you sure you want to delete member: " << it->getName() << " (ID: " << id_to_delete << ")? (y/n): ";
                char confirm;
                cin >> confirm;
                clearInput();

                if (confirm == 'y' || confirm == 'Y') {
                    // Erase the member and save data
                    members.erase(it);
                    saveMembers();
                    updateNextId(); // Recalculate next ID to ensure integrity
                    cout << "\n✅ Member with ID **" << id_to_delete << "** has been deleted.\n";
                } else {
                    cout << "\nDeletion cancelled.\n";
                }
                return;
            }
        }
        cout << "\n❌ Member with ID **" << id_to_delete << "** not found.\n";
    }

    // Save all members to file
    void saveMembers() const {
        ofstream outFile(DATA_FILE);
        if (outFile.is_open()) {
            for (const auto& member : members) {
                member.save(outFile);
            }
            // Optional: cout << "Data saved successfully.\n";
        } else {
            cout << "❌ Error: Unable to open file for saving data.\n";
        }
    }

    // Load all members from file
    void loadMembers() {
        ifstream inFile(DATA_FILE);
        members.clear(); // Clear existing data before loading

        if (inFile.is_open()) {
            Member m;
            while (m.load(inFile)) {
                members.push_back(m);
                m = Member(); // Reset object for the next read
            }
            // Optional: cout << "Data loaded successfully.\n";
        } else {
            // Optional: cout << "Info: Data file not found. Starting with an empty list.\n";
        }
    }
};

// --- Main Menu and Program Execution ---
void displayMenu() {
    cout << "\n======================================\n";
    cout << "      **GYM MANAGEMENT SYSTEM** MENU\n";
    cout << "======================================\n";
    cout << "1. Add New Member\n";
    cout << "2. View All Members\n";
    cout << "3. Search Member by ID\n";
    cout << "4. Delete Member\n";
    cout << "5. Save & Exit\n";
    cout << "--------------------------------------\n";
    cout << "Enter your choice: ";
}

int main() {
    GymManager manager;
    int choice;

    do {
        displayMenu();

        if (!(cin >> choice)) {
            cout << "\n❌ Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                manager.addMember();
                break;
            case 2:
                manager.viewMembers();
                break;
            case 3:
                manager.searchMember();
                break;
            case 4:
                manager.deleteMember();
                break;
            case 5:
                cout << "\nSaving data and exiting. Goodbye! 👋\n";
                break;
            default:
                cout << "\n❌ Invalid choice. Please try again.\n";
        }
    } while (choice != 5);

    return 0;
}
