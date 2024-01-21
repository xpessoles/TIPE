from scipy.integrate import solve_ivp
import numpy as np

import matplotlib.pyplot as plt

# Définition des paramètres du système
m = 57e-3 # kg
LAMBDA = -4e-3*0
g = 9.81  # m/s²
alpha = 1e-4 # m
omega_y = 600 # rad/s


# Définition du système d'équation différentielle
def systeme(t, X):
    x_1 = X[0]
    x_2 = X[1]
    y_1 = X[2]
    y_2 = X[3]
    z_1 = X[4]
    z_2 = X[5]

    dx1_dt = x_2
    dy1_dt = y_2
    dz1_dt = z_2

    dx2_dt = (1/m)*(LAMBDA*dx1_dt*np.sqrt(dx1_dt*dx1_dt+dy1_dt*dy1_dt+dz1_dt*dz1_dt)+alpha*omega_y*dz1_dt)
    dy2_dt = (1/m)*(LAMBDA*dy1_dt*np.sqrt(dx1_dt*dx1_dt+dy1_dt*dy1_dt+dz1_dt*dz1_dt))
    dz2_dt = (1/m)*(LAMBDA*dz1_dt*np.sqrt(dx1_dt*dx1_dt+dy1_dt*dy1_dt+dz1_dt*dz1_dt)-alpha*omega_y*dx1_dt-m*g)

    return [dx1_dt, dx2_dt,dy1_dt, dy2_dt,dz1_dt, dz2_dt]

t0 = 0 # seconde
tf = 1 # seconde

# Solution exact
t = np.linspace(t0,tf,1000)


# Résolution
position = solve_ivp(systeme, [t0, tf], [0,1,0,0,0,1], method='RK45',max_step=0.01)

plt.plot(position.t, position.y[0], label="x(t)")
plt.plot(position.t, position.y[2], label="y(t)")
plt.plot(position.t, position.y[4], label="z(t)")

plt.ylabel('x(t) (m)')
plt.xlabel("Temps (s)")
plt.title("Position de la masse en fonction du temps")
plt.grid(which="both")
plt.legend()
plt.show()