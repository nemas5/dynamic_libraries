#include <iostream>
#include <vector>
#include <functional>
#include <dlfcn.h>
#include <fstream>
#include <filesystem> 
#include <unordered_map>

using namespace std;

vector<double> model(double t, const vector<double>& y, const vector<double>& k) {
    vector<double> sodu = {
        -k[0] * y[0],
        k[0] * y[0] - (k[2] + k[1]) * y[1] + k[3] * y[2],
        k[2] * y[1] - k[3] * y[2],
        k[4] * y[1] - k[5] * y[3] + k[6],
        k[7] * y[1] - k[8] * y[4] + k[9],
        k[10] * y[1] - k[11] * y[5] + k[12]
    };
    return sodu;
}

class Plugins {
    public:
        Plugins()
        {
            for (const auto& entry : filesystem::directory_iterator("./plugins")) {
                if (entry.path().extension() == ".so") {
                    string path = entry.path().string();
                    cout << "Загрузка плагина: " << path << endl;
                    void* plugin_file = dlopen(path.c_str(), RTLD_NOW);
                    vector<string>(*plugin)() = (vector<string>(*)())dlsym(plugin_file, "register_plugin");
                    vector<string> info = plugin();
                    cout << "Загружена функция " << info[1] << " типа " << info[0] << endl;
                    if (info[0] == "solver") {
                        Solver(*get_plugin)() = (Solver(*)())dlsym(plugin_file, "get_plugin");
                        add_solver(info[1], get_plugin());
                    }
                    else {
                        Visualiser(*get_plugin)() = (Visualiser(*)())dlsym(plugin_file, "get_plugin");
                        add_visualiser(info[1], get_plugin());
                    }
                }
            }
        }

        using Solver = function<vector<vector<double>>(
            function<vector<double>(double, const vector<double>&, const vector<double>&)> f,
            const vector<vector<double>>& initials, const vector<double>& params, double t_start, double t_end, int steps
        )>;
        using Visualiser = function<void(
            const vector<vector<double>>&, vector<string>
        )>;

        void add_solver(string name, Solver f) {
            solvers[name] = f;
        }

        void add_visualiser(string name, Visualiser f) {
            visualisers[name] = f;
        }

        Solver get_solver(string name) {
            auto solver = solvers.find(name);
            if (solver == solvers.end()) {
                cout << name << endl;
                throw runtime_error("Запрашиваемой функции нет!");
            }
            return solvers[name];
        }

        Visualiser get_visualiser(string name) {
            auto vis = visualisers.find(name);
            if (vis == visualisers.end()) {
                cout << name << endl;
                throw runtime_error("Запрашиваемой функции нет!");
            }
            return visualisers[name];
        }

    private:
        
        unordered_map<string, Visualiser> visualisers;
        unordered_map<string, Solver> solvers;
};

int main() {
    Plugins plugins;

    vector<double> m_minus = {
        1.029, 0.670, 0.118, 1.981, 0.201, 0.295, 8.998, 
        0.138, 0.347, 0.913, 0.687, 0.580, 2.343
    };
    vector<double> m_plus = {
        2.481, 0.649, 0.949, 2.757, 2.900, 0.466, 1.376,
        0.531, 0.869, 2.814, 1.490, 1.666, 7.060
    };

    auto write_csv = plugins.get_visualiser("write_csv");
    auto milne_simpson = plugins.get_solver("milne_simpson");
    auto abm = plugins.get_solver("abm");
    auto runge_kutta = plugins.get_solver("runge_kutta");
    

    vector<vector<double>> rkm, rkp;
    rkm = runge_kutta(model, {{1., 0., 0., 0., 0., 0.}}, m_minus, 0., 10., 1e+3);
    rkp = runge_kutta(model, {{1., 0., 0., 0., 0., 0.}}, m_plus, 0., 10., 1e+3);
    write_csv(rkm, {"runge-kutta-m-minus.csv"});
    write_csv(rkp, {"runge-kutta-m-plus.csv"});

    vector<vector<double>> mm_initial(rkm.begin(), rkm.begin() + 4);
    vector<vector<double>> mp_initial(rkp.begin(), rkp.begin() + 4);
    write_csv(milne_simpson(model, mm_initial, m_minus, 0., 10., 1e+3), {"milne-simpson-m-minus.csv"});
    write_csv(milne_simpson(model, mp_initial, m_plus, 0., 10., 1e+3), {"milne-simpson-m-plus.csv"});

    write_csv(abm(model, mm_initial, m_minus, 0., 10., 1e+3), {"adams-bashfort-moulton-m-minus.csv"});
    write_csv(abm(model, mp_initial, m_plus, 0., 10., 1e+3), {"adams-bashfort-moulton-m-plus.csv"});

    return 0;
}
