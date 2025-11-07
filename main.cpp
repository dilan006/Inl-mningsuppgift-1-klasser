#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

using namespace std;


// Funktion för tidsstämpel

string current_timestamp() {
    time_t now = time(nullptr);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now));
    return {buf};
}


// Sensor-klass

class Sensor {
private:
    string name;
    string unit;
    double minValue;
    double maxValue;

public:
    Sensor(string n, string u, double min, double max)
        : name(std::move(n)), unit(std::move(u)), minValue(min), maxValue(max) {}

    double read() const {
        double r = static_cast<double>(rand()) / RAND_MAX;
        return minValue + r * (maxValue - minValue);
    }

    string getName() const { return name; }
    string getUnit() const { return unit; }
};

// =============================================================
// Struktur för mätning
// =============================================================
struct Measurement {
    string name;
    string unit;
    double value;
    string timestamp;
};

// =============================================================
// Storage-klass för alla mätvärden
// =============================================================
class MeasurementStorage {
private:
    vector<Measurement> data;

public:
    void addMeasurement(const Measurement& m) {
        data.push_back(m);
    }

    void printAll() const {
        if (data.empty()) {
            cout << "Inga mätvärden finns ännu.\n";
            return;
        }

        for (const auto& m : data) {
            cout << m.timestamp << " | "
                 << m.name << " | "
                 << fixed << setprecision(2) << m.value << " "
                 << m.unit << "\n";
        }
    }

    // Statistik per sensor
    void printStatistics() const {
        if (data.empty()) {
            cout << "Inga mätvärden finns att analysera.\n";
            return;
        }

        vector<string> names;

        for (const auto& m : data) {
            if (find(names.begin(), names.end(), m.name) == names.end())
                names.push_back(m.name);
        }

        for (const string& sensorName : names) {
            vector<double> values;

            for (const auto& m : data)
                if (m.name == sensorName)
                    values.push_back(m.value);

            if (values.empty()) continue;

            double sum = 0;
            double minV = values[0];
            double maxV = values[0];

            for (double v : values) {
                sum += v;
                minV = min(minV, v);
                maxV = max(maxV, v);
            }

            double avg = sum / values.size();

            double variance = 0;
            for (double v : values) variance += pow(v - avg, 2);
            variance /= values.size();
            double stdDev = sqrt(variance);

            cout << "\n--- Statistik för " << sensorName << " ---\n";
            cout << "Antal mätvärden: " << values.size() << "\n";
            cout << "Medelvärde: " << fixed << setprecision(2) << avg << "\n";
            cout << "Min: " << minV << "\n";
            cout << "Max: " << maxV << "\n";
            cout << "Standardavvikelse: " << stdDev << "\n";
        }
    }

    // Spara till fil
    void saveToFile(const string& filename) const {
        ofstream file(filename);
        for (const auto& m : data) {
            file << m.timestamp << ","
                 << m.name << ","
                 << m.value << ","
                 << m.unit << "\n";
        }
        cout << "Data sparad till fil: " << filename << "\n";
    }

    // Ladda från fil
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Kunde inte öppna fil.\n";
            return;
        }

        string timestamp, name, unit, valueStr;

        while (getline(file, timestamp, ',')) {
            getline(file, name, ',');
            getline(file, valueStr, ',');
            getline(file, unit);

            if (timestamp.empty() || name.empty() || valueStr.empty() || unit.empty())
                continue;

            Measurement m;
            m.timestamp = timestamp;
            m.name = name;
            m.value = stod(valueStr);
            m.unit = unit;

            data.push_back(m);
        }

        cout << "Data inläst från fil.\n";
    }
};

// =============================================================
// Huvudprogram (meny)
// =============================================================
int main() {
    srand(time(nullptr));

    Sensor temp("Temperatur", "°C", 15, 30);
    Sensor hum("Luftfuktighet", "%", 20, 60);

    vector<Sensor> sensors = {temp, hum};

    MeasurementStorage storage;

    int val;

    while (true) {
        cout << "\n===== MENY =====\n";
        cout << "1. Läs nya mätvärden\n";
        cout << "2. Visa statistik per sensor\n";
        cout << "3. Visa alla mätvärden\n";
        cout << "4. Spara till fil\n";
        cout << "5. Läs från fil\n";
        cout << "6. Avsluta\n";
        cout << "Val: ";
        cin >> val;

        if (!cin) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Felaktig input!\n";
            continue;
        }

        if (val == 1) {
            for (const auto& s : sensors) {
                Measurement m;
                m.name = s.getName();
                m.unit = s.getUnit();
                m.value = s.read();
                m.timestamp = current_timestamp();
                storage.addMeasurement(m);
            }
            cout << "Nya mätvärden registrerade.\n";
        }
        else if (val == 2) {
            storage.printStatistics();
        }
        else if (val == 3) {
            storage.printAll();
        }
        else if (val == 4) {
            storage.saveToFile("data.txt");
        }
        else if (val == 5) {
            storage.loadFromFile("data.txt");
        }
        else if (val == 6) {
            cout << "Avslutar...\n";
            break;
        }
        else {
            cout << "Ogiltigt val.\n";
        }
    }

    return 0;
}