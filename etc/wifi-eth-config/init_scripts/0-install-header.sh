if [ -d "/dev/src" ] && [ -z "$(ls -A /dev/src)" ]; then
    echo "/dev/src is empty"
    sudo apt install linux-headers-$(uname -r)
else
    echo "/dev/src is not empty"
fi
