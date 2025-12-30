#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <memory>
#include<ranges>

using namespace std;

// Data Model
struct UserProfile {
    string university, student_id, major, routine;
    vector<string> interests;
    bool premium = false;
};

// Template Validator with function pointer
template<typename T>
class Validator {
    bool (*check)(const T&);
    string msg;
public:
    Validator(bool (*c)(const T&), string m) : check(c), msg(m) {}
    bool isValid(const T& val) const { return check(val); }
    string getMessage() const { return msg; }
};

string getInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

// Base class with pure virtual functions
class FormStep {
public:
    virtual ~FormStep() = default;
    virtual bool execute(UserProfile& profile) = 0;
    virtual string getTitle() const = 0;
};

// University Step - uses Ranges
class UniversityStep : public FormStep {
    vector<string> universities;
public:
    UniversityStep(vector<string> unis) : universities(move(unis)) {}
    string getTitle() const override { return "Select University"; }
    
    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";
        string query = getInput("Search university (or Enter for all): ");
        
        // C++20 ranges with lambda
        auto filtered = universities | views::filter([&](const string& u) {
            if (query.empty()) return true;
            string lower_u = u, lower_q = query;
            ranges::transform(lower_u, lower_u.begin(), ::tolower);
            ranges::transform(lower_q, lower_q.begin(), ::tolower);
            return lower_u.find(lower_q) != string::npos;
        });
        
        vector<string> matches(filtered.begin(), filtered.end());
        for (size_t i = 0; i < matches.size(); i++)
            cout << i + 1 << ") " << matches[i] << "\n";
        
        int choice = stoi(getInput("Choose: "));
        if (choice >= 1 && choice <= (int)matches.size()) {
            p.university = matches[choice - 1];
            return true;
        }
        return false;
    }
};

// Student ID Step - uses Template
class StudentIDStep : public FormStep {
    Validator<string> validator;
    static bool validateID(const string& s) { return s.length() >= 3; }
public:
    StudentIDStep() : validator(validateID, "ID must be 3+ characters") {}
    string getTitle() const override { return "Enter Student ID"; }
    
    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";
        while (true) {
            string id = getInput("Student ID: ");
            if (validator.isValid(id)) {
                p.student_id = id;
                return true;
            }
            cout << validator.getMessage() << "\n";
        }
    }
};

// Profile Step - uses STL containers
class ProfileStep : public FormStep {
    vector<string> parseChoices(const string& input, const vector<string>& opts) {
        vector<string> result;
        stringstream ss(input);
        string token;
        while (getline(ss, token, ',')) {
            try {
                int c = stoi(token);
                if (c >= 1 && c <= (int)opts.size()) result.push_back(opts[c - 1]);
            } catch (...) {}
        }
        return result;
    }
public:
    string getTitle() const override { return "Profile Details"; }
    
    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";
        p.major = getInput("Major: ");
        
        cout << "Routine: 1) Early Bird  2) Night Owl\n";
        p.routine = (getInput("Choose: ") == "1") ? "Early Bird" : "Night Owl";
        
        vector<string> interestOpts = {"Sports", "Coding", "Music", "Debate"};
        cout << "Interests:\n";
        for (size_t i = 0; i < interestOpts.size(); i++)
            cout << i + 1 << ") " << interestOpts[i] << "\n";
        p.interests = parseChoices(getInput("Choose (comma-separated): "), interestOpts);
        return true;
    }
};

// Premium Step - uses function pointer
class PremiumStep : public FormStep {
    void (*callback)(const UserProfile&);
public:
    PremiumStep(void (*cb)(const UserProfile&)) : callback(cb) {}
    string getTitle() const override { return "Premium Subscription"; }
    
    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";
        cout << "Premium: Enhanced visibility & unlimited connections\n";
        cout << "1) Enable Premium  2) Free account\n";
        p.premium = (getInput("Choose: ") == "1");
        if (callback) callback(p);
        return true;
    }
};

// Onboarding Manager
class OnboardingFlow {
    vector<unique_ptr<FormStep>> steps;
    UserProfile profile;
    
    void showSummary() {
        cout << "\n===== Summary =====\n";
        cout << "University: " << profile.university << "\n";
        cout << "Student ID: " << profile.student_id << "\n";
        cout << "Major: " << profile.major << "\n";
        cout << "Routine: " << profile.routine << "\n";
        cout << "Interests: ";
        for (const auto& i : profile.interests) cout << i << " ";
        cout << "\nPremium: " << (profile.premium ? "Yes" : "No") << "\n";
    }
public:
    OnboardingFlow(vector<string> universities) {
        steps.push_back(make_unique<UniversityStep>(move(universities)));
        steps.push_back(make_unique<StudentIDStep>());
        steps.push_back(make_unique<ProfileStep>());
        steps.push_back(make_unique<PremiumStep>([](const UserProfile& p) {
            cout << "[Callback] Done for " << p.student_id << "\n";
        }));
    }
    
    void run() {
        cout << "Welcome to CampusConnect\n";
        for (auto& step : steps) {
            if (!step->execute(profile)) {
                cout << "Cancelled.\n";
                return;
            }
        }
        showSummary();
    }
};

int main() {
    vector<string> universities = {"IBA Karachi", "LUMS Lahore", "NED University", 
                                    "UET Lahore", "University of Karachi"};
    OnboardingFlow flow(universities);
    flow.run();
    cout << "\nThank you!\n";
    return 0;
}