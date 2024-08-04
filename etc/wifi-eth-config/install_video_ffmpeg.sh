#!/bin/sh

#echo "What is the APN?"
#read carrierapn

#wget --no-check-certificate https://raw.githubusercontent.com/sixfab/Sixfab_RPi_3G-4G-LTE_Base_Shield/master/tutorials/QMI_tutorial/reconnect_service -O qmi_reconnect.service
#wget --no-check-certificate https://raw.githubusercontent.com/sixfab/Sixfab_RPi_3G-4G-LTE_Base_Shield/master/tutorials/QMI_tutorial/reconnect_sh -O qmi_reconnect.sh

#sed -i "s/#APN/$carrierapn/" qmi_reconnect.sh
echo "start video service"
if sudo systemctl list-timers | grep video.timer; then
  	echo "terminate video.service"
	systemctl stop video.timer
	systemctl stop video.service
	systemctl disable video.timer
else
  	echo "video.service doesn't exist"

fi
cp start_video_ffmpeg.sh /usr/local/etc/smartrobot
cp video.service /etc/systemd/system/
cp video.timer /etc/systemd/system/

systemctl daemon-reload
systemctl start video.timer
systemctl enable video.timer

echo "DONE"
