#!/bin/sh

#echo "What is the APN?"
#read carrierapn

#wget --no-check-certificate https://raw.githubusercontent.com/sixfab/Sixfab_RPi_3G-4G-LTE_Base_Shield/master/tutorials/QMI_tutorial/reconnect_service -O qmi_reconnect.service
#wget --no-check-certificate https://raw.githubusercontent.com/sixfab/Sixfab_RPi_3G-4G-LTE_Base_Shield/master/tutorials/QMI_tutorial/reconnect_sh -O qmi_reconnect.sh

#sed -i "s/#APN/$carrierapn/" qmi_reconnect.sh

if sudo service --status-all  | grep start_drone; then
	echo "remove  start_drone"
	systemctl disable  start_drone
	systemctl stop start_drone
fi

sudo cp  start_drone /etc/init.d/start_drone
cd /etc/init.d/
sudo chown root:root start_drone
sudo chmod 755 start_drone


systemctl daemon-reload
systemctl start start_drone
systemctl enable start_drone

echo "DONE"
