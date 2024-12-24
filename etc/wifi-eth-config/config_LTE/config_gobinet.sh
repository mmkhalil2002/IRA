
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
#sudo apt-get update
#sudo apt-get install linux-source
#sudo apt-get install build-essential
#sudo apt-get install libncurses5-dev libncursesw5-dev
#sudo cd /usr/src
#sudo tar -xf linux-source-4.15.0.tar.bz2

path=$(uname -r)
echo "/usr/src/linux-headers-$path/"
sudo ln -s /usr/src/linux-headers-$path/arch/arm /usr/src/linux-headers-$path/arch/armv7l
filepath=`pwd`
echo $filepath
cd $filepath/Quectel-Linux-4G5G
make clean
make RAWIP=1
make install
