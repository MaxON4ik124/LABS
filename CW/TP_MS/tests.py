import numpy as np
import scipy as sp
from scipy.special import gammaln
from scipy.optimize import root_scalar
from scipy import stats
V = []
N = 300
with open("Vyborka.csv") as V:
    V = list(map(float, V.readlines()))
# print(Vyborka)

def vyborka_mid(data):
    return sum(data) / N

def vyborka_disp(data):
    D = 0
    M = vyborka_mid(data)
    for i in range(300):
        D += ((data[i] - M)**2)
    return D / N

m1 = vyborka_mid(V)
m2 = vyborka_disp(V)

# print(m1, m2)
Min = min(V)
V_biased = [i - Min for i in V]
df_fit, loc_fit, scale_fit = stats.chi.fit(V, fdf=5.71, fscale=1)
# df_fit = stats.chi.fit(V)
# print(df_fit, loc_fit)

import numpy as np

def chi_mean(df):
    return np.sqrt(2) * np.exp(
        gammaln((df + 1) / 2) - gammaln(df / 2)
    )

def chi_var(df):
    m = chi_mean(df)
    return df - m**2

def estimate_chi_moments(x):
    x = np.asarray(x)

    mean_x = np.mean(x)
    var_x = np.mean((x - mean_x)**2)

    def equation(df):
        return chi_var(df) - var_x

    result = root_scalar(
        equation,
        bracket=[1e-6, 1e6],
        method="brentq"
    )

    df_hat = result.root
    loc_hat = mean_x - chi_mean(df_hat)

    return df_hat, loc_hat

df_hat, loc_hat = estimate_chi_moments(V)
print(df_hat, loc_hat)