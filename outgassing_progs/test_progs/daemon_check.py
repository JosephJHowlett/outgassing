import smtplib, os.path, time
from email.mime.text import MIMEText

i = 0
while i<1:
   if os.path.isfile('outgassing_combined.c')==False:
      msg = MIMEText('For some reason, your daemon is not running.')
      msg['Subject'] = 'Daemon Not Running'
      daemon = 'log.daemon.checker@gmail.com'
      me = 'jhowl01@gmail.com'
      msg['From'] = daemon
      msg['To'] = me
      server = smtplib.SMTP('smtp.gmail.com',587)
      server.ehlo()
      server.starttls()
      server.ehlo()
      server.login("log.daemon.checker","logdcheck")
      server.sendmail(daemon, me, msg.as_string())
      server.quit()
   time.sleep(1800)