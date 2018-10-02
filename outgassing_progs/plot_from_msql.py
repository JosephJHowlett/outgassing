import sys, os, time, calendar, signal
import MySQLdb
from array import array
import numpy as np
import settings


def get_data_from_mysql(table='pfeiffer0', t0=1410802453, t1=int(time.time())):

    database_user = 'smac_user'
    database_pass = 'fr45ju76'

    if table not in ['ard0', 'pfeiffer0']:
        raise ValueError('Table must be either ard0 or pfeiffer0!')

    pid = os.spawnlp(os.P_NOWAIT, 'ssh', 'ssh', '-L', '3307:localhost:3306', '-N', 'outgas@outgas.astro.columbia.edu')
    time.sleep(4)

    # open the connection to the database
    try:
        connection = MySQLdb.connect('127.0.0.1', database_user, database_pass, 'smac', port=3307)
        cursor = connection.cursor()
    except MySQLdb.Error, e:
        print 'problem connection to run database, error %d: %s' % (e.args[0], e.args[1])
        sys.exit(1)

    if table=='ard0':
        query = 'select ard0_id,T_Amb,T_Plate,T_Sample from ard0 where (ard0_id > %i) && (ard0_id < %i);' % (t0, t1)
    else:
        query = 'select pfeiffer0_id,P_Vacuum_Chamber from pfeiffer0 where (pfeiffer0_id > %i) && (pfeiffer0_id < %i);' % (t0, t1)
    cursor.execute(query)
    data = cursor.fetchall()
    connection.close()

    os.kill(pid, signal.SIGTERM)

    return data

def write_to_file(data, outfile, table='pfeiffer0'):
    with open(outfile, 'w') as f:
        if table=='pfeiffer0':
            for row in data:
                f.write('%i\t%.2e\n' % (row[0] - 3600, row[1]))
        else:
            for row in data:
                f.write('%i\t%.2f\t%.2f\t%.2f\n' % (row[0] - 3600, row[1], row[2], row[3]))

if __name__ == '__main__':
    table_map = {'temperatures': 'ard0', 'pressures': 'pfeiffer0'}
    dir_map = {'temperatures': settings.temperature_dir, 'pressures': settings.pressure_dir}
    usage = 'Usage:\npython plot_from_mysql.py <\"pressures\" or \"temperatures\"> <output file name> <(opt.) t_i> <(opt.) t_f>'
    try:
        table = table_map[sys.argv[1]]
        outfile = os.path.join(dir_map[sys.argv[1]], sys.argv[2])
    except IndexError:
        print(usage)
        sys.exit()
    t0 = 1410802453
    if len(sys.argv)>3:
        t0 = sys.argv[3]
    t1 = int(time.time())
    if len(sys.argv)>4:
        t1 = sys.argv[4]

    data = get_data_from_mysql(table, t0, t1)
    write_to_file(data, outfile, table)

