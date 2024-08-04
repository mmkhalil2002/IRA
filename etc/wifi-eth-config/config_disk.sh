#!/bin/bash


current_date=$(date +"%Y-%m-%d")
current_time=$(date +"%H:%M:%S")

echo "Date: $current_date"
echo "Time: $current_time"
FILE=/etc/fstab
if [ ! -f "$FILE.orig" ]; then
	echo "cp  $FILE $FILE.orig"
	cp $FILE  $FILE.orig
echo -e "# autmatic generated script Data = $current_date Time = $current_time \n\
/dev/sda1       /media/usb_a          vfat    defaults        0       0 \n\
/dev/sdb1       /media/usb_b          vfat    defaults        0       0" >> $FILE
fi

mkdir -p  /media/usb_a
mkdir -p  /media/usb_b



