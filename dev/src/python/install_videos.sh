sudo mkdir -p /usr/local/robot/bin
sudo chmod 755 /usr/local/robot/bin
#sudo mkdir -p /media/usb
#sudo chmod 755 /media/usb
sudo systemctl daemon-reload
sudo cp -f video_server.py /usr/local/robot/bin/
sudo ln -sf /usr/local/robot/bin/video_server.py /usr/local/bin
sudo mkdir -p /usr/local/robot/etc/systemd/system
sudo chmod 755 /usr/local/robot/etc/systemd/system
sudo cp -f video_server_bcamera.service /usr/local/robot/etc/systemd/system
sudo cp -f video_server_bcamera.timer   /usr/local/robot/etc/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/video_server_bcamera.service /lib/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/video_server_bcamera.timer   /lib/systemd/system
sudo cp video_server_bcamera.sh /usr/local/robot/bin/video_server_bcamera.sh
sudo ln -sf /usr/local/robot/bin/video_server_bcamera.sh /usr/local/bin
sudo cp -f video_server_fcamera.service /usr/local/robot/etc/systemd/system
sudo cp -f video_server_fcamera.timer   /usr/local/robot/etc/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/video_server_fcamera.service /lib/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/video_server_fcamera.timer   /lib/systemd/system
sudo cp video_server_fcamera.sh /usr/local/robot/bin/video_server_fcamera.sh
sudo ln -sf /usr/local/robot/bin/video_server_fcamera.sh /usr/local/bin
sudo cp -f media-usb.mount /usr/local/robot/etc/systemd/system
sudo ln -sf  /usr/local/robot/etc/systemd/system/media-usb.mount /lib/systemd/system
systemctl stop video_server_fcamera.service
systemctl disable video_server_fcamera.service
systemctl stop video_server_bcamera.service
systemctl disable video_server_bcamera.service
systemctl enable video_server_fcamera.service
systemctl start video_server_fcamera.service
systemctl enable video_server_bcamera.service
systemctl start video_server_bcamera.service

