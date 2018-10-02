import numpy as np
import scipy.stats

temp_arr = []
pressure_arr = []
for line in open('temp_pressure_long.dat'):
   list = line.split('\t')
   temp_arr.append(float(list[0]))
   pressure_arr.append(float(list[1].split('\n')[0]))

temp = np.array(temp_arr)
pressure = np.array(pressure_arr)

def my_pearson(x, y):
   x_std = np.std(x)
   y_std = np.std(y)
   x_mean = np.mean(x)
   y_mean = np.mean(y)
   n = x.size
   r = 0.
   for i in range(0,n-1):
      p1 = (x[i]-x_mean)/x_std
      p2 = (y[i]-y_mean)/y_std
      summer = p1*p2
      r += summer
   r = r/(n-1)
   return r

my_corr = my_pearson(temp, pressure)
# print my_corr

corr = scipy.stats.pearsonr(temp,pressure)
print corr[0]
print corr[1]