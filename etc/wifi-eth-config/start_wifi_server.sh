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

eth="eth0"
#eth="usb0"
wlan="wlan0"
#
#check if wlan0 is up
#
i="0"
while true; do
    if [ $i -le 10 ]; then
      i=$[$i+1]
    else
       exit;
    fi
    ip route | grep $wlan 
    if [ $? -eq 0 ]; then
        break   # network up
        echo "net found"  $i
    else
        sleep 1  # network not yet up
        echo "net not found"  $i
    fi
done




sudo killall wpa_supplicant &> /dev/null
sudo rfkill unblock wlan &> /dev/null
sleep 2

sudo systemctl start network-online.target

sudo iptables -F
sudo iptables -t nat -F
sudo iptables -t nat -A POSTROUTING -o $eth -j MASQUERADE
sudo iptables -A FORWARD -i $eth -o $wlan -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i $wlan -o $eth -j ACCEPT

sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"

#sudo ifconfig $wlan $ip_address netmask $netmask
echo "will stop dmsmasq"
sudo systemctl stop dnsmasq
echo "stoped dmsmasq"

# Remove default route
#sudo ip route del 0/0 dev $wlan &> /dev/null

echo "will start dmsmasq"
sudo systemctl start dnsmasq
echo "started dmsmasq"
echo "will start hostapd"
sudo systemctl stop hostapd
sudo systemctl start hostapd
echo "started hostapd"
#sudo hostapd /etc/hostapd/hostapd.conf &
