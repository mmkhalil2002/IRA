#!/bin/sh

for i in 1 2 3 4; do
    echo "start "
    ip_address=$(ip -o -4 addr show wlan0 | awk '{print $4}')
    echo " address = " $i   $(ip -o -4 addr show wlan0)
    if [ -n "$ip_address" ]; then
        echo "wlan0 has an IP address: $ip_address"
        break
    else
        echo "wlan0 does not have an IP address yet"
	sudo netplan apply
    fi
    
#	sleep 1  # Wait for 1 second before checking again
done

