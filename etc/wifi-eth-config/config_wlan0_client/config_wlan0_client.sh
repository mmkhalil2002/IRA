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

net=TMOBILE-9B81
psk=khalil02
#net=dronenetpi4-1-2
#psk=123456789
#kill -9 `ps aux | grep wpa | awk '{print $2}'`

pkill wpa

./config_wifi_dns.sh
mkdir -p /usr/local/robot/etc/systemd/system

FILE=/etc/netplan/50-cloud-init.yaml
#FILE=/etc/netplan/01-netcfg.yaml
if [ ! -f "$FILE.client.orig" ]; then
	echo "mv  $FILE $FILE.client.orig"
	mv $FILE  $FILE.client.orig
fi


current_date=$(date +"%Y-%m-%d")
current_time=$(date +"%H:%M:%S")

echo "Date: $current_date"
echo "Time: $current_time"

echo -e "# autmatic generated script Data = $current_date Time = $current_time \n\
network: \n\
    version: 2  \n\
    ethernets: \n\
        eth0: \n\
          dhcp4: true \n\
          dhcp6: true \n\
          optional: true \n\
    wifis: \n\
        wlan0: \n\
          dhcp4: true \n\
          dhcp6: true \n\
          access-points: \n\
                \"TMOBILE-9B81\": \n\
                 password: \"khalil02\"" > $FILE

systemctl  stop     config_wlan0_1
systemctl  stop     config_wlan0_2
systemctl  stop     config_wlan0_3
systemctl  disable  config_wlan0_1
systemctl  disable  config_wlan0_2
systemctl  disable  config_wlan0_3
sudo cp -f ./config_wlan0_ip.sh  /usr/local/bin
sudo cp -f ./netplan.sh  /usr/local/bin
sudo cp -f config_wlan0_1.service /usr/local/robot/etc/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/config_wlan0_1.service /lib/systemd/system
sudo cp -f config_wlan0_2.service /usr/local/robot/etc/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/config_wlan0_2.service /lib/systemd/system
sudo cp -f config_wlan0_3.service /usr/local/robot/etc/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/config_wlan0_3.service /lib/systemd/system
systemctl  enable     config_wlan0_1
systemctl  enable     config_wlan0_2
systemctl  enable     config_wlan0_3
systemctl  start      config_wlan0_1
systemctl  start      config_wlan0_2
systemctl  start      config_wlan0_3

#sudo iptables -F
#sudo iptables -t nat -F
#sudo iptables -t nat -A POSTROUTING -o $eth -j MASQUERADE
#sudo iptables -A FORWARD -i $eth -o $wlan -m state --state RELATED,ESTABLISHED -j ACCEPT
#sudo iptables -A FORWARD -i $wlan -o $eth -j ACCEPT

sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"


: << 'WPA_SUPPLICANT'
# ------------------  start configure wlan0 using wpa_supplicant.conf
# 
# starting the wpa_supplicant.service is necessary to initiate the Wi-Fi
# scanning process and collect the SSIDs of nearby Wi-Fi networks, which allows users
# to view and connect to the available networks using the network management tool.

FILE=/etc/wpa_supplicant.conf
if [ ! -f "$FILE.orig" ]; then
    if [  -f "$FILE" ]; then
	echo "mv  $FILE $FILE.orig"
	mv $FILE  $FILE.orig
    fi
fi

echo -e "network={\n\
        ssid=\"$net\" \n\
        psk=\"$psk\" \n\
        scan_ssid=1  \n\
}"  > /etc/wpa_supplicant.conf
FILE=/etc/systemd/system/wpa_supplicant.service
if [ ! -f "$FILE.orig" ]; then
    if [  -f "$FILE" ]; then
	echo "mv  $FILE  $FILE.orig"
	mv  $FILE  $FILE.orig
    fi
fi

echo -e  "[Unit] \n\
Description=WPA supplicant \n\
Before=network.target \n\
After=dbus.service \n\
Wants=network.target \n\
IgnoreOnIsolate=true \n\
\n\
[Service] \n\
Type=dbus \n\
BusName=fi.w1.wpa_supplicant1 \n\
ExecStart=/sbin/wpa_supplicant -u -s -c /etc/wpa_supplicant.conf -i  wlan0 \n\
\n\
[Install]\n\
WantedBy=multi-user.target \n\
Alias=dbus-fi.w1.wpa_supplicant1.service" > /etc/systemd/system/wpa_supplicant.service



sudo systemctl start wpa_supplicant.service
sudo systemctl enable wpa_supplicant.service 
sudo systemctl daemon-reload


FILE=/etc/systemd/system/dhclient.service
if [ ! -f "$FILE.orig" ]; then
    if [  -f "$FILE" ]; then
        echo "mv  $FILE  $FILE.orig"
        mv  $FILE  $FILE.orig
    fi
fi

# allocate IP address for the client using dhcleint

echo -e  "[Unit] \n\
Description= DHCP Client \n\
Before=network.target \n\
After=wpa_supplicant.service \n\
\n\
[Service]\n\
Type=forking \n\
ExecStart=/sbin/dhclient wlan0 -v \n\
ExecStop=/sbin/dhclient wlan0 -r \n\
\n\
[Install] \n\
WantedBy=multi-user.target"  > /etc/systemd/system/dhclient.service


echo start enable dhclient

sudo systemctl enable dhclient.service
sudo systemctl start dhclient.service

echo end enable dhclient

WPA_SUPPLICANT




: << 'NETWORK_MANAGER'

# this method is used to create wifi cleint using NetworkMamager
# install NetworkManager. addthe fllowing into load_pkgs.sh
# sudo apt-get install network-manager

sudo nmcli --version
nmcli device wifi list
sudo systemctl start NetworkManager
sudo systemctl enable  NetworkManager
sudo nmcli device wifi connect $net password $psk
sudo nmcli connection show

NETWORK_MANAGER


#  this daemon is used to disable hostapd
# hostapd (host access point daemon) is a user space daemon software enabling a network
# interface card to act as an access point and authentication server.
# the following code is to disable access point as server
sudo systemctl stop hostapd
sudo systemctl disable  hostapd

