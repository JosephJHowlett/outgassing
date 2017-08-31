import matplotlib.pyplot as plt
import sys
from matplotlib import dates

import numpy as np

import datetime
filename = sys.argv[1]
with open(filename, 'r') as f:
	lines = f.readlines()
lines = lines[5:]
t1 = np.zeros(len(lines))
t2 = np.zeros(len(lines))
t3 = np.zeros(len(lines))
time = np.zeros(len(lines))
for i, line in enumerate(lines):
	time[i] = int(line.split()[0])
	t1[i] = float(line.split()[1])
	t2[i] = float(line.split()[2])
	t3[i] = float(line.split()[3])



fig = plt.figure()
ax = fig.add_subplot(111)
secs = dates.epoch2num(time)
plt.plot_date(secs, t1, 'r.', label='Ambient')
plt.plot_date(secs, t2, 'b.', label='Plate')
plt.plot_date(secs, t3, 'g.', label='Sample')

date_fmt = '%m/%d\n%H:%M'
# Use a DateFormatter to set the data to the correct format.
date_formatter = dates.DateFormatter(date_fmt)
ax.xaxis.set_major_formatter(date_formatter)

# Sets the tick labels diagonal so they fit easier.
#fig.autofmt_xdate()

plt.xlabel('Time')
plt.ylabel('Temperature (deg C)')
plt.legend(loc='best')

plt.show()
