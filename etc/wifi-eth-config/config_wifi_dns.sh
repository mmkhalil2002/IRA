
#!/bin/bash


FILE=/etc/hosts
if [ ! -f "$FILE.orig" ]; then
	echo "mv  $FILE $FILE.orig"
	mv $FILE  $FILE.orig
fi
echo -e "127.0.0.1 localhost \n\
127.0.1.1    ubuntu \n\
\n\
# The following lines are desirable for IPv6 capable hosts \n\
::1 ip6-localhost ip6-loopback \n\
fe00::0 ip6-localnet \n\
ff00::0 ip6-mcastprefix \n\
ff02::1 ip6-allnodes \n\
ff02::2 ip6-allrouters \n\
ff02::3 ip6-allhosts" > $FILE


sudo apt update
sudo apt install resolvconf
sudo systemctl start resolvconf.service
sudo systemctl enable resolvconf.service
FILE=/etc/resolvconf/resolv.conf.d/head
if [ ! -f "$FILE.orig" ]; then
	echo "cp  $FILE $FILE.orig"
	cp $FILE  $FILE.orig
sudo echo -e "nameserver 8.8.8.8 \n\
nameserver 8.8.8.4" >> $FILE
fi

FILE=/etc/resolv.conf
if [ ! -f "$FILE.orig" ]; then
	echo "mv  $FILE $FILE.orig"
	mv $FILE  $FILE.orig
fi

sudo echo -e "nameserver 8.8.8.8 \n\
nameserver 8.8.8.4" > $FILE
