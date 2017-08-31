#!/bin/bash

echo -e "Please enter todays date followed by desired extension, e.g. YYYYMMDD.dat: \c "
read date
echo "Saving file $date"
echo `cat pfeiffer.log | sed -e 's/,/ /g' | awk '{ print $2, substr($3,20,8)}' > $date`
