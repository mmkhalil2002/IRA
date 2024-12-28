#!/bin/sh

#echo "What is the APN?"
#read carrierapn

#wget --no-check-certificate https://raw.githubusercontent.com/sixfab/Sixfab_RPi_3G-4G-LTE_Base_Shield/master/tutorials/QMI_tutorial/reconnect_service -O qmi_reconnect.service
#wget --no-check-certificate https://raw.githubusercontent.com/sixfab/Sixfab_RPi_3G-4G-LTE_Base_Shield/master/tutorials/QMI_tutorial/reconnect_sh -O qmi_reconnect.sh

#sed -i "s/#APN/$carrierapn/" qmi_reconnect.sh

systemctl stop qmi_reconnect.service
systemctl disable qmi_reconnect.service

cp qmi_reconnect.sh /usr/local/etc/smartrobot
cp qmi_reconnect.service /etc/systemd/system/

systemctl daemon-reload
systemctl start qmi_reconnect.service
systemctl enable qmi_reconnect.service

echo "DONE"
