apt --fix-broken install
sudo apt-get install motion -y

#Edit the configuration file located in /etc/default/motion
#start_motion_daemon=yes
#Then edit the /etc/motion/motion.conf file file as follows:
#start_motion_daemon=yes

#Set width and height appropriately (640×480 in my case).
#Adjust framerate if you want.
#I set minimum_motion_frames to 2 because I have a higher framerate.


#webcontrol_localhost off
#stream_localhost off
#quality 75
#post_capture 5
#brightness 20
#framerate 100
#width 640
#height 480

FILE=/etc/default/motion
if [ ! -f "$FILE.orig" ]; then
        echo "cp  $FILE $FILE.orig"
        cp $FILE  $FILE.orig
fi
sed -i 's/start_motion_daemon=no/start_motion_daemon=yes/g' $FILE


FILE=/etc/motion/motion.conf
if [ ! -f "$FILE.orig" ]; then
        echo "cp  $FILE $FILE.orig"
        cp $FILE  $FILE.orig
fi

sed -i 's/webcontrol_localhost on/webcontrol_localhost off/g' $FILE
sed -i 's/stream_localhost on/stream_localhost off/g' $FILE
sed -i 's/post_capture 0/post_capture 5/g'  $FILE
sed -i 's/brightness 20/brightness 20/g' $FILE
sed -i 's/framerate 15/brightness 100/g' $FILE
sed -i 's/width 320/width 640/g' $FILE
sed -i 's/height 240/height 480/g' $FILE


sudo systemctl start   motion
sudo systemctl enable   motion






# installing release 4.3.1 is not tested 
#sudo apt-get install gdebi-core -y
#sudo apt-get install libavcodec58 -y
#sudo mkdir motion
#sudo cd motion
#sudo wget https://github.com/Motion-Project/motion/releases/download/release-4.3.1/pi_buster_motion_4.3.1-1_armhf.deb
#sudo gdebi pi_buster_motion_4.3.2-1_armhf.deb



