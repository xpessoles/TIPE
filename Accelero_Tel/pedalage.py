import numpy as np
import matplotlib.pyplot as plt

fid = open('mesure.csv','r')
data = fid.readlines()
fid.close()

les_t = []
les_ax = []
for line in data[1:]:
    line = line.split(';')
    les_t.append(float(line[0].replace(",",".")))
    les_ax.append(float(line[1].replace(",",".")))


def moyenne_gl(s,n):
    """
    s : signal
    n : fenetre de moyenne
    """
    les_f = []
    for i in range(len(s)-n):
        les_f.append(sum(s[i:i+n])/n)
    return les_f

def fm(s,t):
    m = sum(s)/len(s)
    les_tops = [0]
    for i in range(len(s)-1):
        if s[i]<m and s[i+1]>m :
            les_tops.append(t[i])
    return les_tops

def cadence(s):
    cad = [0]
    for i in range(len(s)-1):
        cad.append(s[i+1]-s[i])
    return cad

# On moyenne le signal
n = 20
fg = moyenne_gl(les_ax,n)
fg_t = les_t[:-n]

# On récupere les tops
tops = fm(fg,fg_t)
cad = cadence(tops)

# affichage cadence
plt.plot(cad,".")
plt.grid()
plt.show()


# spectre = np.fft.fft(les_ax)
# freq  = np.fft.fftfreq(len(les_t))
# plt.plot(freq,spectre.real,freq,spectre.imag)

# plt.plot(les_t,les_ax)
# plt.grid()
#plt.show()

