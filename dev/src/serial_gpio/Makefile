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
#C = arm-linux-gnueabihf-g++
CC = g++

# architecture to use
# arm
# hostx86_64 

#CPU = hostx86_64
CPU = arm

# To support shared development, install the Sierra Wireless
# Development Kit into /usr/local.  To do this:
#   * sudo -s
#   * mkdirs -p /usr/local/SierraWirelesss/SLQS04.00.18
#   * cd /usr/local/SierraWirelesss/SLQS04.00.18
#   * Copy the SierraWireless tarball to this folder (SLQS04.00.18.bin.tar.gz)
#   * tar zxfv ./SLQS04.00.18.bin.tar.gz
# Needed for SqwaQ specific libraries and headers

CONFDIR= /home/mkhalil/

# Combine up a bunch of needed Paths for the preprocessor

INCLUDES =  \
	   -I $(CONFDIR)/MAVLINK/v2.0/ \



# C pre-processor flags
CPPFLAGS = $(INCLUDES)

# compiler flags
CFLAGS = -Wall

# linker flags
LDFLAGS = 
         
CONFLAGS=  
LDLIBS =  -lwiringPi -lpthread -lcrypt -lm -lrt

# Project name.  This is used in building the file name for the backup target
PROJECTNAME = serial_gpio 


# Enter the names of the C++ source files that you want to compile
# with this project

#SRCS = sq_config.c config_io.c
SRCS = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

# Enter the name for the executable to be created
EXEC =  serial_gpio 


#
# You normally don't need to change anything below here.
# ======================================================
#
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(EXEC)

devinstall: 

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

Makefile: $(SRCS:.cpp=.d)


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


# Include the dependency files created by the PreProcessor.  The
# dash in front of the command keeps the system from complaining
# if the files do not exist.

-include $(SRCS:.cpp=.d)

