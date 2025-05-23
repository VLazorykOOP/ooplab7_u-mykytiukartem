#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <windows.h>

using namespace std;

class Person {
protected:
    string name;
public:
    Person() {}
    Person(const string& name) : name(name) {}
    virtual void printInfo() const = 0;
    virtual ~Person() {}
};

class Faculty {
private:
    string name;
public:
    Faculty() {}
    Faculty(const string& name) : name(name) {}
    string getName() const { return name; }
};

class Exam {
private:
    string subject;
public:
    Exam() {}
    Exam(const string& subject) : subject(subject) {}
    string getSubject() const { return subject; }
};

class Teacher : public Person {
private:
    string subject;
public:
    Teacher() {}
    Teacher(const string& name, const string& subject) : Person(name), subject(subject) {}
    int assignGrade() {
        int grade;
        cout << "Введіть оцінку (0–100): ";
        cin >> grade;
        if (grade < 0) grade = 0;
        if (grade > 100) grade = 100;
        return grade;
    }

    void printInfo() const override {
        cout << "Викладач: " << name << ", предмет: " << subject << endl;
    }
};

class Applicant : public Person {
private:
    string faculty;
    vector<pair<string, int>> exams; // предмет, оцінка
public:
    Applicant() {}
    Applicant(const string& name, const string& faculty) : Person(name), faculty(faculty) {}

    void addResult(const string& subject, int grade) {
        exams.push_back({subject, grade});
    }

    double averageGrade() const {
        if (exams.empty()) return 0;
        int sum = 0;
        for (const auto& e : exams)
            sum += e.second;
        return static_cast<double>(sum) / exams.size();
    }

    string getName() const { return name; }
    string getFaculty() const { return faculty; }

    void printInfo() const override {
        cout << "Абітурієнт: " << name << ", факультет: " << faculty << endl;
        for (const auto& e : exams) {
            cout << "  Іспит: " << e.first << ", оцінка: " << e.second << endl;
        }
        cout << "  Середній бал: " << fixed << setprecision(2) << averageGrade() << endl;
    }

    string toTextLine() const {
        ostringstream oss;
        oss << name << ";" << faculty;
        for (const auto& e : exams) {
            oss << ";" << e.first << ":" << e.second;
        }
        return oss.str();
    }

    void fromTextLine(const string& line) {
        istringstream iss(line);
        string token;
        getline(iss, name, ';');
        getline(iss, faculty, ';');
        exams.clear();
        while (getline(iss, token, ';')) {
            size_t sep = token.find(':');
            if (sep != string::npos) {
                string subj = token.substr(0, sep);
                int grade = stoi(token.substr(sep + 1));
                exams.push_back({subj, grade});
            }
        }
    }
};

class AdmissionSystem {
private:
    vector<Applicant> applicants;
    const string applicantFile = "applicants.txt";

    void saveToFile() {
        ofstream out(applicantFile, ios::out | ios::binary);
        out << "\xEF\xBB\xBF"; // UTF-8 BOM
        for (const auto& a : applicants) {
            out << a.toTextLine() << "\n";
        }
        out.close();
    }

    void loadFromFile() {
        applicants.clear();
        ifstream in(applicantFile, ios::in | ios::binary);
        string line;
        bool firstLine = true;
        while (getline(in, line)) {
            if (firstLine) {
                if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
                    line = line.substr(3); // remove BOM
                }
                firstLine = false;
            }
            if (!line.empty()) {
                Applicant a;
                a.fromTextLine(line);
                applicants.push_back(a);
            }
        }
        in.close();
    }

public:
    AdmissionSystem() {
        loadFromFile();
    }

    void registerApplicant() {
        string name, faculty;
        cout << "Введіть ім'я абітурієнта: ";
        cin.ignore();
        getline(cin, name);
        cout << "Введіть назву факультету: ";
        getline(cin, faculty);
        applicants.emplace_back(name, faculty);
        saveToFile();
        cout << "Абітурієнта зареєстровано!\n";
    }

    void conductExam() {
        if (applicants.empty()) {
            cout << "Немає зареєстрованих абітурієнтів.\n";
            return;
        }

        string subject;
        cout << "Введіть назву предмету: ";
        cin.ignore();
        getline(cin, subject);
        Exam exam(subject);

        string teacherName;
        cout << "Введіть ім'я викладача: ";
        getline(cin, teacherName);
        Teacher teacher(teacherName, subject);

        for (auto& applicant : applicants) {
            cout << "\nОцінювання: " << applicant.getName() << endl;
            int grade = teacher.assignGrade();
            applicant.addResult(subject, grade);
        }

        saveToFile();
        cout << "Іспит завершено і оцінки збережено.\n";
    }

    void showApplicants() const {
        if (applicants.empty()) {
            cout << "Немає абітурієнтів для показу.\n";
            return;
        }
        for (const auto& a : applicants) {
            a.printInfo();
            cout << "-----------------------\n";
        }
    }

    void admitApplicants() const {
        if (applicants.empty()) {
            cout << "Немає даних.\n";
            return;
        }

        cout << "Список зарахованих абітурієнтів (середній бал >= 60):\n";
        for (const auto& a : applicants) {
            if (a.averageGrade() >= 60) {
                cout << a.getName() << " – Зараховано!" << endl;
            } else {
                cout << a.getName() << " – НЕ зараховано.\n";
            }
        }
    }

    void runMenu() {
        int choice;
        do {
            cout << "\n====== МЕНЮ ======\n";
            cout << "1. Зареєструвати абітурієнта\n";
            cout << "2. Провести іспит\n";
            cout << "3. Показати абітурієнтів\n";
            cout << "4. Зарахування\n";
            cout << "0. Вихід\n";
            cout << "Ваш вибір: ";
            cin >> choice;

            switch (choice) {
                case 1: registerApplicant(); break;
                case 2: conductExam(); break;
                case 3: showApplicants(); break;
                case 4: admitApplicants(); break;
                case 0: cout << "Завершення роботи...\n"; break;
                default: cout << "Невірний вибір!\n";
            }
        } while (choice != 0);
    }
};

int main() {
    // Увімкнути українську мову в консолі Windows (UTF-8)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    AdmissionSystem system;
    system.runMenu();
    return 0;
}
