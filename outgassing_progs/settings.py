import os

head_dir = '/Users/josephhowlett/research/outgassing/'
pressure_dir = os.path.join(head_dir, 'pressure_data')
rga_dir = os.path.join(head_dir, 'rga_xmls')


area = 1
initial_jump_threshold = 0.5
offtime_threshold_seconds = 500
coeff = 12.23
trans_prob = 0.245

pressure_stat_err = 0.05
