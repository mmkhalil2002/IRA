#!/bin/sh

: '
QMI installation script by Sixfab

Created By Metin Koc, Nov 2018
Modified by Saeed Johar, 11th June 2019
'
sudo apt-get update    #Update the list of packages in the software center
apt install make -y
apt install make-guile -y
cd  Quectel-CM-4G5G
cd src
make 
cp -f ./quectel-CM /usr/local/bin

echo Installing udhcpc$

apt-get install udhcpc -y

echo Copying udhcpc default script
mkdir -p /usr/share/udhcpc
cp default.script /usr/share/udhcpc/
chmod +x /usr/share/udhcpc/default.script 

cd ..
#sudo rm -r quectel-CM
#sudo rm -r quectel-CM.zip
