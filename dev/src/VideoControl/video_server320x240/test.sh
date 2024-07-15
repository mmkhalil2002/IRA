#/bin/bash

#disk='mlsblk -no UUID /dev/sda1'
uuid=`lsblk -no UUID /dev/sda1`
#disk='lsblk -no UUID /dev/sda1'
disk='pwd'
echo $disk		$current_dir
