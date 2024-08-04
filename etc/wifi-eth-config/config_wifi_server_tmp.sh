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

#ip_address="192.168.2.1"
#netmask="255.255.255.0"
#dhcp_range_start="192.168.47.1"
#dhcp_range_end="192.168.47.200"
#ip_addr="192.168.47.1"
#dhcp_time="12h"
#eth="eth0"
#wlan="wlan0"
#ssid="dronenetpi4-1-5"
#psk="123456789"


#./config_wifi_dns.sh
#install GobiNet
./config_gobinet.sh

echo "after gob"

# install qmi
./install_auto_connect.sh
echo "after auto"
