s	installing ubuntu in raspberry pi
https://ubuntu.com/download/raspberry-pi

#security for git

https://stackoverflow.com/questions/35821245/github-server-certificate-verification-failed

git config --global http.sslverify false

#this porcedure doesn't work in 20.04
1-sudo apt install ntp
2-sudo echo "ip_adress" >> /etc/ntp
3-sed -i 's/pool /#pool /g' /etc/ntp.conf
4-sudo systemctl restart ntp.service
#stop autpmatic time sync
timedatectl set-ntp 0
#start autpmatic time sync
timedatectl set-ntp 1

#edit file

vi /etc/timezone

add  "America/Chicago"


the above steps are not needed

sudo apt install rfkill

5-sudo timedatectl set-timezone America/Chicago
6-sudo timedatectl set-time 2019-11-14
6-sudo timedatectl set-time 10:12:00

sudo apt-get update    #Update the list of packages in the software center
sudo apt-get install screen python-matplotlib  python3-pip python-numpy python-dev libxml2-dev libxslt-dev python-lxml -y

sudo apt-get install libxml2-dev libxslt1-dev -y
sudo pip3 install pymavlink
sudo pip3 install mavproxy
sudo pip3 install dronekit
sudo pip3 install dronekit-sitl


15-sudo adduser mkhalil
16-usermod -aG sudo mkhalil

17-git clone https://github.com/wiringpi/wiringpi

#18-git config --global http.sslVerify "false"
#19-sudo apt-get install wiringpi
20-cd wiringpi/
21-./build


22-#uninstall
23-sudo apt-get purge wiringpi
24-hash -r

#install vpn
mkdir hamashi-vpn 
cp deb file from https://www.vpn.net/linux
ARM HF version (BETA):
logmein-hamachi-2.1.0.203-1.armhf.rpm
logmein-hamachi_2.1.0.203-1_armhf.deb
logmein-hamachi-2.1.0.203-armhf.tgz

cd hamachi-vpn/
sudo dpkg -i logmein-hamachi_2.1.0.203-1_armhf.deb
sudo apt-get remove logmein-hamachi

#Log into your LogMeIn account and go online


#routing table should be like this, u should be able to ping 8.8.8.8
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.168.1.1     0.0.0.0         UG    100    0        0 eth0
25.0.0.0        0.0.0.0         255.0.0.0       U     0      0        0 ham0
25.94.116.160   0.0.0.0         255.255.255.224 U     0      0        0 usb0
192.168.1.0     0.0.0.0         255.255.255.0   U     0      0        0 eth0
192.168.1.1     0.0.0.0         255.255.255.255 UH    100    0        0 eth0
192.168.44.0    0.0.0.0         255.255.255.0   U     0      0        0 wlan0


sudo  systemctl status logmein-hamachi.service -l
sudo  systemctl start logmein-hamachi.service
sudo /etc/init.d/logmein-hamachi start
	sudo hamachi login

#log file

vi /var/lib/logmein-hamachi/h2-engine.log


#Attach the client to your LogMeIn account using your LogMeIn ID (email address)

#sudo hamachi attach [email@example.com] will send a requst to logmein to approve
#Sending attach request to mmkhalil2001@yahoo.com without networks .. ok

sudo hamachi attach mmkhalil2001@yahoo.com

#List available networks in your account and your peers.
#Networks, where your client is online, are marked with an asterisk (*).
#For online peers additional information is displayed, such as IP address.

sudo hamachi list
# Display network information
#sudo hamachi network mohamedkhalilnetwork

sudo hamachi join <nettwork id> M11KhM11Oh
sudo hamachi join 414-044-990 M11KhM11Oh
414-044-990 is the netork id for mohamedkhalilnetwork

in hamachi login managment site set the "network setting" to must be approved. when u join u will find a request for approval or rejection. 
click on apporave

#remove hamachi
sudo apt-get purge logmein-hamachi 
sudo apt-get remove logmein-hamachi


#install FFMPEG

https://www.hackster.io/whitebank/rasbperry-pi-ffmpeg-install-and-stream-to-web-389c34
https://linuxize.com/post/how-to-install-ffmpeg-on-ubuntu-18-04/
https://www.raspberrypi.org/forums/viewtopic.php?t=212417
https://help.ubuntu.com/community/FFmpeg
https://trac.ffmpeg.org/wiki/StreamingGuide
https://ffmpeg.org/ffmpeg-formats.html

sudo apt install -y libopus-dev libmp3lame-dev libfdk-aac-dev libvpx-dev libx264-dev yasm libass-dev libtheora-dev libvorbis-dev mercurial cmake
sudo apt install libx265-dev

# install in ubuntu 18.04 video4linux2

sudo apt-get install -y v4l-utilssudo


sudo apt update
sudo add-apt-repository ppa:jonathonf/ffmpeg-4
sudo apt install ffmpeg

vi /etc/ffserver.conf

Port 8090
 # bind to all IPs aliased or not
 BindAddress 0.0.0.0
 # max number of simultaneous clients
 MaxClients 10
 # max bandwidth per-client (kb/s)
 MaxBandwidth 1000
 # Suppress that if you want to launch ffserver as a daemon.
 NoDaemon

<Feed feed1.ffm>
 File /tmp/feed1.ffm
 FileMaxSize 10M
 </Feed>

vi /usr/sbin/webcam.sh

ffserver -f /etc/ffserver.conf & ffmpeg -v verbose -r 5 -s 600x480 -f video4linux2 -i /dev/video0 http://localhost:8090/feed1.ffm
<exit>
chmod +x /usr/sbin/webcam.sh
/usr/sbin/webcam.sh
#Go to web browser, type address <your-ip-address:8090>/test.mjpg



#commands

ffmpeg -encoders
ffmpeg -decoders
ffmpeg -version

#output
ffmpeg version 4.2.1-0york0~18.04.1 Copyright (c) 2000-2019 the FFmpeg developers
built with gcc 7 (Ubuntu/Linaro 7.4.0-1ubuntu1~18.04.1)
configuration: --prefix=/usr --extra-version='0york0~18.04.1' --toolchain=hardened --libdir=/usr/lib/aarch64-linux-gnu --incdir=/usr/include/aarch64-linux-gnu --arch=arm64 --enable-gpl --disable-stripping --enable-avresample --disable-filter=resample --enable-avisynth --enable-gnutls --enable-ladspa --enable-libaom --enable-libass --enable-libbluray --enable-libbs2b --enable-libcaca --enable-libcdio --enable-libcodec2 --enable-libflite --enable-libfontconfig --enable-libfreetype --enable-libfribidi --enable-libgme --enable-libgsm --enable-libjack --enable-libmp3lame --enable-libmysofa --enable-libopenjpeg --enable-libopenmpt --enable-libopus --enable-libpulse --enable-librsvg --enable-librubberband --enable-libshine --enable-libsnappy --enable-libsoxr --enable-libspeex --enable-libssh --enable-libtheora --enable-libtwolame --enable-libvidstab --enable-libvorbis --enable-libvpx --enable-libwavpack --enable-libwebp --enable-libx265 --enable-libxml2 --enable-libxvid --enable-libzmq --enable-libzvbi --enable-lv2 --enable-omx --enable-openal --enable-opengl --enable-sdl2 --enable-nonfree --enable-libfdk-aac --enable-libdc1394 --enable-libdrm --enable-libiec61883 --enable-chromaprint --enable-frei0r --enable-libx264 --enable-shared
libavutil      56. 31.100 / 56. 31.100
libavcodec     58. 54.100 / 58. 54.100
libavformat    58. 29.100 / 58. 29.100
libavdevice    58.  8.100 / 58.  8.100
libavfilter     7. 57.100 /  7. 57.100
libavresample   4.  0.  0 /  4.  0.  0
libswscale      5.  5.100 /  5.  5.100
libswresample   3.  5.100 /  3.  5.100
libpostproc    55.  5.100 / 55.  5.100


#configuring ubuntu as a router

vi /etc/sysctl.conf

# Uncomment the next line to enable packet forwarding for IPv4
net.ipv4.ip_forward=1

# Uncomment the next line to enable packet forwarding for IPv6
#  Enabling this option disables Stateless Address Autoconfiguration
#  based on Router Advertisements for this host
net.ipv6.conf.all.forwarding=1

https://www.diyhobi.com/install-wifi-hotspot-raspberry-pi-ubuntu-mate/
https://www.youtube.com/watch?v=j3wsYskgdAs
https://linoxide.com/linux-how-to/install-configure-dhcp-ubuntu/
https://www.ascinc.com/blog/linux/how-to-build-a-simple-router-with-ubuntu-server-18-04-1-lts-bionic-beaver/

#installing DHCP in ubuntu 18.04
#sudo systemctl restart isc-dhcp-server


mv /dev/random /dev/random.orig
ln -s /dev/urandom /dev/random

vi /etc/netplan/50-cloud-init.yaml

# This file is generated from information provided by
# the datasource.  Changes to it will not persist across an instance.
# To disable cloud-init's network configuration capabilities, write a file
# /etc/cloud/cloud.cfg.d/99-disable-network-config.cfg with the following:
# network: {config: disabled}
network:
    version: 2
    ethernets:
        eth0:
            dhcp4: true
#            match:
#                macaddress: b8:27:eb:d3:b8:84
            set-name: eth0
    wifis:
        wlan0:
          addresses: [192.168.42.20/24]
#          gateway4: 192.168.42.1
          nameservers:
             addresses: [192.168.42.1, 8.8.8.8]
          optional: true
          access-points:
                "dronenet":
                    password: "1234567890"
          dhcp4: true
          dhcp6: false
<exit>
netplan apply
apt-get install rng-tools
vi /etc/default/rng-tools
add ... and add the line HRNGDEVICE=/dev/urandom:
/etc/init.d/rng-tools start

#check https://www.diyhobi.com/install-wifi-hotspot-raspberry-pi-ubuntu-mate/


sudo apt-get install hostapd
sudo systemctl unmask hostapd
sudo systemctl enable hostapd

vi /etc/default/hostapd
add following line
DAEMON_CONF="/etc/hostapd/hostapd.conf"
<exit>

sudo vi /etc/hostapd/hostapd.conf

interface=wlan0
#bridge=br0
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
ssid=dronenetapi4-1-1
wpa_passphrase=raspberry
<exit>

excute eth0-to-wifi.sh

#sudo systemctl start hostapd

#sudo apt-get install isc-dhcp-server
#sudo service isc-dhcp-server stop
#vi /etc/default/isc-dhcp-server
#add  line to file  
#     INTERFACES="wlan0"

#sudo mv /etc/dhcp/dhcpd.conf /etc/dhcp/dhcpd.bak
#sudo vi /etc/dhcp/dhcpd.conf



#add lines
#default-lease-time 600;
#max-lease-time 7200;
#option subnet-mask 255.255.255.0;
#option broadcast-address 192.168.43.255;
#option routers 192.168.43.1;
#option domain-name-servers 192.168.43.1,8.8.8.8;
#option domain-name "diyhobi.com";
#subnet 192.168.43.0 netmask 255.255.255.0 {
#range 192.168.43.10 192.168.43.150;
#}


#sudo vi /etc/default/isc-dhcp-server
#add the following line
#INTERFACES="wlan0"

#sudo service isc-dhcp-server start
#sudo service isc-dhcp-server enable

#Activating the IPv4 forwaring
sudo nano /etc/sysctl.conf
#Un-comment this line by removing #
net.ipv4.ip_forward=1
sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"
root@ubuntu:~# sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
root@ubuntu:~# sudo iptables -A FORWARD -i eth0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
root@ubuntu:~# sudo iptables -A FORWARD -i wlan0 -o eth0 -j ACCEPT
root@ubuntu:~# sudo iptables -L -n -v
Chain INPUT (policy ACCEPT 304 packets, 17023 bytes)
 pkts bytes target     prot opt in     out     source               destination

Chain FORWARD (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination
    0     0 ACCEPT     all  --  eth0   wlan0   0.0.0.0/0            0.0.0.0/0            state RELATED,ESTABLISHED
    0     0 ACCEPT     all  --  wlan0  eth0    0.0.0.0/0            0.0.0.0/0

Chain OUTPUT (policy ACCEPT 703 packets, 69464 bytes)
 pkts bytes target     prot opt in     out     source               destination
root@ubuntu:~# sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"
root@ubuntu:~# sudo vi /etc/rc.local
#add the following code
iptables-restore < /etc/iptables.ipv4.nat
#exit

# list the properties of wifi interface
nmcli -p -f general,wifi-properties device show wlan0
#get gateway ip address
ip r | grep default
#list all wifi networks
iwlist  wlan0 s
   42  nmcli d wifi list
   43  nmcli d
   44  nmcli c add type wifi con-name drone_net ifname wlan0 ssid dronenet
   45  nmcli d wifi list
   46  nmcli c delete drone_net
   47  nmcli d wifi list
   48  nmcli c add type wifi con-name drone_net ifname wlan0 ssid dronenet
   51  nmcli c modify drone_net wifi-sec.key-mgmt wpa-psk wifi-sec.psk 1234567890
   53  nmcli dev wifi connect dronenet password 123567890 hidden yes
   54  nmcli d wifi list
   55  nmcli d

#disable wlan0 wifi
sudo modprobe nl80211
#disable wlan0 is bugy
Alternatively, you could use 
vi /etc/modprobe.d/blacklist.conf

 and adding:
#disable wlan0
blacklist brcmfmac
blacklist brcmutil



#building hostapd
You can get the latest development version of hostapd from the git repository with:
 sudo apt-get update -y
 sudo apt-get install -y libnl-genl-3-dev
sudo apt install openssl libssl-dev


git clone git://w1.fi/srv/git/hostap.git
cd hostap/hostapd
Or you can get a stable release (0.6.8 or later recommended) by downloading the tarball from http://w1.fi/hostapd/ as follows:
sudo apt-get update -y
sudo apt-get inst
cd hostapd-x.y.z/hostapd
Next, we need to configure the hostapd build to enable nl80211 driver support. Copy defconfig to .config, and open it in your preferred text editor. Also, there are other options that you may want to enable, like 802.11n support if your hardware can do it. Most of the other encryption types and features aren't needed for most applications, so if you're questioning if you need to enable it, you probably don't need to.
cd ~/hostap/hostapd
cp defconfig .config
vi .config
Now find this line:

#CONFIG_DRIVER_NL80211=y
and uncomment it by removing the '#' sign. Repeat for other settings that you may be interested in. The basic configuration, with only this line uncommented, is enough to get hostapd up and running with WPA/WPA2 authentication and encryption.

Next, compile hostapd:

make
make install
sudo apt update && sudo apt upgrade -y
 sudo apt install -y dnsmasq


# remove dhcpclient if there is 2 
root@ubuntu:~# dpkg -l | grep dhcp
root@ubuntu:~#  dpkg -l | grep dhcp
ii  dhcpcd5                              6.11.5-0ubuntu1                     arm64        DHCPv4, IPv6RA and DHCPv6 client with IPv4LL support
ii  isc-dhcp-client                      4.3.5-3ubuntu7.1                    arm64        DHCP client for automatically obtaining an IP address
ii  isc-dhcp-common                      4.3.5-3ubuntu7.1                    arm64        common manpages relevant to all of the isc-dhcp packages
s
root@ubuntu:~#
apt-get remove dhcpcd5
root@ubuntu:~# dpkg -l | grep dhcp
rc  dhcpcd5                              6.11.5-0ubuntu1                     armhf        DHCPv4, IPv6RA and DHCPv6 client with IPv4LL support
ii  isc-dhcp-client                      4.3.5-3ubuntu7.1                    armhf        DHCP client for automatically obtaining an IP address
ii  isc-dhcp-common                      4.3.5-3ubuntu7.1                    armhf        common manpages relevant to all of the isc-dhcp packages

apt autoremove  # remove unused packages

sudo systemctl status dhcpcd.service

root@ubuntu:~# sudo systemctl status dhcpcd.service
● dhcpcd.service - LSB: IPv4 DHCP client with IPv4LL support
   Loaded: loaded (/etc/init.d/dhcpcd; generated)
   Active: active (exited) since Mon 2020-04-06 21:50:57 CDT; 42min ago
     Docs: man:systemd-sysv-generator(8)
    Tasks: 0 (limit: 2150)
   CGroup: /system.slice/dhcpcd.service

Apr 06 21:50:57 ubuntu systemd[1]: Starting LSB: IPv4 DHCP client with IPv4LL support...
Apr 06 21:50:57 ubuntu systemd[1]: Started LSB: IPv4 DHCP client with IPv4LL support.


 vi /etc/netplan/50-cloud-init.yaml

# This file is generated from information provided by
# the datasource.  Changes to it will not persist across an instance.
# To disable cloud-init's network configuration capabilities, write a file
# /etc/cloud/cloud.cfg.d/99-disable-network-config.cfg with the following:
# network: {config: disabled}
network:
    version: 2
    ethernets:
        eth0:
            dhcp4: true
#            match:
#                macaddress: b8:27:eb:d3:b8:84
            set-name: eth0
    wifis:
        wlan0:
          addresses: [192.168.42.20/24]
#          gateway4: 192.168.42.1
          nameservers:
             addresses: [192.168.42.1, 8.8.8.8]
          optional: true
          access-points:
                "dronenet":
                    password: "1234567890"
          dhcp4: true
          dhcp6: false



root@ubuntu:~# systemctl status dnsmasq
● dnsmasq.service - dnsmasq - A lightweight DHCP and caching DNS server
   Loaded: loaded (/lib/systemd/system/dnsmasq.service; enabled; vendor preset: enabled)
   Active: active (running) since Fri 2020-04-10 19:56:35 UTC; 7s ago
  Process: 8298 ExecStop=/etc/init.d/dnsmasq systemd-stop-resolvconf (code=exited, status=0/SUCCESS)
  Process: 8452 ExecStartPost=/etc/init.d/dnsmasq systemd-start-resolvconf (code=exited, status=0/SUCCESS)
  Process: 8443 ExecStart=/etc/init.d/dnsmasq systemd-exec (code=exited, status=0/SUCCESS)
  Process: 8442 ExecStartPre=/usr/sbin/dnsmasq --test (code=exited, status=0/SUCCESS)
 Main PID: 8451 (dnsmasq)
    Tasks: 1 (limit: 4441)
   CGroup: /system.slice/dnsmasq.service
           └─8451 /usr/sbin/dnsmasq -x /run/dnsmasq/dnsmasq.pid -u dnsmasq -7 /etc/dnsmasq.d,.dpkg-dist,.dpkg-old,.dpkg-new --local-service --trust-anchor=.,19036,8,2,

Apr 10 19:56:35 ubuntu dnsmasq[8451]: started, version 2.79 cachesize 150
Apr 10 19:56:35 ubuntu dnsmasq[8451]: compile time options: IPv6 GNU-getopt DBus i18n IDN DHCP DHCPv6 no-Lua TFTP conntrack ipset auth DNSSEC loop-detect inotify
Apr 10 19:56:35 ubuntu dnsmasq-dhcp[8451]: DHCP, IP range 192.168.43.2 -- 192.168.43.200, lease time 12h
Apr 10 19:56:35 ubuntu dnsmasq-dhcp[8451]: DHCP, sockets bound exclusively to interface wlan0
Apr 10 19:56:35 ubuntu dnsmasq[8451]: using nameserver 8.8.8.8#53
Apr 10 19:56:35 ubuntu dnsmasq[8451]: reading /etc/resolv.conf
Apr 10 19:56:35 ubuntu dnsmasq[8451]: using nameserver 8.8.8.8#53
Apr 10 19:56:35 ubuntu dnsmasq[8451]: using nameserver 127.0.0.53#53
Apr 10 19:56:35 ubuntu dnsmasq[8451]: read /etc/hosts - 8 addresses
Apr 10 19:56:35 ubuntu systemd[1]: Started dnsmasq - A lightweight DHCP and caching DNS server.


#installing drivers
sudo apt install xz-utils
sudo apt install unzip
mkdir Quectel-devise-drivers
cd Quectel-devise-drivers
copy the contents of this link to Quectel-devise-drivers
unzip 'Quectel_Linux&Android_GobiNet_Driver_V1.6.zip'
mkdir ~/Quectel-devise-drivers/Quectel_Linux
cp ~/Quectel-devise-drivers/'Quectel_Linux&Android_GobiNet_Driver_V1.6'/*  ~/Quectel-devise-drivers/Quectel_Linux

$ sudo apt-get update
$ sudo apt-get install linux-source
$ sudo apt-get install build-essential
sudo apt-get install libncurses5-dev libncursesw5-dev
$ cd /usr/src
$tar -xf linux-source-4.15.0.tar.bz2

# for 32 bit
 sudo ln -s /usr/src/linux-headers-5.3.0-1017-raspi2/arch/arm /usr/src/linux-headers-5.3.0-1017-raspi2/arch/armv7l
#for 64 bits
sudo ln -s /usr/src/linux-headers-4.15.0-1060-raspi2/arch/arm64/ /usr/src/linux-headers-4.15.0-1060-raspi2/arch/aarch64

cd Quectel_Linux
make
make install


#setting GPS
minicom -D /dev/ttyUSB2
AT+QGPS=1	Starts GPS service + NMEA Feed on /dev/modemGPS
AT+QGPSCFG="gpsnmeatype",31	Configure which NMEA output sentences are generated (See SIMCOM GPS Page)
reboot module
minicom -D /dev/ttyUSB2
AT+QGPS=1



installing OpenCV in abuntu
#instructions
https://emotionrobots.com/2015/06/12/installing-opencv-on-ubuntu-and-running-video-capture-from-camera-scott/


git clone https://github.com/Itseez/opencv.git
or
git clone https://github.com/opencv/opencv.git
cd opencv && mkdir Release && cd Release 
cmake -D CMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/usr/local ..
make
make install

#ArduCam
mkdir CameraMotion
cd cameraMotion
git config --global http.sslverify false
git clone https://github.com/ArduCAM/PCA9685.git
cd PCA9685
make


install opencv in window

https://towardsdatascience.com/install-and-configure-opencv-4-2-0-in-windows-10-vc-d132c52063a1
https://www.deciphertechnic.com/install-opencv-with-visual-studio/

---------------------   MOUNT DEVICE at BOOT-----------------------
# I used the following procedure to mount /dev/sda1
the file used is /VideoControl/media-usb.mount
# this mechanism use systemd
# https://forum.manjaro.org/t/root-tip-use-systemd-to-mount-any-device/1185
----------------------------------------------------------------
comand argument

Solution Explorer (right hand side) click right button -> Properties->Command Arguments

#mount storage in ubuntu 

ls -al /dev/disk/by-uuid/
total 0
drwxr-xr-x 2 root root 100 Jan  1  1970 .
drwxr-xr-x 7 root root 140 Jan  1  1970 ..
lrwxrwxrwx 1 root root  15 Jul 21 19:00 09CD-6B98 -> ../../mmcblk0p1
lrwxrwxrwx 1 root root  10 Jul 21 19:00 5365-165F -> ../../sda1
lrwxrwxrwx 1 root root  15 Aug  2 21:27 e25d9784-3410-48b0-9928-acf20eecb840 -> ../../mmcblk0p2

or

sudo blkid

mkdir /media/usb

# fstab doesn't recognize 2T devices I have put a mount in /home/ubuntu/VideoControl/video_server320x240# in video_server_bcamera.sh
# and video_server_fcamera.sh as follows


-----------------  OBSLETE Porceudre ----------------
#/bin/bash

mount /dev/sda1 /media/usb
video_server  -v 2 -p 14095 -c 14098

------------------------  END --------------------------

add this line to /etc/fstab file

/dev/disk/by-uuid/5365-165F   /media/usb         vfat   0   0

or

/dev/sda1       /media/usb          vfat    defaults        0       0 

final result

LABEL=writable  /        ext4   defaults        0 0
LABEL=system-boot       /boot/firmware  vfat    defaults        0       1
/dev/disk/by-uuid/5365-165F     /media/usb      vfat    defaults 0      0


#changing the name of interface 

vi /etc/udev/rules.d/70-persistent-net.rules

SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="1c:bf:ce:13:07:85", ATTR{dev_id}=="0x0", ATTR{type}=="1", NAME="wlan0"
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="2e:f4:4a:7e:5c:96",ATTR{dev_id}=="0x0", ATTR{type}=="1", NAME="usb0"

replace the MAC address with the MAC address of wifi and LTE interface
#  u can find the MAC address

>>ip addr

u can change the name temprorary by

>> ip link set wlx1cbfce130785 name wlan0


#openvpn 

openvpn --config mkhalil.ovpn --auth-retry interact --log /var/log/openvpn.log
netstat -anp | grep openvpn
netstat -anu | grep 1194
995  tcpdump -eni any
  996  tcpdump -eni any port 1194
  997  tcpdump -eni any host 10.8.0.2
  998  tcpdump -eni any src 10.8.0.2
  999  ifconfig -a
 1000  tcpdump -eni tun0
 1001  tcpdump -eni any
 1002  tcpdump -eni any host 10.8.0.2
 1003  tcpdump -eni any host 8.8.8.8
 1004  tcpdump -eni any dst 8.8.8.8
 1005  tcpdump -eni any dst 10.8.0.1
 1006  tcpdump -eni any dst 8.8.8.8
 1007  tcpdump -eni any icmp
 1008  pwd
 1009  tcpdump -eni any icmp
 1010  tcpdump -eni eth0
 1011  tcpdump -eni eth0 -w wireshark.log

#flush arp
ip -s -s neigh flush all
#discover all hosts in sub
nmap -sP 192.168.1.0/24


cd /lib/systemd/system

root@ubuntu:/lib/systemd/system# ls -al open*
-rw-r--r-- 1 root root 987 Jan 19  2021 open-iscsi.service
-rw-r--r-- 1 root root 688 Jul 19 14:26 openvpn-client@.service
-rw-r--r-- 1 root root 810 Jul 19 14:26 openvpn-server@.service
-rw-r--r-- 1 root root 299 Jul 19 14:26 openvpn.service
-rw-r--r-- 1 root root 946 Jul 19 14:26 openvpn@.service


1351  systemctl status  openvpn@server.service
1352  systemctl restart  openvpn@server.service
1351  systemctl enable  openvpn@server.service
1352  systemctl start openvpn@server.service

openvpn --config /etc/openvpn/client.conf
systemctl status  openvpn@client.service
systemctl restart  openvpn@client.service
systemctl enable  openvpn@client.service
systemctl start openvpn@client.service

# show MAC addr
ip addr show

#system parameters
vi /etc/sysctl.conf
# update system
 sysctl --system

ip route get 8.8.8.8
root@ubuntu:/home/ubuntu# ip route get 8.8.8.8
8.8.8.8 via 192.168.1.1 dev eth0 src 192.168.1.116 uid 0
    cache

ip route get 192.168.1.130
192.168.1.130 dev eth0 src 192.168.1.116 uid 0
    cache

#to get connectivity from 116 to 130 add in 116 the following
route add -net 192.168.1.130 netmask 255.255.255.255 gw 192.168.1.1

#arp request/reply
tcpdump -i eth99 arp and arp[6:2] == 2
#For broadcasts with opcode "reply", which should be just the gratuitous ARPs:
tcpdump -i eth99 broadcast and arp and arp[6:2] == 2

#sending gratituious arp
arping -A -i eth0 10.0.0.1

# list all open ports in a machine
netstate -tulpn

# scan all hosts in subnet with icmp
nmap -sP 192.168.1.0/24



                
----------------   ubuntu 24.04 --------------------
add the following to /etc/ssh/sshd_config

Port 22
# To disable tunneled clear text passwords, change to no here!
PasswordAuthentication yes
PermitEmptyPasswords no
PubkeyAuthentication  no
PermitRootLogin  yes
# Change to yes to enable challenge-response passwords (beware issues with
# some PAM modules and threads)
KbdInteractiveAuthentication no

then execute


vi /etc/ssh/sshd_config.d/50-cloud-init.conf

change to  

PasswordAuthentication yes

systemctl restart ssh

