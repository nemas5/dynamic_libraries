#include <vector>
#include <fstream>
#include <functional>
#include <iostream>

using namespace std;

extern "C" vector<string> register_plugin() {
    return {"visualiser", "write_csv"};
}

using Visualiser = function<void(
    const vector<vector<double>>&, vector<string>
)>;

void write_csv(const vector<vector<double>>& data,
               vector<string> settings)
{
    string filename = settings[0];
    ofstream csv_file(filename);
    if (!csv_file.is_open())
        cout << "Не удалось открыть файл " + filename << endl;
    for (const auto& row : data) {
        for (int i = 0; i < row.size(); i++) {
            csv_file << row[i];
            if (i + 1 < row.size()) csv_file << ',';
        }
        csv_file << '\n';
    }
}

extern "C" Visualiser get_plugin() {
    return write_csv;
}
