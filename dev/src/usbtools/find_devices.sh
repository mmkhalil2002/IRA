#!/bin/bash

port=".3-port"

if [ "$1" = "-port" ]
then
port="port"
fi

#x=`lsusb -t | grep -i serial | grep "GobiSerial" | grep "If 2" | awk '{print $3}'|sed 's/://g'`
x=`lsusb -t | grep "option" | grep "If" |  awk '{print $7}'|sed 's/://g'`
y=`echo $x |sed 's/ //g'`
#echo devises = $y
#ls /dev/serial/by-path/*[$y]* | grep $port | xargs readlink -f
ls /dev/serial/by-path/*[$y]*  | xargs readlink -f 


