#!/bin/sh
#for i in $(seq 1 100); do
video_servers_started=0
echo $video_servers_started
while true; do
    echo "test wlan0 ip"
    ip_address=$(ip -o -4 addr show wlan0 | awk '{print $4}')
    echo " address = "  $(ip -o -4 addr show wlan0)
    #echo " address =  $ip_address"
    if [ -n "$ip_address" ]; then
       if [ $video_servers_started -eq 0 ]; then
       		 echo "wlan0 has an IP address: $ip_address"
		 video_servers_started=1
	         systemctl stop video_server_bcamera
	         systemctl start video_server_bcamera
	         systemctl stop video_server_fcamera
	         systemctl start video_server_bcamera
		 continue

	else
		sleep 10
		continue
        fi
    else
        echo "wlan0 does not have an IP address yet"
	video_servers_started=0
	./config_wlan0_ip.sh
	#(exec sudo netplan apply)
	#(exec sudo netplan apply)
	#(exec sudo netplan apply)
    fi
    
#	sleep 1  # Wait for 1 second before checking again
done

