#include <iostream>
#include <vector>
#include <functional>

using namespace std;

using Solver = function<vector<vector<double>>(
    function<vector<double>(double, const vector<double>&, const vector<double>&)> f,
    const vector<vector<double>>& initials, const vector<double>& params, double t_start, double t_end, int steps
)>;

extern "C" vector<string> register_plugin() {
    return {"solver", "runge_kutta"};
}

vector<vector<double>> runge_kutta(
    function<vector<double>(double, const vector<double>&, const vector<double>&)> f,
    const vector<vector<double>>& initials, const vector<double>& params, double t_start, double t_end, int steps)
{
    int n = initials[0].size();
    vector<vector<double>> y(steps, vector<double>(n));
    y[0] = initials[0];
    double h = (t_end - t_start) / (steps - 1);
    vector<double> k1, k2, k3, k4, ans;
    int i;
    for (int step = 0; step < steps - 1; step++) {
        ans = y[step];
        k1 = f(step * h, y[step], params);
        for (i = 0; i < n; i++) {
            k1[i] *= h;
            ans[i] += k1[i] / 6.;
            k1[i] *= 0.5;
            k1[i] += y[step][i];
        }
        k2 = f(step * h + (h / 2.), k1, params);
        for (i = 0; i < n; i++) {
            k2[i] *= h;
            ans[i] += k2[i] / 3.;
            k2[i] *= 0.5;
            k2[i] += y[step][i];
        }
        k3 = f(step * h + (h / 2.), k2, params);
        for (i = 0; i < n; i++) {
            k3[i] *= h;
            ans[i] += k3[i] / 3.;
            k3[i] += y[step][i];
        }
        k4 = f(step * (h + 1), k3, params);
        for (i = 0; i < n; i++) {
            k4[i] *= h;
            ans[i] += k4[i] / 6.;
        }
        y[step + 1] = ans;
    }
    return y;
}

extern "C" Solver get_plugin() {
    return runge_kutta;
}
