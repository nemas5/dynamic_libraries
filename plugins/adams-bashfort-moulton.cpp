#include <iostream>
#include <vector>
#include <functional>

using namespace std;

using Solver = function<vector<vector<double>>(
    function<vector<double>(double, const vector<double>&, const vector<double>&)> f,
    const vector<vector<double>>& initials, const vector<double>& params, double t_start, double t_end, int steps
)>;

extern "C" vector<string> register_plugin() {
    return {"solver", "abm"};
}

vector<vector<double>> abm(
    function<vector<double>(double, const vector<double>&, const vector<double>&)> f,
    const vector<vector<double>>& initials, const vector<double>& params, double t_start, double t_end, int steps)
{
    int n = initials[0].size();
    vector<vector<double>> y(steps, vector<double>(n));
    for (int i = 0; i < 4; i++)
        y[i] = initials[i];
    double h = (t_end - t_start) / (steps - 1);
    vector<double> w_pred(n);
    vector<vector<double>> f_res(5);
    for (int step = 3; step < steps - 1; step++) {
        for (int i = 0; i < 4; i++)
            f_res[i] = f((step - i) * h, y[step - i], params);
        for (int i = 0; i < n; i++) {
            w_pred[i] = y[step][i] + (h / 24.) * (
                55. * f_res[0][i] - 59. * f_res[1][i] + 37. * f_res[2][i] - 9. * f_res[3][i]
            );
        }
        f_res[4] = f((step + 1) * h, w_pred, params);
        for (int i = 0; i < n; i++) {
            y[step + 1][i] = y[step][i] + (h / 24.) * (
                9. * f_res[4][i] + 19. * f_res[0][i] - 5. * f_res[1][i] + f_res[2][i]
            );
        }
    }
    return y;
}

extern "C" Solver get_plugin() {
    return abm;
}