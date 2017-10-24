#!/usr/local/bin/python

# Usage './rga_xml_to_dat header' where header is the fname of the scan w/o extension
# in the directory specified by xml_fileheader. It is then converted to a scan .dat 
# readable by the outgassing code (analyze_rga_dat_new.c)

from xml.dom import minidom
import numpy as np
import sys, os

dat_fileheader = ''
xml_fileheader = ''

def xml_to_dat(fname):
   xmldoc = minidom.parse(fname)
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
   x_vals = np.linspace(low_lim, high_lim, nb_bins)
   val_list = xmldoc.getElementsByTagName('Sample')
   values = []
   value = 0
   for s in val_list:
      value = s.attributes['Value'].value
      values.append(value)
   # open new file
   dat_name = fname.split('.xml')[0] + '.dat'
   datfile = open(dat_fileheader + dat_name, 'w')
   datfile.write(str(tot_pressure) + '\n')
   for i, value in enumerate(values):
      datfile.write('%.3f\t%s\n' % (x_vals[i], value))
   datfile.close()


def main():
   for filename in os.listdir(os.getcwd()):
      if '.xml' in filename:
         xml_to_dat(filename)


if __name__ == '__main__':
   main()
