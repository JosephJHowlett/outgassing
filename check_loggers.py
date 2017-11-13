#!/usr/bin/python

import smtplib, os.path, time
from email.mime.text import MIMEText
import sys, os
import numpy as np

from subprocess import check_output


def get_pids(name):
   return map(int, check_output(["pidof",name]).split())


def are_loggers_running(pids):
   for pid in pids:
      try:
         os.kill(pid, 0)
      except OSError:
         return False
   return True


# not using daemon anymore, need to check for processes by pid
# first, get the pids (MAKE SURE ONLY PYTHON PROCESSES ARE LOGGERS)
def get_current_python_pids():
   pids = get_pids('python')
   pids.pop(np.where(np.asarray(pids, dtype=np.int32)==os.getpid())[0][0])
   return pids

pids = get_current_python_pids()
if len(pids)!=2:
   print('Found %i python processes running, please have only the two loggers running.' % len(pids))
   sys.exit()

i = 0
while i<1:
   #if os.path.isfile('log_daemon/log_daemon.pid')==False:
   # Now continuously check that both pids are still running
   if not are_loggers_running(pids):
      # Loggers not running, send email using old daemon.checker email
      daemon = 'log.daemon.checker@gmail.com'
      me = 'jhowl01@gmail.com'
      print('Logger not running, sending email to %s' % me)
      msg = MIMEText('For some reason, your temperature and/or pressure logger has stopped running.')
      msg['Subject'] = 'Logger Not Running'
      msg['From'] = daemon
      msg['To'] = me
      server = smtplib.SMTP('smtp.gmail.com',587)
      server.ehlo()
      server.starttls()
      server.ehlo()
      server.login("log.daemon.checker","logdcheck")
      server.sendmail(daemon, me, msg.as_string())
      server.quit()
      pids = get_current_python_pids()
      while len(pids) < 2:
         time.sleep(60)
         pids = get_current_python_pids()
   time.sleep(600)
