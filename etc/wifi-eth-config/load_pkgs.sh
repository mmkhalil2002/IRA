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

mkdir /usr/local/etc/smartrobot

sudo apt-get clean
sudo mv /var/lib/apt/lists /tmp
sudo mkdir -p /var/lib/apt/lists/partial
sudo apt-get clean
sudo apt-get update
sudo apt-get install libncurses5-dev -y
sudo apt-get install cmake -y
sudo apt-get install net-tools -y


sudo apt install xz-utils
sudo apt install unzip
sudo apt install gcc -y
sudo apt install g++ -y
sudo apt-get update    #Update the list of packages in the software center
sudo apt-get install screen python-matplotlib  python3-pip python-numpy python-dev libxml2-dev libxslt-dev python-lxml -y

sudo apt-get install libxml2-dev libxslt1-dev -y
sudo apt install  python3-pip -y
sudo pip3 install pymavlink
sudo pip3 install mavproxy
sudo pip3 install dronekit
sudo pip3 install dronekit-sitl

#sudo apt-get update    #Update the list of packages in the software center
#sudo apt-get install screen python-matplotlib python-opencv python-pip python-numpy python-dev libxml2-dev libxslt-dev python-lxml -y
#sudo pip install future -y
#sudo apt-get update --fix-missing
#sudo apt install python-pip -y
#sudo apt-get install libxml2-dev libxslt1-dev -y
#sudo pip install pymavlink -y
#sudo pip install mavproxy -y
#sudo pip install dronekit -y
#sudo pip install dronekit-sitl  -y
# download examples
git clone http://github.com/dronekit/dronekit-python.git
sudo apt-get install gpsd gpsd-clientsa -y


./qmi_install_4G5G.sh

#sudo apt update
#sudo add-apt-repository ppa:jonathonf/ffmpeg-4
#sudo apt install ffmpeg -y



sudo apt-get install rng-tools
sudo echo -e "HRNGDEVICE=/dev/urandom:" >> /etc/default/rng-tools
sudo /etc/init.d/rng-tools start
sudo mv /dev/random /dev/random.orig
sudo ln -s /dev/urandom /dev/random


sudo apt-get install hostapd
sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo echo -e "DAEMON_CONF=\"/etc/hostapd/hostapd.conf\"">> /etc/default/hostapd


sudo apt update && sudo apt upgrade -y
sudo apt-get install cheese v4l-utils -y
sudo apt install -y dnsmasq


sudo echo -e "#disable wlan0 \n\
blacklist brcmfmac \n\
blacklist brcmutil" >> /etc/modprobe.d/blacklist.conf


sudo apt-get remove dhcpcd5


#to get the source code of ffmpeg
#cd /home/ubuntu/
sudo apt-get install -y pkg-config
sudo apt-get install yasm libvpx. libx264. -y
#git clone git://source.ffmpeg.org/ffmpeg.git
cd ffmpeg
sudo ./configure --arch=armel --target-os=linux --enable-gpl --enable-libx264 --enable-nonfree --enable-gray

sudo make && make install  // this talks a long time


cd /home/ubuntu/
git clone https://github.com/opencv/opencv.git
cd opencv && mkdir Release && cd Release
cmake -D CMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/usr/local ..
make
make install

sudo apt-get install network-manager network-manager-gnome


git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi/
./build
cd ../
rm -r WiringPi

sudo reboot
