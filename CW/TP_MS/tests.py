import numpy as np
import scipy as sp
from scipy.special import gammaln
from scipy.optimize import root_scalar, minimize
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
# print(df_hat, loc_hat)




def chi_mle_df_loc(x):
    x = np.asarray(x, dtype=float)

    n = len(x)
    xmin = np.min(x)

    def negative_log_likelihood(theta):
        """
        theta[0] = log_df
        theta[1] = log_gap

        df  = exp(log_df)
        loc = xmin - exp(log_gap)
        """

        log_df, log_gap = theta

        df = np.exp(log_df)
        gap = np.exp(log_gap)
        loc = xmin - gap

        y = x - loc

        # Теоретически y всегда > 0 из-за параметризации,
        # но проверку оставляем для безопасности.
        if df <= 0 or np.any(y <= 0):
            return np.inf

        log_pdf = (
            (1 - df / 2) * np.log(2)
            - gammaln(df / 2)
            + (df - 1) * np.log(y)
            - 0.5 * y**2
        )

        log_likelihood = np.sum(log_pdf)

        return -log_likelihood

    # Начальные приближения
    # df = 5, gap = 1
    start_df = 5.0
    start_gap = 1.0

    theta0 = np.array([
        np.log(start_df),
        np.log(start_gap)
    ])

    result = minimize(
        negative_log_likelihood,
        theta0,
        method="Nelder-Mead",
        options={
            "maxiter": 10000,
            "xatol": 1e-10,
            "fatol": 1e-10
        }
    )

    log_df_hat, log_gap_hat = result.x

    df_hat = np.exp(log_df_hat)
    gap_hat = np.exp(log_gap_hat)
    loc_hat = xmin - gap_hat

    return {
        "df": df_hat,
        "loc": loc_hat,
        "success": result.success,
        "message": result.message,
        "negative_log_likelihood": result.fun
    }

# result = chi_mle_df_loc(V)

# print(result["df"])
# print(result["loc"])
# print(result["success"])
# print(result["negative_log_likelihood"])
V = [round(i, 4) for i in V]
a_hat, loc_hat, scale_hat = stats.gamma.fit(V, fscale=1)
df = a_hat*2

df_fit, loc_fit, scale_fit = stats.chi.fit(V, fdf=df, fscale=1)
print(f"df = {df_fit}\nloc = {loc_fit}")
# print(len(V))