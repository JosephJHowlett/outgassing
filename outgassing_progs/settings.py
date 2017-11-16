import os

head_dir = '/Users/josephhowlett/research/outgassing/'
pressure_dir = os.path.join(head_dir, 'pressure_data')
temperature_dir = os.path.join(head_dir, 'temperature_data')
rga_dir = os.path.join(head_dir, 'rga_xmls')
pickle_dir = os.path.join(head_dir, 'outgassing_pkls')


area = 1
initial_jump_threshold = 0.5
offtime_threshold_seconds = 200
coeff = 12.23
trans_prob = 0.245

pressure_stat_err = 0.05

# indices of recognized measurements left off from outgassing calculation
skip_measurements = []
# manually set peak fraction if first rga scan is missing
# otherwise set to zero
initial_peak_fraction = 2.69e-8
