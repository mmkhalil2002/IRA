#!/bin/bash


sudo systemctl daemon-reload
sudo systemctl stop     video_server_bcamera
sudo systemctl disable  video_server_bcamera
sudo systemctl stop     video_server_fcamera
sudo systemctl disable  video_server_fcamera
sudo systemctl stop     config_wlan0_1
sudo systemctl disable  config_wlan0_1
sudo systemctl stop     config_wlan0_2
sudo systemctl disable  config_wlan0_2
sudo systemctl stop     config_wlan0_3
sudo systemctl disable  config_wlan0_3

sudo systemctl enable   config_wlan0_1
sudo systemctl start    config_wlan0_1
sudo systemctl enable   config_wlan0_2
sudo systemctl start    config_wlan0_2
sudo systemctl enable   config_wlan0_3
sudo systemctl start    config_wlan0_3
sudo systemctl enable   video_server_bcamera
sudo systemctl start    video_server_bcamera
sudo systemctl enable   video_server_fcamera
sudo systemctl start    video_server_fcamera
