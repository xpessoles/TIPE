import math as m
import numpy as np
import matplotlib.pyplot as plt
# pas = 2*pi*b

# rayon = a
pas = 30    # mm
a = 30      # mm
b = pas/2/m.pi
r = 5 # rayon tore

les_t = [i*0.01 for i in range(500)]
les_x = [a*m.cos(t) for t in les_t]
les_y = [a*m.sin(t) for t in les_t]
les_z = [b*t for t in les_t]


ax = plt.figure().add_subplot(projection='3d')

#ax.plot(les_x, les_y, les_z)
ax.legend()

les_xx = []
les_yy = []
les_zz = []

N = 10
u = [2*m.pi/N*i for i in range(N+1) ]
les_cx = [r*m.cos(x) for x in u]
les_cy = [r*m.sin(x) for x in u]
for t in les_t :
    x,y,z = a*m.cos(t),a*m.sin(t),b*t

    theta = m.atan2(y,x)
    for i in range(N):
        xx = x + les_cx[i]*m.cos(theta)
        yy = y + les_cx[i]*m.sin(theta)
        zz = z + les_cy[i]
        les_xx.append(xx)
        les_yy.append(yy)
        les_zz.append(zz)

ax.plot(les_xx, les_yy, les_zz,'.')
plt.axis("equal")

plt.show()