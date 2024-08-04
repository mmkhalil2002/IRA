#!/bin/bash

# Share Eth with WiFi Hotspot
#
# This script is created to work with Raspbian Stretch
# but it can be used with most of the distributions
# by making few changes.
#
# Make sure you have already installed `dnsmasq` and `hostapd`
# Please modify the variables according to your need
# Don't forget to change the name of network interface
# Check them with `ifconfig`

#install GobiNet
./config_gobinet.sh

# install qmi
./install_auto_connect.sh
#dns configuration


FILE=/etc/modprobe.d/blacklist-modem.conf
if [ ! -f "$FILE.orig" ]; then
	echo "cp  $FILE $FILE.orig"
	mv $FILE  $FILE.orig
fi
echo -e "#automatic generated file \n\
blacklist qcserial \n\
blacklist qmi_wwan" >> $FILE



#/configure dns
./config_wifi_dns.sh


sudo reboot
