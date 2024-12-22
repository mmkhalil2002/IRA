#!/bin/sh

: '
QMI installation script by Sixfab

Created By Metin Koc, Nov 2018
Modified by Saeed Johar, 11th June 2019
'
sudo apt-get update    #Update the list of packages in the software center
apt install make -y
apt install make-guile -y
echo "${YELLOW}Downloading source files${SET}"
wget https://github.com/sixfab/Sixfab_RPi_3G-4G-LTE_Base_Shield/raw/master/tutorials/QMI_tutorial/src/quectel-CM.zip
unzip -fx quectel-CM.zip
#unzip quectel-CM.zip -d .  && rm -r quectel-CM.zip
unzip quectel-CM.zip -d .
cd  quectel-CM
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
