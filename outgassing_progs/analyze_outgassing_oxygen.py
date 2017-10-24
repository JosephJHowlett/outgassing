import sys, os
import numpy as np
import matplotlib.pyplot as plt
from  matplotlib.dates import DateFormatter
import matplotlib.dates as dates
from matplotlib.ticker import FormatStrFormatter
from datetime import datetime
from dateutil import tz
from xml.dom import minidom
from scipy.optimize import curve_fit

import settings

usage = 'Usage: python analyze_outgassing_oxygen.py <pressure_data_filename>'
try:
    filename = sys.argv[1]
except IndexError:
    print(usage)
# TODO: setup arg parser

def find_asymptotes(filename, t0=0, plot=False):
    with open(os.path.join(settings.pressure_dir, filename), 'r') as f:
        lines = f.readlines()
    times = []
    pressures = []
    for line in lines:
        if len(line.split()) > 2:
            continue
        times.append(int(line.split()[0]))
        pressures.append(float(line.split()[1]))
    climbing = 0
    start_indices = []
    end_indices = []
    for i in range(len(times)):
        if (
                (pressures[i]==0) or
                (i==0) or
                (i-1 in start_indices) or
                (i-1 in end_indices)
           ):
            continue
        if (
                (climbing == 0) and
                ((pressures[i] - pressures[i-1])/pressures[i] > settings.initial_jump_threshold)
           ):
            # big pressure jump, starting climb (valve closed)
            start_indices.append(i)
            climbing = 1
        if (
                (climbing == 1) and
                (times[i+1] - times[i] > settings.offtime_threshold_seconds)
           ):
            # gauge about to be turned off - measurement taken
            end_indices.append(i)
            climbing = 0
    print('Found %i measurements' % len(start_indices))
    # TODO: add asymptote fitting
    # but for now...
    if t0==0:
        t0 = times[0]
    measurement_times = [times[i] for i in end_indices]
    measurement_pressures = [pressures[i] for i in end_indices]
    #for i in range(len(end_indices)):
    #    times_in_range = times[start_indices[i]:end_indices[i]]
    #    pressures_in_range = pressures[start_indices[i]:end_indices[i]]
    #    popt, popcov = curve_fit(asymptotic(times_in_range, pressures_in_range))    
    if plot:
        fig = plt.figure(figsize=(10,6))
        ax = fig.add_subplot(111)
        datetimes = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])
        ax.plot_date(datetimes, pressures, 'k.')
        ax.plot_date(
                        [datetimes[i] for i in start_indices],
                        [pressures[i] for i in start_indices],
                        'b.',
                        label='Bypass Close Points'
                    )
        ax.plot_date(
                        [datetimes[i] for i in end_indices],
                        [pressures[i] for i in end_indices],
                        'r.',
                        label='Scan Begin Points'
                    )
        date_format = dates.DateFormatter('%Y/%m/%d\n%H:%M')
        ax.xaxis.set_major_formatter(date_format)
        fig.autofmt_xdate()
        plt.yscale('log')
        plt.legend(loc='best')
        plt.show()
    return measurement_times, measurement_pressures, t0


def get_rga_filenames(measurement_times):
    rga_filename_strings = []
    nb_scans_from_date = 0
    for time in measurement_times:
        rga_string = datetime.fromtimestamp(time, tz=tz.gettz('America/New_York')).strftime('%m%d%Y')
        if rga_string in rga_filename_strings:
            nb_scans_from_date += 1
            rga_filename_strings.append(rga_string + '_%i' % (nb_scans_from_date + 1))
        else:
            nb_scans_from_date = 0
            rga_filename_strings.append(rga_string)
    rga_filename_strings = [string + '.xml' for string in rga_filename_strings]
    return rga_filename_strings

def load_rga_xml(filename, plot=False):
    xmldoc = minidom.parse(os.path.join(settings.rga_dir, filename))
    top_line = xmldoc.getElementsByTagName('Data')
    tot_pressure = top_line[0].attributes['TotalPressure'].value
    samples_per_amu = int(top_line[0].attributes['SamplesPerAMU'].value)
    low_mass = int(top_line[0].attributes['LowMass'].value)
    high_mass = int(top_line[0].attributes['HighMass'].value)
    # from manual, scan is really from LowMass-0.5 to HighMass+0.5
    nb_bins = samples_per_amu*(high_mass-low_mass+1)
    step_size = 1.0/samples_per_amu
    low_lim = low_mass - 0.5 + (step_size/2.0)
    high_lim = high_mass + 0.5 - (step_size/2.0)
    masses = np.linspace(low_lim, high_lim, nb_bins)
    val_list = xmldoc.getElementsByTagName('Sample')
    partial_pressures = []
    for s in val_list:
       value = s.attributes['Value'].value
       partial_pressures.append(value)
    partial_pressures = np.asarray(partial_pressures)
    if plot:
        plt.figure(figsize=(10,6))
        plt.plot(masses, partial_pressures, 'k.')
        plt.yscale('log')
        plt.show()
    return masses, partial_pressures, tot_pressure

def analyze_rga_scan(filename, pressure, input_mass=32.0, plot=False):
    # simple scanning based on amplitude closest to input_mass
    masses, partial_pressures, total_rga_pressure = load_rga_xml(filename, plot=plot)
    peak_pressure = float(partial_pressures[np.argmin(np.absolute(masses - input_mass))])
    peak_fraction = peak_pressure * float(pressure) / float(total_rga_pressure)
    return peak_fraction


def exp_noconst(x, amp, tau):
    return float(amp)*np.exp(-x/float(tau))

def exp_const(x, amp, tau, const):
    return const + (amp*np.exp(-x/tau))


def plot_outgassing_errors(time_hrs, outgassings, outgassing_errs, area, fit='exp_const'):
    # tries to fit the data to input function, then plots
    # TODO: switch to -log likelihood minimizer
    outgassings = outgassings/area
    fig = plt.figure(figsize=(10,6))
    ax = fig.add_subplot(111)
    plt.errorbar(time_hrs, outgassings, yerr=outgassing_errs, fmt='k.')
    t_cont = np.linspace(12.0, time_hrs[-1]+12.0, 1000)
    print('\n============ Best Fit Parameters ============')
    print('Parameter:\tValue:')
    try:
        if fit=='exp':
            guess=[10.0*outgassings[0], 40.0]
            best_fit_pars, popcov = curve_fit(
                                                exp_noconst,
                                                time_hrs,
                                                outgassings,
                                                p0=guess
                                             )
            print('Amplitude\t%.2e +/- %.2e torr*l/s' % (best_fit_pars[0], np.sqrt(popcov[0][0])))
            print('Lifetime\t%.2f +/- %.2f hrs' % (best_fit_pars[1], np.sqrt(popcov[1][1])))
            plt.plot(t_cont, exp_noconst(t_cont, *best_fit_pars), 'k--')
        elif fit=='exp_const':
            guess=[1.0e-4, 40.0, .1*outgassings[-1]]
            best_fit_pars, popcov = curve_fit(
                                                exp_const,
                                                time_hrs,
                                                outgassings,
                                                p0=guess
                                             )
            print('Amplitude\t%.2e +/- %.2e torr*l/s' % (best_fit_pars[0], np.sqrt(popcov[0][0])))
            print('Lifetime\t%.2f +/- %.2f hrs' % (best_fit_pars[1], np.sqrt(popcov[1][1])))
            print('Constant\t%.2e +/- %.2e torr*l/s' % (best_fit_pars[2], np.sqrt(popcov[2][2])))
            plt.plot(t_cont, exp_const(t_cont, *best_fit_pars), 'k--')
    except RuntimeError:
        print('fitting failed')
    plt.tick_params(axis='y', which='minor')
    ax = plt.gca()
    ax.set_yscale('log')
    #ax.yaxis.set_minor_formatter(FormatStrFormatter("%.0e"))
    #plt.yscale('log')
    plt.xlabel('Time Under Vacuum [Hrs]')
    plt.ylabel('Sample Outgassing Rate [Torr*l/s]')
    plt.show()


def calculate_outgassing(filename, t0=0, input_mass = 32.0):
    measurement_times, measurement_pressures, t0 = find_asymptotes(filename, t0, plot=False)
    rga_xml_filenames = get_rga_filenames(measurement_times)
    conductance = settings.coeff*settings.trans_prob*1.0/(settings.area*np.sqrt(input_mass))
    outgassings = np.zeros(len(measurement_pressures), dtype=np.float32)
    outgassing_errs = np.zeros(len(measurement_pressures), dtype=np.float32)
    time_hrs = np.zeros(len(measurement_pressures), dtype=np.float32)
    print('\n======== Calculating Outgassing Rate ========')
    print("Time (hrs):\tOutgassing (torr*l/s):")
    for (i, pressure) in enumerate(measurement_pressures):
        peak_fraction = analyze_rga_scan(
                                            rga_xml_filenames[i],
                                            pressure,
                                            input_mass=input_mass,
                                            plot=False
                                        )
        outgassings[i] = pressure * peak_fraction * conductance
        pressure_err = settings.pressure_stat_err*pressure
        outgassing_errs[i] = (pressure_err/pressure)*outgassings[i]
        time_hrs[i] = (measurement_times[i] - t0)/3600.0
        print('%.2f\t\t%.2e' % (time_hrs[i], outgassings[i]))
    plot_outgassing_errors(time_hrs, outgassings, outgassing_errs, settings.area)
    return time_hrs, outgassings, outgassing_errs


def main():
    time_hrs, outgassings, outgassing_errs = calculate_outgassing(filename)

if __name__=='__main__':
    main()
