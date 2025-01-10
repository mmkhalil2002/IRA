sudo echo -e "#disable wlan0 \n\
blacklist brcmfmac \n\
blacklist brcmutil" >> /etc/modprobe.d/blacklist.conf
