#! /usr/bin/python
import sys, os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from  matplotlib.dates import DateFormatter
import matplotlib.dates as dates
from matplotlib.ticker import FormatStrFormatter
from datetime import datetime
from dateutil import tz
from xml.dom import minidom
from scipy.optimize import curve_fit
import pickle
import argparse
import re
import math
import settings

def is_num(s):
    try:
	float(s)
	return True
    except ValueError:
	return False

def find_nearest(array,value):
    idx = np.searchsorted(array, value, side="left")
    if idx > 0 and (idx == len(array) or math.fabs(value - array[idx-1]) < math.fabs(value - array[idx])):
        return idx-1
    else:
        return idx

def temp_finder(measurement_times, filename):
    with open(os.path.join(settings.temperature_dir, filename), 'r') as f:
        lines = f.readlines()
    times = []
    sample = []
    plate = []
    ambient = []

    for line in lines:
        line = line.rstrip()
        count = 0
        for bit in re.split(r'\t+', line):
	    if is_num(bit) == True:
                count = count + 1
        if count ==4:
            array = re.split(r'\t+', line)
    	    times.append(int(array[0]))
            ambient.append(float(array[1]))
            plate.append(float(array[2]))
    	    sample.append(float(array[3]))

    sample_temps = []
    plate_temps = []
    sample_errs =[]
    plate_errs =[]

    for value in measurement_times:
        index = find_nearest(times, value)
        sample_temps.append(sample[index])
        plate_temps.append(plate[index])
	plate_range = []
	sample_range = []
	for i in range(100):
	    plate_range.append(plate[index-i])
	    sample_range.append(sample[index -i])
	plate_errs.append(np.std(plate_range))
	sample_errs.append(np.std(sample_range))

    return sample_temps, plate_temps, sample_errs, plate_errs

