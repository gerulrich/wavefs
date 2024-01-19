#include "config.h"
#include <fstream>
#include <iostream>

namespace wavefs {
    using namespace std;

    map<string, string>& config::operator[](const string& section) {
        return data[section];
    }

    map<string, map<string, string>> config::init(const string& file_name) {

        map<string, map<string, string>> entries;

        ifstream file(file_name);
        if (!file.is_open()) {
            cerr << "Error al abrir el archivo de configuración: " << file_name << endl;
            return entries;
        }

        string line;
        string section;

        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // find section with format [section]
            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
            } else {
                size_t idx = line.find('=');
                if (idx != string::npos) {
                    string key = line.substr(0, idx);
                    string value = line.substr(idx + 1);
                    entries[section][key] = value;
                }
            }
        }
        return entries;
    }
} // namespace wavefs
