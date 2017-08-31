import time ##Linfengcollins 7/19/2017
import matplotlib.pyplot as plt
logfile=raw_input("Enter Filename: ")
epoch = []
ambient = []
plate = []
sample = []
def read(x):
    with open(x) as f:
        for line in f:
            epoch.append(line.split()[0])
            ambient.append(line.split()[1])
            plate.append(line.split()[2])
            sample.append(line.split()[3])
            
read(logfile)
plt.plot(epoch, sample, 'b.')
plt.plot(epoch, plate, 'r.')
plt.plot(epoch, ambient, 'y.')
plt.show()
