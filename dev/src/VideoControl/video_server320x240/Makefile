#!/usr/bin/make -f
#
# Filename:		Makefile
# Date:			05/07/2019
# Author:		Stephen Perkins
# Copyright:		2019 - SqwaQ, Inc.- All Rights Reserved
#
# Description:
#
#	Build a system that utilized RudeConfig to R/W
#	conf files
#


# Set up needed values to support make's implicit commands
# for compiling C++ source files.

# The compilers to use
CC = g++

# C pre-processor flags
CPPFLAGS = -I /usr/local/robot/include -I  /usr/local/include/opencv4/

# compiler flags
CFLAGS = -Wall

# linker flags
LDFLAGS = -L/usr/local/robot/lib -L/usr/local/lib/
#LDLIBS =  -lncurses -lrobot -lwiringPi -lpthread -lm
LDLIBS    =  -lpthread -lm -lcrypt -lrt
LDLIBS    += -lopencv_core
LDLIBS    += -lopencv_flann
LDLIBS    += -lopencv_imgproc
LDLIBS    += -lopencv_ml
LDLIBS    += -lopencv_dnn
LDLIBS    += -lopencv_features2d
LDLIBS    += -lopencv_videoio
LDLIBS    += -lopencv_calib3d
LDLIBS    += -lopencv_highgui
LDLIBS    += -lopencv_stitching
LDLIBS    += -lopencv_video
LDLIBS    += -lopencv_gapi


# Project name.  This is used in building the file name for the backup target
PROJECTNAME = VideoControl


# Enter the names of the C++ source files that you want to compile
# with this project

#SRCS = sq_config.c config_io.c
SRCS = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)

# Enter the name for the executable to be created
EXEC =  video_server


#
# You normally don't need to change anything below here.
# ======================================================
#
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(EXEC)

clean:
	rm -f $(OBJS) *.d $(EXEC) *~ .*~ TAGS

# Need to make sure clang-format installed via:
#    sudo apt-get install clang-format
#
pretty:
	clang-format -i $(SRCS) $(HEADERS)

#
# Emacs tags for finding function definitions
# etags is part of Emacs installation
tags:
	etags $(SRCS) $(HEADERS)

# Pattern for .d files.
%.d:%.cpp
	@echo Updating .d Dependency File
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


#  This is a rule to link the files.  Pretty standard
$(EXEC): $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LDFLAGS) $(LDLIBS)

Makefile: $(SRCS:.c=.d)


# Backup Target
backup:	clean
	@mkdir -p ~/backups; chmod 700 ~/backups
	@$(eval CURDIRNAME := $(shell basename `pwd`))
	@$(eval MKBKUPNAME := ~/backups/$(PROJECTNAME)-$(shell date +'%Y.%m.%d-%H:%M:%S').tar.gz)
	@echo
	@echo Writing Backup file to: $(MKBKUPNAME)
	@echo
	@-tar zcfv $(MKBKUPNAME) ../$(CURDIRNAME)
	@chmod 600 $(MKBKUPNAME)
	@echo
	@echo Done!



install: 
	sudo mkdir -p /usr/local/robot/bin
	sudo chmod 755 /usr/local/robot/bin
	sudo mkdir -p /media/usb
	sudo chmod 755 /media/usb
	sudo cp -f video_server /usr/local/robot/bin/ 
	sudo ln -sf /usr/local/robot/bin/video_server /usr/local/bin
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


uninstall:
	systemctl stop video_server_fcamera.service
	systemctl disable video_server_fcamera.service
	systemctl stop video_server_bcamera.service
	systemctl disable video_server_bcamera.service
	sudo rm -f  /usr/local/bin/video_server
	sudo rm -f  /usr/local/robot/etc/systemd/system/video_server_bcamera.service 
	sudo rm -f  /usr/local/robot/etc/systemd/system/video_server_bcamera.timer
	sudo rm -f  /usr/local/robot/etc/systemd/system/media-usb.mount 
	sudo rm -f  /lib/systemd/system/video_server_bcamera.service
	sudo rm -f  /lib/systemd/system/video_server_bcamera.timer
	sudo rm -f  /lib/systemd/system/media-usb.mount
	sudo rm -f  /usr/local/robot/bin/video_server_bcamera.sh
	sudo rm -f  /usr/local/bin/video_server_bcamera.sh
	sudo rm -f  /usr/local/robot/etc/systemd/system/video_server_fcamera.service 
	sudo rm -f  /usr/local/robot/etc/systemd/system/video_server_fcamera.timer
	sudo rm -f  /lib/systemd/system/video_server_fcamera.service
	sudo rm -f  /lib/systemd/system/video_server_fcamera.timer
	sudo rm -f  /usr/local/robot/bin/video_server_fcamera.sh
	sudo rm -f  /usr/local/bin/video_server_fcamera.sh
	make clean

# Include the dependency files created by the PreProcessor.  The
# dash in front of the command keeps the system from complaining
# if the files do not exist.

-include $(SRCS:.cpp=.d)

