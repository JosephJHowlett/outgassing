import numpy as np
import sys

pi = np.pi
e = np.e

try:
    p0 = float(sys.argv[1])
    p1 = float(sys.argv[2])
except IndexError:
    p0 = -9.894
    p1 = -0.009320


A = .3*.185 # m^2
A = 1223.6 * (10**(-4)) # m^2 using full SA
d = 5.3e-3 # meters of half-thickness 0.0053
#d = 1.0e-2 #
c1_torr = 10.0**(p0*np.log10(e))
c1 = 10.0**(p0*np.log10(e))/7.5/A # Pa*m^3/m^2/s
c2 = -1.0/p1*3600.0 # s


c0 = (pi**2.0)*c1*c2/(8*d)
D = d*c1/(2*c0)

t_change = (d**2.0)/(6*D)

print('Diffusion Constant = %.2e m^2/s' % D)
print('Exponential Decay Time Constant = %.2f hrs' % (c2/3600.0))
print('Exponential Decay Amplitude = %.2e torr*l/s' % c1_torr)
print('Exponential Decay Amplitude = %.2e Pa*m/s' % c1)
print('Transition Time = %.2f hrs' % (t_change/3600.))
print('Initial Surface Concentration = %.2f (units?)' % c0)
