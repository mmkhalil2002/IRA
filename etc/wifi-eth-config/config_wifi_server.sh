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
dhcp_range_start="192.168.47.1"
dhcp_range_end="192.168.47.200"
ip_addr="192.168.47.1"
dhcp_time="12h"
#eth="eth0"
wlan="wlan0"
ssid="dronenetpi4-1-5"
psk="123456789"


#install GobiNet
./config_gobinet.sh

# install qmi
./install_auto_connect.sh
#dns configuration
./config_wifi_dns.sh

systemctl stop hotspot.service
systemctl disable hotspot.service

cp ./hotspot.service /etc/systemd/system
cp ./start_wifi_server.sh /usr/local/etc/smartrobot

systemctl daemon-reload
systemctl start hotspot.service
systemctl enable hotspot.service

sudo killall wpa_supplicant &> /dev/null
sudo rfkill unblock wlan &> /dev/null
sleep 2

sudo systemctl start network-online.target

FILE=/etc/modprobe.d/blacklist.conf
if [ ! -f "$FILE.orig" ]; then
	echo "cp  $FILE $FILE.orig"
	cp $FILE  $FILE.orig
fi
echo -e "#disable wlan0 \n\
blacklist brcmfmac \n\
blacklist brcmutil" >> $FILE


FILE=/etc/modprobe.d/blacklist-modem.conf
if [ ! -f "$FILE.orig" ]; then
	echo "cp  $FILE $FILE.orig"
	mv $FILE  $FILE.orig
fi
echo -e "#automatic generated file \n\
blacklist qcserial \n\
blacklist qmi_wwan" >> $FILE


#FILE=/etc/hosts
#if [ ! -f "$FILE.orig" ]; then
#	echo "mv  $FILE $FILE.orig"
#	mv $FILE  $FILE.orig
#fi

#echo -e "127.0.0.1 localhost \n\
#127.0.1.1    ubuntu \n\
#\n\
# The following lines are desirable for IPv6 capable hosts \n\
#::1 ip6-localhost ip6-loopback \n\
#fe00::0 ip6-localnet \n\
#ff00::0 ip6-mcastprefix \n\
#ff02::1 ip6-allnodes \n\
#`ff02::2 ip6-allrouters \n\
#ff02::3 ip6-allhosts" > $FILE


#FILE=/etc/resolv.conf
#if [ ! -f "$FILE.orig" ]; then
#	echo "mv  $FILE $FILE.orig"
#	mv $FILE  $FILE.orig
#fi

#sudo echo -e "nameserver 8.8.8.8" > $FILE

#/configure dns
./config_wifi_dns.sh


FILE=/etc/netplan/50-cloud-init.yaml
if [ ! -f "$FILE.orig" ]; then
	echo "mv  $FILE $FILE.orig"
	mv $FILE  $FILE.orig
fi

sudo echo -e "# This file is generated from information provided by the datasource.  Changes \n\
# to it will not persist across an instance reboot.  To disable cloud-init's \n\
# network configuration capabilities, write a file \n\
# /etc/cloud/cloud.cfg.d/99-disable-network-config.cfg with the following: \n\
# network: {config: disabled} \n\
network: \n\
    version: 2 \n\
    ethernets: \n\
        eth0: \n\
          dhcp4: true \n\
          optional: true \n\
    wifis: \n\
        wlan0: \n\
          addresses: [$ip_addr/24] \n\
#         gateway4: $ip_addr \n\
          nameservers: \n\
             addresses: [$ip_addr, 8.8.8.8] \n\
          optional: true \n\
          access-points: \n\
               \"$ssid\": \n\
                 password: \"$psk\" \n\
          dhcp4: true \n\
          dhcp6: false" > $FILE


#sudo iptables -F
#sudo iptables -t nat -F
#sudo iptables -t nat -A POSTROUTING -o $eth -j MASQUERADE
#sudo iptables -A FORWARD -i $eth -o $wlan -m state --state RELATED,ESTABLISHED -j ACCEPT
#sudo iptables -A FORWARD -i $wlan -o $eth -j ACCEPT

sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"

#sudo ifconfig $wlan $ip_address netmask $netmask
echo "will stop dmsmasq"
sudo systemctl stop dnsmasq
echo "stoped dmsmasq"

# Remove default route
sudo ip route del 0/0 dev $wlan &> /dev/null

sudo rm -rf /etc/dnsmasq.d/* &> /dev/null

echo -e "interface=$wlan \n\
bind-interfaces \n\
#bind-dynamic \n\
server=8.8.8.8 \n\
domain-needed \n\
bogus-priv \n\
dhcp-range=$dhcp_range_start,$dhcp_range_end,$dhcp_time" > /etc/dnsmasq.d/custom-dnsmasq.conf

echo "will start dmsmasq"
sudo systemctl start dnsmasq
sudo systemctl enable dnsmasq
echo "started dmsmasq"

sudo systemctl stop hostapd
echo -e "interface=$wlan\n\
#driver=nl80211\n\
ssid=$ssid\n\
hw_mode=g\n\
#ieee80211n=1\n\
wmm_enabled=1\n\
macaddr_acl=0\n\
ht_capab=[HT40][SHORT-GI-20][DSSS_CCK-40]\n\
channel=6\n\
auth_algs=1\n\
ignore_broadcast_ssid=0\n\
wpa=2\n\
wpa_key_mgmt=WPA-PSK\n\
wpa_passphrase=$psk\n\
rsn_pairwise=CCMP" > /etc/hostapd/hostapd.conf

sudo systemctl start hostapd
sudo systemctl enable hostapd

#sudo hostapd /etc/hostapd/hostapd.conf &

sudo reboot
