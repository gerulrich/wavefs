#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>

namespace wavefs {
    using namespace std;
    class config {
        public:
            map<string, string>& operator[](const string& section);
            explicit config(const string &file_name) {
                data = init(file_name);
            };
    private:
        static map<string, map<string, string>> init(const string& file_name);
        map<string, map<string, string>> data;
    };
}

#endif // CONFIG_H
