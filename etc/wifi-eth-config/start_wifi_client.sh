#!/bin/bash


sudo systemctl daemon-reload
sudo systemctl stop     video_server_bcamera
sudo systemctl disable  video_server_bcamera
sudo systemctl stop     video_server_fcamera
sudo systemctl disable  video_server_fcamera
sudo systemctl stop     config_wlan0
sudo systemctl disable  config_wlan0

sudo systemctl enable   config_wlan0
sudo systemctl start    config_wlan0
sudo systemctl enable   video_server_bcamera
sudo systemctl start    video_server_bcamera
sudo systemctl enable   video_server_fcamera
sudo systemctl start    video_server_fcamera
