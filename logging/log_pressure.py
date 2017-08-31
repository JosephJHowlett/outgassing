import sys, os, time, datetime
from PfeifferVacuum import MaxiGauge
from time import gmtime, strftime

file_header = '/home/outgassing/outgassing/pressure_data/'

nowtime = strftime("%Y-%m-%d %H:%M:%S")
nowtime2 = strftime("%Y%m%d_%H%M%S")
try:
        filename = os.path.join(file_header, sys.argv[1])
except IndexError:
        print('Usage: python log_pressure.py <file name (not including path)>')
        sys.exit()
file1 = open(filename, 'a')
file1.write('Pfeiffer read started at '+nowtime + ' (EDT)\n' + 'Time (s)\tPressure (Torr)\n')
file1.close()
while True:
    mg = MaxiGauge('/dev/my_usb_serial')
    ps = mg.pressures()
    file1 = open(filename,'a')
    #file1.write(str(time.time())+'\n')
    file1.write(str(int(round(time.time())))+'\t'+str(ps[0].pressure)+'\n')
    file1.close()
    time.sleep(59)
