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

import settings

def plot_only_temperatures(filename):
    fig = plt.figure(figsize=(10,6))
    ax = fig.add_subplot(111)
    with open(os.path.join(settings.temperature_dir, filename), 'r') as f:
        lines = f.readlines()
    times = []
    t_ambient = []
    t_plate = []
    t_sample = []
    for line in lines:
        if len(line.split()) > 4:
            continue
        times.append(int(line.split()[0]))
	a = float(line.split()[1])
	if isinstance(a, float) == True:
	    t_ambient.append(float(line.split()[1]))
        t_plate.append(float(line.split()[2]))
        t_sample.append(float(line.split()[3]))
    datetimes = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])
    if len(t_ambient) > 0:
        ax.plot_date(datetimes, t_ambient, 'g.', label='T_ambient')
    ax.plot_date(datetimes, t_plate, 'b.', label='T_plate')
    ax.plot_date(datetimes, t_sample, 'r.', label='T_sample')
    date_format = dates.DateFormatter('%Y/%m/%d\n%H:%M')

    ax.xaxis.set_major_formatter(date_format)
    fig.autofmt_xdate()
    plt.tick_params(axis='y', which='minor')
    plt.ylabel('Temperatures [$\,^{\circ}$C]')
    plt.xlabel('Time')
    plt.legend(loc='best')
    plt.show()
    return


def plot_pressure(filename):
    with open(os.path.join(settings.pressure_dir, filename), 'r') as f:
        lines = f.readlines()
    times = []
    pressures = []
    for line in lines:
        if len(line.split()) > 2:
            continue
        times.append(int(line.split()[0]))
        pressures.append(float(line.split()[1]))
    fig = plt.figure(figsize=(10,6))
    ax = fig.add_subplot(111)
    datetimes = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])
    ax.plot_date(datetimes, pressures, 'k.')
    date_format = dates.DateFormatter('%Y/%m/%d\n%H:%M')
    ax.xaxis.set_major_formatter(date_format)
    fig.autofmt_xdate()
    plt.yscale('log')
    plt.tick_params(axis='y', which='minor')
    plt.ylabel('Pressure [Torr]')
    plt.xlabel('Time')
    plt.show()
    return


def plot_both(file_p, file_t):
    
    #Get pressure data
    with open(os.path.join(settings.pressure_dir, file_p), 'r') as f:
        lines = f.readlines()
    times = []
    pressures = []
    for line in lines:
        if len(line.split()) > 2:
            continue
        times.append(int(line.split()[0]))
        pressures.append(float(line.split()[1]))
    datetimes_p = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])

    #Get temperature data
    with open(os.path.join(settings.temperature_dir, file_t), 'r') as f:
        lines = f.readlines()
    times = []
    t_ambient = []
    t_plate = []
    t_sample = []
    for line in lines:
        if len(line.split()) > 4:
            continue
        times.append(int(line.split()[0]))
	a = float(line.split()[1])
	if isinstance(a, float) == True:
	    t_ambient.append(float(line.split()[1]))
        t_plate.append(float(line.split()[2]))
        t_sample.append(float(line.split()[3]))
    datetimes_t = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])

    fig = plt.figure(figsize=(10,6))
    ax1 = fig.add_subplot(111)
    ax1.plot_date(datetimes_p, pressures, 'k.', label= 'Pressure')
    date_format = dates.DateFormatter('%Y/%m/%d\n%H:%M')
    ax1.xaxis.set_major_formatter(date_format)
    fig.autofmt_xdate()
    ax1.set_yscale('log')
    ax1.tick_params(axis='y', which='minor')
    ax1.set_ylabel('Pressure [Torr]')
    ax1.set_xlabel('Time')
    ax1.legend(loc='best')

    ax2 = ax1.twinx()
    ax2.set_ylabel('Temperature [$\,^{\circ}$C]')
    if len(t_ambient) > 0:
        ax2.plot_date(datetimes_t, t_ambient, 'g.', label='T_ambient')
    ax2.plot_date(datetimes_t, t_plate, 'b.', label='T_plate')
    ax2.plot_date(datetimes_t, t_sample, 'r.', label='T_sample')
    ax2.tick_params(axis='y', which='minor')
    ax2.legend(loc='best')
    plt.show()
    return
