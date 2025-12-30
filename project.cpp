#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <memory>
#include <ranges>

using namespace std;

// ===================== Utility =====================
string getInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

bool safeToInt(const string& s, int& out) {
    try {
        out = stoi(s);
        return true;
    } catch (...) {
        return false;
    }
}

// ===================== Data Model =====================
struct UserProfile {
    string university, student_id, major, routine;
    vector<string> interests;
    bool premium = false;
};

// ===================== Template Validator =====================
template<typename T>
class Validator {
    bool (*check)(const T&);
    string msg;
public:
    Validator(bool (*c)(const T&), string m) : check(c), msg(m) {}
    bool isValid(const T& val) const { return check(val); }
    string message() const { return msg; }
};

// ===================== Base Step =====================
class FormStep {
public:
    virtual ~FormStep() = default;
    virtual bool execute(UserProfile&) = 0;
    virtual string getTitle() const = 0;
};

// ===================== University Step =====================
class UniversityStep : public FormStep {
    vector<string> universities;

public:
    UniversityStep(vector<string> u) : universities(move(u)) {}

    string getTitle() const override { return "Select University"; }

    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";
        string query = getInput("Search university (Enter for all): ");

        auto filtered = universities | views::filter([&](const string& u) {
            if (query.empty()) return true;

            string a = u, b = query;
            ranges::transform(a, a.begin(), ::tolower);
            ranges::transform(b, b.begin(), ::tolower);
            return a.find(b) != string::npos;
        });

        vector<string> matches(filtered.begin(), filtered.end());

        if (matches.empty()) {
            cout << "No universities found.\n";
            return false;
        }

        for (size_t i = 0; i < matches.size(); ++i)
            cout << i + 1 << ") " << matches[i] << "\n";

        int choice;
        while (true) {
            if (safeToInt(getInput("Choose: "), choice) &&
                choice >= 1 && choice <= (int)matches.size()) {
                p.university = matches[choice - 1];
                return true;
            }
            cout << "Invalid choice. Try again.\n";
        }
    }
};

// ===================== Student ID Step =====================
class StudentIDStep : public FormStep {
    static bool validID(const string& s) { return s.length() >= 3; }
    Validator<string> validator;

public:
    StudentIDStep() : validator(validID, "ID must be at least 3 characters.") {}

    string getTitle() const override { return "Enter Student ID"; }

    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";
        while (true) {
            string id = getInput("Student ID: ");
            if (validator.isValid(id)) {
                p.student_id = id;
                return true;
            }
            cout << validator.message() << "\n";
        }
    }
};

// ===================== Profile Step =====================
class ProfileStep : public FormStep {
    vector<string> parseChoices(const string& input,
                                const vector<string>& opts) {
        vector<string> result;
        stringstream ss(input);
        string token;

        while (getline(ss, token, ',')) {
            int idx;
            if (safeToInt(token, idx) &&
                idx >= 1 && idx <= (int)opts.size())
                result.push_back(opts[idx - 1]);
        }
        return result;
    }

public:
    string getTitle() const override { return "Profile Details"; }

    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";

        p.major = getInput("Major: ");

        cout << "Routine:\n1) Early Bird\n2) Night Owl\n";
        p.routine = (getInput("Choose: ") == "1") ? "Early Bird" : "Night Owl";

        vector<string> interests = {"Sports", "Coding", "Music", "Debate"};
        cout << "Interests:\n";
        for (size_t i = 0; i < interests.size(); ++i)
            cout << i + 1 << ") " << interests[i] << "\n";

        p.interests = parseChoices(
            getInput("Choose (comma-separated): "), interests);

        return true;
    }
};

// ===================== Premium Step =====================
class PremiumStep : public FormStep {
    void (*callback)(const UserProfile&);

public:
    PremiumStep(void (*cb)(const UserProfile&)) : callback(cb) {}

    string getTitle() const override { return "Premium Subscription"; }

    bool execute(UserProfile& p) override {
        cout << "\n===== " << getTitle() << " =====\n";
        cout << "1) Enable Premium\n2) Free Account\n";

        p.premium = (getInput("Choose: ") == "1");
        if (callback) callback(p);

        return true;
    }
};

// ===================== Flow Manager =====================
class OnboardingFlow {
    vector<unique_ptr<FormStep>> steps;
    UserProfile profile;

    void summary() {
        cout << "\n===== Summary =====\n";
        cout << "University: " << profile.university << "\n";
        cout << "Student ID: " << profile.student_id << "\n";
        cout << "Major: " << profile.major << "\n";
        cout << "Routine: " << profile.routine << "\n";
        cout << "Interests: ";
        for (auto& i : profile.interests) cout << i << " ";
        cout << "\nPremium: " << (profile.premium ? "Yes" : "No") << "\n";
    }

public:
    OnboardingFlow(vector<string> unis) {
        steps.push_back(make_unique<UniversityStep>(move(unis)));
        steps.push_back(make_unique<StudentIDStep>());
        steps.push_back(make_unique<ProfileStep>());
        steps.push_back(make_unique<PremiumStep>(
            [](const UserProfile& p) {
                cout << "[Callback] Profile created for "
                     << p.student_id << "\n";
            }));
    }

    void run() {
        cout << "Welcome to CampusConnect\n";
        for (auto& step : steps)
            if (!step->execute(profile)) return;
        summary();
    }
};

// ===================== main =====================
int main() {
    vector<string> universities = {
        "IBA Karachi", "LUMS Lahore",
        "NED University", "UET Lahore",
        "University of Karachi"
    };

    OnboardingFlow flow(universities);
    flow.run();

    cout << "\nThank you!\n";
    return 0;
}
