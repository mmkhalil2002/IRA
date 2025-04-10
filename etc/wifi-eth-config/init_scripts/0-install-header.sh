if [ -d "/dev/src" ] && [ -z "$(ls -A /dev/src)" ]; then
	echo "/dev/src is empty"
	cd /dev/src
	sudo apt install linux-headers-$(uname -r)
	echo "/usr/src/linux-headers-$path/"
	sudo ln -s /usr/src/linux-headers-$path/arch/arm64 /usr/src/linux-headers-$path/arch/$(uname -m)
else
	echo "/dev/src is not empty"
fi
