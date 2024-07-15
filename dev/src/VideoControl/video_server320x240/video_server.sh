#/bin/bash

#mount /dev/sda1   /media/usb
video_server  -v 2 -p 14095 -c 14098& 
video_server  -v 0 -p 4095 -c 4098 &
