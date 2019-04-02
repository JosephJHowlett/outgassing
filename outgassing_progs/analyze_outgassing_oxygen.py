#! /usr/bin/python
import sys, os, scipy.stats
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
import temp_reader
from temp_reader import *
import settings
import plotters
from plotters import *

def get_parsed_args():
    parser = argparse.ArgumentParser(description="Analysis for outgassing data.")
    parser.add_argument(
                            '--calculate_oxygen_outgassing', '-o',
                            help = """Basic oxygen outgassing calculation. Input pressure file,
                                      outputs pickled outgassing data and plots with fit line."""
                       )
    parser.add_argument(
                            '--cooling',
			    nargs = 2,
                            help = """Input pressure file, then temp file"""
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
                            '--plot_both',
			    nargs = 2,
                            help = """Input pressure file, then temp file"""
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
    parser.add_argument(
			    '--coef', nargs=2,
			    help = "Input pressure file then temp file"
			)
    return parser.parse_args()


def asymptotic(x, amp, rate, mean):
    return amp*(1.0 - np.exp(-1*rate*(x - mean)))

def find_asymptotes(filename, t0=0, plot=True, plot_fits=True, bypass = 'on'):
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
    if bypass == 'on':
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
                    (times[i+1] - times[i] > settings.offtime_threshold_seconds)
               ):
               #  gauge about to be turned off - measurement taken
                end_indices.append(i)
	        if climbing ==0:
	            start_indices.append(i-20)
                climbing = 0
        print('Found %i measurements' % len(start_indices))
        if t0==0:
            t0 = times[0]
        measurement_times = [times[i] for i in end_indices]

        # initialize the pressures at the final values
        measurement_pressures = [pressures[i] for i in end_indices]
        #measurement_pressures = [pressures[i+36] for i in start_indices]
	#adjusted_indices = [(i + 36) for i in start_indices]
	#adjusted_indices[8] = end_indices[8]
	#adjusted_indices[9] = end_indices[9]
	pressure_err = []

    # then loop through and fit each set to get ultimate pressure
        for i in range(len(end_indices)):
            times_in_range = times[start_indices[i]+1:end_indices[i]]
	    better_range = times[start_indices[i]+10:end_indices[i]]
            pressures_in_range = pressures[start_indices[i]+1:end_indices[i]]
	    better_range_p = pressures[start_indices[i]+10:end_indices[i]]

	    #times_in_range = times[start_indices[i]+1:adjusted_indices[i]]
            #pressures_in_range = pressures[start_indices[i]+1:adjusted_indices[i]]
            popt, popcov = curve_fit(asymptotic, times_in_range, pressures_in_range, p0=[pressures_in_range[-1], 1.0/1200.0, times_in_range[0]])
	    popt, popcov = curve_fit(asymptotic, better_range, better_range_p, p0=[better_range_p[-1], 1.0/1200.0, times_in_range[0]])
            measurement_pressures[i] = popt[0]
	    pressure_err.append(settings.pressure_stat_err*measurement_pressures[i])
            if plot_fits:
                plt.plot(times_in_range, pressures_in_range, 'k.')
                x = np.linspace(times_in_range[0], times_in_range[-1], len(times_in_range)*10)
                plt.plot(x, asymptotic(x, *popt), 'r--')
                plt.show()
    
    if bypass == 'off':
	custom_range = [100, 100, 100, 100]
	counter = 0
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
                    (times[i+1] - times[i] > settings.offtime_threshold_seconds)
               ):
                # gauge about to be turned off - measurement taken
                end_indices.append(i)
	        start_indices.append(i-custom_range[counter])
		#start_indices.append(i-60)
		counter = counter + 1
	#end_indices[3] = end_indices[3] - 120
	#start_indices[3] = start_indices[3] - 120
	holder = [start_indices[1], end_indices[1]]
	holder2 = [start_indices[2], end_indices[2]]
	start_indices[1] = start_indices[0] + 500
	end_indices [1] = end_indices[0] + 500
	start_indices[2] = (start_indices[1] + 700)
	end_indices[2] = (end_indices[1] + 900)
	start_indices.append(holder[0])
	end_indices.append(holder[1])
	start_indices.append(holder2[0])
	end_indices.append(holder2[1])
        print('Found %i measurements' % len(start_indices))
        if t0==0:
            t0 = times[0]
        measurement_times = [times[i] for i in end_indices]

        # initialize the pressures at the final values
        measurement_pressures = []
	pressure_err = []

        # find median pressure in range
        for i in range(len(end_indices)):
            times_in_range = times[start_indices[i]+1:end_indices[i]]
            pressures_in_range = pressures[start_indices[i]+1:end_indices[i]]
            measurement_pressures.append(np.median(pressures_in_range))
	    pressure_err.append(np.std(pressures_in_range))

    #time error bars
    datetimes = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])

    first_indices = []
    for i in end_indices:
        j = i + 1
	if j < len(pressures) -1:
	    while pressures[j] <= 0 and j < len(pressures) -1:
	        j = j+ 1
	    if pressures[j] > 0:
	        first_indices.append(j)

    ind_errs = []	
    for i in range(len(first_indices)):
        d = datetimes[first_indices[i]] - datetimes[end_indices[i]]
	ind_errs.append(d/2)
	
    time_errs = []
    for i in range(len(measurement_times)):
	if i <= len(ind_errs) - 1:
	    time_errs.append(ind_errs[i])
	else:
	    time_errs.append(np.mean(ind_errs))

    for i in range(len(measurement_times)):
	measurement_times[i] = measurement_times[i] + time_errs[i]*3600*24

    if plot:
        for measurement_index in settings.skip_measurements:
            start_indices.pop(measurement_index)
            end_indices.pop(measurement_index)
	    first_indices.pop(measurement_index)
        fig = plt.figure(figsize=(10,6))
        ax = fig.add_subplot(111)
        datetimes = dates.date2num([datetime.fromtimestamp(time-(4*3600), tz=tz.tzutc()) for time in times])

        
	#plotting the points on pressure diagram
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
	ax.plot_date(
                        [datetimes[i] for i in end_indices],
                        [measurement_pressures[i] for i in range(len(end_indices))],
                        'm.',
                        label='Avg. pressure'
                    )
        #ax.plot_date(
                        #[datetimes[i] for i in adjusted_indices],
                        #[pressures[i] for i in adjusted_indices],
                        #'.',
                        #label='Scan Begin Points-- adulterated'
                    #)
	#ax.plot_date(
			#[datetimes[i] for i in first_indices],
			#[pressures[i] for i in first_indices],
			#'c.',
			#label = 'First point after RGA'
		   # )
        date_format = dates.DateFormatter('%Y/%m/%d\n%H:%M')
        ax.xaxis.set_major_formatter(date_format)
        fig.autofmt_xdate()
        plt.yscale('log')
        plt.ylabel('Pressure [Torr]')
        plt.legend(loc='best')
        plt.show()
    return measurement_times, measurement_pressures, t0, time_errs, pressure_err


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


def load_rga_xml(filename, plot=True):
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


def load_rga_trend_xml(filename, plot=True):
    xmldoc = minidom.parse(os.path.join(settings.rga_dir, filename))
    top_line = xmldoc.getElementsByTagName('Data')
    val_list = xmldoc.getElementsByTagName('Sample')
    partial_pressures = np.zeros((settings.trend_n_peaks, len(val_list)/settings.trend_n_peaks))
    for i in range(len(val_list)):
        value = val_list[i].attributes['Value'].value
        partial_pressures[i % settings.trend_n_peaks, i/settings.trend_n_peaks] = value
    time_per_point = 5 * settings.trend_n_peaks * settings.trend_dwell
    times = np.array(range(settings.trend_samples))*time_per_point # seconds
    if plot:
        plt.figure(figsize=(10,6))
        plt.plot(times, partial_pressures[settings.trend_peaks['oxygen']/settings.trend_peaks['total']])
        #for pp in partial_pressures:
        #    plt.plot(range(len(pp)), pp)
        plt.show()
    return times, partial_pressures, tot_pressure


def analyze_rga_scan(filename, pressure, input_mass=32.0, plot=True):
    # simple scanning based on max pressure amplitude +- 0.3 AMU from input_mass
    masses, partial_pressures, total_rga_pressure = load_rga_xml(filename, plot=plot)
    closest_mass = np.argmin(np.absolute(masses - input_mass))
    pressure_range = [float(partial_pressures[closest_mass])]
    for i in [1,2,3]:
        pressure_range.append(float(partial_pressures[closest_mass + i]))
	pressure_range.append(float(partial_pressures[closest_mass - i]))
    peak_pressure = max(pressure_range)
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
                            outgassing_errs, time_errs, area,
                            fit='exp_const', color='k',
                            last_plot=True, label=''
                          ):
    # tries to fit the data to input function, then plots
    # TODO: switch to -log likelihood minimizer
    outgassings = outgassings/area
    plt.errorbar(time_hrs, outgassings, yerr=outgassing_errs, xerr=time_errs, fmt='%s.' % color, label=label)
    #ax.grid(which = 'minor')
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
    plt.ylabel('Oxygen Outgassing [$Torr*l/s$]')
    plt.savefig('expofit_180720_180725_ptfe_run.png')

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

def calculate_outgassing(filename, t0= 1532040900, input_mass = 32.0):
    measurement_times, measurement_pressures, t0, time_errs, pressure_err = find_asymptotes(filename, t0, plot=False, plot_fits=False)

    #t0=1532040900, 1531171200, 1504394760
    #plate_temps = [27.23, -18.47, -56.88, -96.22, -138]
    #fig = plt.figure(figsize=(10,6))
    #plt.plot(plate_temps, measurement_pressures, 'b.')
    #plt.yscale('log')
    #plt.grid()
    #plt.xlabel('Temperature [deg C]')
    #plt.ylabel('Pressure [Torr]')
    #plt.show()

    for measurement_index in settings.skip_measurements:
        measurement_times.pop(measurement_index)
        measurement_pressures.pop(measurement_index)
	time_errs = np.delete(time_errs, measurement_index)

    rga_xml_filenames = get_rga_filenames(measurement_times)
    conductance = settings.coeff*settings.trans_prob*1.0/(settings.area*np.sqrt(input_mass))
    outgassings = np.zeros(len(measurement_pressures), dtype=np.float32)
    outgassing_errs = np.zeros(len(measurement_pressures), dtype=np.float32)
    time_hrs = np.zeros(len(measurement_pressures), dtype=np.float32)
    
    print('\n======== Calculating Outgassing Rate ========')
    print("Time (hrs):\tOutgassing (torr*l/s):\tPeak Fraction:")
    peak_fraction = settings.initial_peak_fraction
    pf = []
    #uh_oh = [1.02e-08, 1.5e-08, 2.1e-08, 2.46e-08, 3.1e-08]
    #uh_oh = [1.5e-08, 1.5e-08, 1.5e-08, 1.5e-08, 1.5e-08] 
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
	#peak_fraction = uh_oh[i]        
	outgassings[i] = pressure * peak_fraction * conductance
        outgassing_errs[i] = (pressure_err[i]/pressure)*outgassings[i]*1.5
        time_hrs[i] = (measurement_times[i] - t0)/3600.0
	pf.append(peak_fraction)
        print('%.2f\t\t%.2e\t\t%.2e' % (time_hrs[i], outgassings[i], peak_fraction))
    fig = plt.figure(figsize=(10,6))
    ax = fig.add_subplot(111)
    plot_outgassing_errors(ax, time_hrs, outgassings, outgassing_errs, time_errs, settings.area, last_plot=True)

    fig = plt.figure(figsize=(10,6))
    plt.plot(time_hrs, pf, 'b.')
    plt.grid()
    plt.xlabel('Time [Hrs]')
    plt.ylabel('Peak fraction')
    plt.show()
    import pandas as pd
    pd.DataFrame({'time_hrs': time_hrs, 'outgassing': outgassings, 'err': outgassing_errs}).to_csv('180720_180725_ptfe_run_o2_outgassing.csv')
    return time_hrs, outgassings, outgassing_errs, time_errs, measurement_times


def calculate_oxygen_outgassing(args):
    time_hrs, outgassings, outgassing_errs, time_errs, measurement_times = calculate_outgassing(args.calculate_oxygen_outgassing)
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

def temp_plotter(sample_temps, plate_temps, outgassings, outgassing_errs, sample_errs, plate_errs):
        fig = plt.figure(figsize=(10,6))
        plt.plot(sample_temps, outgassings, 'k.')
        #plt.plot(plate_temps, outgassings, 'b.', label='Plate')
	for i in range(len(plate_temps)):
	    plt.errorbar(sample_temps[i], outgassings[i], yerr=outgassing_errs[i], xerr = sample_errs[i], fmt ='k.')
        plt.yscale('log')
        plt.grid()
        plt.xlabel('Temperature [deg C]')
        plt.ylabel('Oxygen Outgassing [torr*l/s]')
        plt.legend(loc=4)
	plt.show()
	print sample_temps
	return

def coef_checker(filename_p, filename_t):
    with open(os.path.join(settings.pressure_dir, filename_p), 'r') as f:
        lines = f.readlines()
    times = []
    pressures = []
    for line in lines:
        if len(line.split()) > 2:
            continue
        times.append(int(line.split()[0]))
        pressures.append(float(line.split()[1]))
    pressure_data = [list(i) for i in zip (times, pressures)]

    with open(os.path.join(settings.temperature_dir, filename_t), 'r') as f:
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
    amb_data = [list(i) for i in zip (times, t_ambient)]

    coef = scipy.stats.pearsonr(t_ambient, pressures)

    return
	


if __name__=='__main__':
    args = get_parsed_args()

    if args.calculate_oxygen_outgassing:
        time_hrs, outgassings, outgassing_errs, time_errs, measurement_times = calculate_outgassing(args.calculate_oxygen_outgassing)
        plt.show()

    elif args.cooling:
	time_hrs, outgassings, outgassing_errs, time_errs, measurement_times = calculate_outgassing(args.cooling[0])
	plt.show()
	sample_temps, plate_temps, sample_errs, plate_errs = temp_finder(measurement_times, args.cooling[1])
	#sample_temps[4] = -102
	#plate_temps[4] = -138
	print plate_temps
	temp_plotter(sample_temps, plate_temps, outgassings, outgassing_errs, sample_errs, plate_errs)

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
    elif args.plot_both:
	print args.plot_both
	plot_both(args.plot_both[0], args.plot_both[1])
    elif args.plot_measurements:
        find_asymptotes(args.plot_measurements, plot=True, plot_fits=True, bypass='on')
    elif args.coef:
	coef_checker(args.coef[0], args.coef[1])
