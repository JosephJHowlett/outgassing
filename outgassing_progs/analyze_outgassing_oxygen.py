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

def get_parsed_args():
    parser = argparse.ArgumentParser(description="Analysis for outgassing data.")
    parser.add_argument(
                            '--calculate_oxygen_outgassing', '-o',
                            help = """Basic oxygen outgassing calculation. Input pressure file,
                                      outputs pickled outgassing data and plots with fit line."""
                       )
    parser.add_argument(
                            '--plot_comparison_from_pickle',
                            nargs = 2,
                            help = """Input two pickle filenames in pickle dir, plots both on same axis
                                      for comparison. Use labels arg to label them"""
                       )
    parser.add_argument(
                            '--plot_pressure',
                            help = """Directly plot pressure from file."""
                       )
    parser.add_argument(
                            '--plot_temperatures',
                            help = """Directly plot temperatures from file."""
                       )
    parser.add_argument(
                            '--labels',
                            nargs = 2,
                            help = """Labels for plotting multiple from pickle"""
                       )
    parser.add_argument(
                            '--pickle_output',
                            action = 'store_true',
                            help = """Set \"False\" to avoid pickling outgassing data."""
                       )
    parser.add_argument(
                            '--plot',
                            action = 'store_true',
                            help = """Set \"False\" to not plot outgassing data."""
                       )
    parser.add_argument(
                            '--plot_measurements',
                            help = """Plot measurements found by framework."""
                       )
    return parser.parse_args()


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

def plot_temperatures(ax, filename):
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
        t_ambient.append(float(line.split()[1]))
        t_plate.append(float(line.split()[2]))
        t_sample.append(float(line.split()[3]))
    datetimes = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])
    ax.plot_date(datetimes, t_ambient, 'g.', label='T_ambient')
    ax.plot_date(datetimes, t_plate, 'b.', label='T_plate')
    #ax.plot_date(datetimes, t_sample, 'r.', label='T_sample')
    return

def plot_only_temperatures(filename):
    fig = plt.figure(figsize=(10,6))
    ax = fig.add_subplot(111)
    plot_temperatures(ax, filename)
    date_format = dates.DateFormatter('%Y/%m/%d\n%H:%M')
    ax.xaxis.set_major_formatter(date_format)
    fig.autofmt_xdate()
    #plt.yscale('log')
    plt.tick_params(axis='y', which='minor')
    plt.ylabel('Temperatures [$\,^{\circ}$C]')
    plt.xlabel('Time')
    plt.legend(loc='best')
    plt.show()
    return

def asymptotic(x, amp, rate, mean):
    return amp*(1.0 - np.exp(-1*rate*(x - mean)))

def find_asymptotes(filename, t0=0, plot=False, plot_fits=False):
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
    for i in range(len(times)-1):
        if (
                (pressures[i]==0) or
                (pressures[i-1]==0) or
                (i==0) or
                (i-1 in start_indices) or
                (i-1 in end_indices)
           ):
            continue
        if (
                (climbing == 0) and
                ((pressures[i] - pressures[i-1])/pressures[i-1] > settings.initial_jump_threshold) and
                (pressures[i+1] > pressures[i]) and
                ((times[i] - times[i-1]) < 180.)
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
    if t0==0:
        t0 = times[0]
    measurement_times = [times[i] for i in end_indices]
    # initialize the pressures at the final values
    measurement_pressures = [pressures[i] for i in end_indices]
    # then loop through and fit each set to get ultimate pressure
    for i in range(len(end_indices)):
        times_in_range = times[start_indices[i]+1:end_indices[i]]
        pressures_in_range = pressures[start_indices[i]+1:end_indices[i]]
        popt, popcov = curve_fit(asymptotic, times_in_range, pressures_in_range, p0=[pressures_in_range[-1], 1.0/1200.0, times_in_range[0]])
        measurement_pressures[i] = popt[0]
        if plot_fits:
            plt.plot(times_in_range, pressures_in_range, 'k.')
            x = np.linspace(times_in_range[0], times_in_range[-1], len(times_in_range)*10)
            plt.plot(x, asymptotic(x, *popt), 'r--')
            plt.show()
    if plot:
        for measurement_index in settings.skip_measurements:
            start_indices.pop(measurement_index)
            end_indices.pop(measurement_index)
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
        plt.ylabel('Pressure [Torr]')
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

def exp_exp(x, amp1, tau1, amp2, tau2, c):
    return (amp1*np.exp(-1*x/tau1)) + (amp2*np.exp(-1*x/tau2)) + c

def plot_outgassing_errors(
                            ax, time_hrs, outgassings,
                            outgassing_errs, area,
                            fit='exp_const', color='k',
                            last_plot=True, label=''
                          ):
    # tries to fit the data to input function, then plots
    # TODO: switch to -log likelihood minimizer
    outgassings = outgassings/area
    plt.errorbar(time_hrs, outgassings, yerr=outgassing_errs, fmt='%s.' % color, label=label)
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
            plt.plot(t_cont, exp_noconst(t_cont, *best_fit_pars), '%s--' % color)
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
            plt.plot(t_cont, exp_const(t_cont, *best_fit_pars), '%s--' % color)
        elif fit=='exp_exp':
            guess=[1.0e-4, 40.0, 1.0e-4, 200.0, .1*outgassings[-1]]
            best_fit_pars, popcov = curve_fit(
                                                exp_exp,
                                                time_hrs,
                                                outgassings,
                                                p0=guess
                                             )
            print('Amplitude 1\t%.2e +/- %.2e torr*l/s' % (best_fit_pars[0], np.sqrt(popcov[0][0])))
            print('Lifetime 1\t%.2f +/- %.2f hrs' % (best_fit_pars[1], np.sqrt(popcov[1][1])))
            print('Amplitude 2\t%.2e +/- %.2e torr*l/s' % (best_fit_pars[2], np.sqrt(popcov[2][2])))
            print('Lifetime 2\t%.2f +/- %.2f hrs' % (best_fit_pars[3], np.sqrt(popcov[3][3])))
            print('Constant\t%.2e +/- %.2e torr*l/s' % (best_fit_pars[4], np.sqrt(popcov[4][4])))
            plt.plot(t_cont, exp_exp(t_cont, *best_fit_pars), '%s--' % color)
    except RuntimeError:
        print('fitting failed')
    plt.tick_params(axis='y', which='minor')
    ax = plt.gca()
    ax.set_yscale('log')
    ax.grid()
    plt.xlabel('Time Under Vacuum [Hrs]')
    plt.ylabel('Room Temp. Oxygen Outgassing [$Torr*l/s/cm^2$]')

def save_to_pickle(pickle_name, time_hrs, outgassings, outgassing_errs):
    output_dict = {}
    output_dict['time_hrs'] = time_hrs
    output_dict['outgassings'] = outgassings
    output_dict['outgassing_errs'] = outgassing_errs
    pickle.dump(output_dict, open(os.path.join(settings.pickle_dir, pickle_name + '.pkl'), 'wb'))
    return

def plot_from_pickle(ax, pickle_name, color='k', last_plot=True, label='', fit='exp_const'):
    outgassing_dict = pickle.load(open(os.path.join(settings.pickle_dir, pickle_name), 'rb'))
    plot_outgassing_errors(
                            ax,
                            outgassing_dict['time_hrs'],
                            outgassing_dict['outgassings']/288.75948,
                            outgassing_dict['outgassing_errs']/288.75948,
                            settings.area,
                            color=color,
                            last_plot=last_plot,
                            label=label,
                            fit=fit
                          )
    return

def calculate_outgassing(filename, t0=0, input_mass = 32.0):
    measurement_times, measurement_pressures, t0 = find_asymptotes(filename, t0, plot=False, plot_fits=False)
    for measurement_index in settings.skip_measurements:
        measurement_times.pop(measurement_index)
        measurement_pressures.pop(measurement_index)
    rga_xml_filenames = get_rga_filenames(measurement_times)
    conductance = settings.coeff*settings.trans_prob*1.0/(settings.area*np.sqrt(input_mass))
    outgassings = np.zeros(len(measurement_pressures), dtype=np.float32)
    outgassing_errs = np.zeros(len(measurement_pressures), dtype=np.float32)
    time_hrs = np.zeros(len(measurement_pressures), dtype=np.float32)
    print('\n======== Calculating Outgassing Rate ========')
    print("Time (hrs):\tOutgassing (torr*l/s):\tPeak Fraction:")
    peak_fraction = settings.initial_peak_fraction
    for (i, pressure) in enumerate(measurement_pressures):
        try:
            peak_fraction = analyze_rga_scan(
                                                rga_xml_filenames[i],
                                                pressure,
                                                input_mass=input_mass,
                                                plot=False
                                            )
        except IOError:
            if (i==0) and (peak_fraction==0):
                print('\nCouldn\'t find first scan. Please input fraction manually or skip.\n')
                sys.exit()
            else:
                print('Couldn\'t find RGA scan %s. Using previous/input peak fraction' % rga_xml_filenames[i])
        outgassings[i] = pressure * peak_fraction * conductance
        pressure_err = settings.pressure_stat_err*pressure
        outgassing_errs[i] = (pressure_err/pressure)*outgassings[i]
        time_hrs[i] = (measurement_times[i] - t0)/3600.0
        print('%.2f\t\t%.2e\t\t%.2e' % (time_hrs[i], outgassings[i], peak_fraction))
    fig = plt.figure(figsize=(10,6))
    ax = fig.add_subplot(111)
    plot_outgassing_errors(ax, time_hrs, outgassings, outgassing_errs, settings.area, last_plot=True)
    return time_hrs, outgassings, outgassing_errs


def calculate_oxygen_outgassing(args):
    time_hrs, outgassings, outgassing_errs = calculate_outgassing(args.calculate_oxygen_outgassing)
    if args.pickle_output:
        save_to_pickle(args.calculate_oxygen_outgassing.split('.')[0], time_hrs, outgassings, outgassing_errs)
    if args.plot:
        plt.show()

def plot_two_from_pickle(args):
    fig = plt.figure(figsize=(10,6))
    ax = fig.add_subplot(111)
    labels = ['%i' % i for i in range(len(args.plot_comparison_from_pickle))]
    if args.labels:
        labels = args.labels
    plot_from_pickle(ax, args.plot_comparison_from_pickle[0], color='r', last_plot=False, label=labels[0], fit='exp_exp')
    plot_from_pickle(ax, args.plot_comparison_from_pickle[1], color='b', last_plot=True, label=labels[1], fit='exp_exp')
    ax.grid()
    plt.legend(loc='best')
    plt.xlim([10,150])
    #plt.ylim([1e-12, 3e-11])
    plt.ylim([3e-15, 1e-13])
    plt.savefig('ptfe_torlon_comparison_double_exponential_constant_fit_big_specific.png')
    plt.show()

def main():
    args = get_parsed_args()
    if args.calculate_oxygen_outgassing:
        calculate_oxygen_outgassing(args)
        sys.exit()
        time_hrs, outgassings, outgassing_errs = calculate_outgassing(args.calculate_oxygen_outgassing)
        plt.show()
        sample_temps = [24.5, 2.3, -11.9, -15.9, -11.9, -7.0]
        plate_temps = [28.0, -17.6, -47.9, -72.7, -117.5, -30.1]
        fig = plt.figure(figsize=(10,6))
        plt.plot(outgassings, sample_temps, 'r.', label='Sample')
        plt.plot(outgassings, plate_temps, 'b.', label='Plate')
        plt.xscale('log')
        plt.grid()
        plt.xlabel('Oxygen Outgassing [torr*l/s]')
        plt.ylabel('Temperature [deg C]')
        plt.legend(loc=4)
        plt.show()
    elif args.plot_comparison_from_pickle:
        plt.rcParams['font.size'] = 18
        import matplotlib
        matplotlib.rcParams.update({
        'xtick.major.size' : 16,
        'xtick.minor.size' : 8,
        'ytick.major.size' : 16,
        'ytick.minor.size' : 8,
        'xtick.major.width' : 2,
        'xtick.minor.width' : 2,
        'ytick.major.width' : 2,
        'ytick.minor.width' : 2,

        'xtick.direction' : 'in',
        'ytick.direction' : 'in',
        'lines.markersize' : 12,
        'lines.markeredgewidth' : 3,
        'errorbar.capsize' : 8,

        'lines.linewidth' : 3,
        'lines.linestyle' : 'None',
        'lines.marker' : 'None',

        'savefig.bbox' : 'tight',
        })
        plot_two_from_pickle(args)
    elif args.plot_pressure:
        plot_pressure(args.plot_pressure)
    elif args.plot_temperatures:
        plot_only_temperatures(args.plot_temperatures)
    elif args.plot_measurements:
        find_asymptotes(args.plot_measurements, plot=True, plot_fits=True)

if __name__=='__main__':
    main()
