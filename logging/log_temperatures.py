import serial
from time import gmtime, strftime, time
import sys, os

file_header = '/home/outgassing/outgassing/temperature_data/'

ser = serial.Serial('/dev/ttyACM0', 9600)
nowtime = strftime("%Y-%m-%d %H:%M:%S")
nowtime2 = strftime("%Y%m%d_%H%M%S")
#filename = os.path.join(os.getcwd(), 'temperature_logs/', nowtime2+'-temperature.log')
try:
    filename = os.path.join(file_header, sys.argv[1])
except IndexError:
    print('Usage: python log_temperatures.py <file name (not incl. path)>')
    sys.exit()
file1 = open(filename, 'a')
file1.write('Temperature read started at '+nowtime + ' (EDT)\n' + 'Time (s)\tAmbient (C)\tPlate (C)\tSample (C)\n')
file1.close()
while True:
    temps = ser.readline()
    try:
        _ = int(temps.strip()[0])
    except:
        continue
    temps_array = temps.split()
    temps = temps_array[0] + '\t\t' + temps_array[1] + '\t\t' + temps_array[2] + '\n'
    file1 = open(filename,'a')
    file1.write(str(int(round(time()))) + '\t' + temps)
    file1.close()
