#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

// User profile data
struct UserProfile {
    string university;
    string student_id;
    string major;
    string routine;
    vector<string> study_habits;
    vector<string> interests;
    vector<string> lifestyle;
    bool premium = false;
};

// Helper function to get user input
string getInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

// Helper to convert string to lowercase
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// Parse comma-separated numbers into selected options
vector<string> parseChoices(const string& input, const vector<string>& options) {
    vector<string> result;
    stringstream ss(input);
    string token;
    
    while (getline(ss, token, ',')) {
        try {
            int choice = stoi(token);
            if (choice >= 1 && choice <= (int)options.size()) {
                result.push_back(options[choice - 1]);
            }
        } catch (...) {}
    }
    return result;
}

// Step 1: Select University
void selectUniversity(UserProfile& profile, const vector<string>& universities) {
    cout << "\n===== Select University =====\n";
    cout << "Search or press Enter to list all:\n";
    
    while (true) {
        string query = getInput("Search: ");
        string lowerQuery = toLower(query);
        
        vector<string> matches;
        for (const auto& uni : universities) {
            if (query.empty() || toLower(uni).find(lowerQuery) != string::npos) {
                matches.push_back(uni);
            }
        }
        
        if (matches.empty()) {
            cout << "No matches found. Try again.\n";
            continue;
        }
        
        for (size_t i = 0; i < matches.size(); i++) {
            cout << i + 1 << ") " << matches[i] << "\n";
        }
        
        string choice = getInput("Enter number (or 'r' to search again): ");
        if (choice == "r") continue;
        
        try {
            int idx = stoi(choice);
            if (idx >= 1 && idx <= (int)matches.size()) {
                profile.university = matches[idx - 1];
                cout << "Selected: " << profile.university << "\n";
                return;
            }
        } catch (...) {}
        
        cout << "Invalid choice.\n";
    }
}

// Step 2: Enter Student ID
void enterStudentID(UserProfile& profile) {
    cout << "\n===== Enter Student ID =====\n";
    while (true) {
        string id = getInput("Student ID (at least 3 characters): ");
        if (id.length() >= 3) {
            profile.student_id = id;
            return;
        }
        cout << "ID too short. Try again.\n";
    }
}

// Step 3: Complete Profile
void completeProfile(UserProfile& profile) {
    cout << "\n===== Profile Details =====\n";
    
    profile.major = getInput("Major/Department: ");
    
    cout << "\nDaily Routine:\n1) Early Bird\n2) Night Owl\n";
    while (true) {
        string choice = getInput("Choose 1 or 2: ");
        if (choice == "1") { profile.routine = "Early Bird"; break; }
        if (choice == "2") { profile.routine = "Night Owl"; break; }
        cout << "Invalid choice.\n";
    }
    
    vector<string> habitOptions = {"Group Study", "Solo Study", "Pomodoro", 
                                    "Last-minute cramming", "Regular review"};
    cout << "\nStudy Habits (enter numbers separated by commas):\n";
    for (size_t i = 0; i < habitOptions.size(); i++) {
        cout << i + 1 << ") " << habitOptions[i] << "\n";
    }
    profile.study_habits = parseChoices(getInput("Your choices: "), habitOptions);
    
    vector<string> interestOptions = {"Sports", "Coding", "Volunteering", 
                                       "Music", "Debate", "Entrepreneurship"};
    cout << "\nInterests (enter numbers separated by commas):\n";
    for (size_t i = 0; i < interestOptions.size(); i++) {
        cout << i + 1 << ") " << interestOptions[i] << "\n";
    }
    profile.interests = parseChoices(getInput("Your choices: "), interestOptions);
    
    vector<string> lifestyleOptions = {"Non-smoker", "Vegan/Vegetarian", 
                                        "Fitness Enthusiast", "Gamer", "Night-social"};
    cout << "\nLifestyle (enter numbers separated by commas):\n";
    for (size_t i = 0; i < lifestyleOptions.size(); i++) {
        cout << i + 1 << ") " << lifestyleOptions[i] << "\n";
    }
    profile.lifestyle = parseChoices(getInput("Your choices: "), lifestyleOptions);
}

// Step 4: Premium Subscription
void offerPremium(UserProfile& profile) {
    cout << "\n===== Premium Subscription =====\n";
    cout << "Upgrade to Premium (optional):\n";
    cout << " - Enhanced profile visibility\n";
    cout << " - Unlimited peer connections\n";
    cout << " - Premium badge\n\n";
    cout << "1) Yes - Enable Premium\n2) Not now\n";
    
    while (true) {
        string choice = getInput("Choose 1 or 2: ");
        if (choice == "1") {
            profile.premium = true;
            cout << "Premium enabled!\n";
            return;
        }
        if (choice == "2") {
            profile.premium = false;
            cout << "Continuing with free account.\n";
            return;
        }
        cout << "Invalid choice.\n";
    }
}

// Display summary
void showSummary(const UserProfile& profile) {
    cout << "\n===== Summary =====\n";
    cout << "University: " << profile.university << "\n";
    cout << "Student ID: " << profile.student_id << "\n";
    cout << "Major: " << profile.major << "\n";
    cout << "Routine: " << profile.routine << "\n";
    
    cout << "Study Habits: ";
    for (const auto& h : profile.study_habits) cout << h << ", ";
    cout << "\n";
    
    cout << "Interests: ";
    for (const auto& i : profile.interests) cout << i << ", ";
    cout << "\n";
    
    cout << "Lifestyle: ";
    for (const auto& l : profile.lifestyle) cout << l << ", ";
    cout << "\n";
    
    cout << "Premium: " << (profile.premium ? "Yes" : "No") << "\n";
}

int main() {
    vector<string> universities = {
        "IBA Karachi", "LUMS Lahore", "NED University", "UET Lahore",
        "University of Karachi", "Sukkur IBA", 
        "Quaid-e-Azam University", "FAST-NU Lahore"
    };
    
    UserProfile profile;
    
    cout << "Welcome to CampusConnect Onboarding\n";
    
    selectUniversity(profile, universities);
    enterStudentID(profile);
    completeProfile(profile);
    offerPremium(profile);
    
    showSummary(profile);
    
    cout << "\nThank you for joining CampusConnect!\n";
    return 0;
}