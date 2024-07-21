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
CC = gcc

# architecture to use
# arm
# hostx86_64 

CPU = hostx86_64
#CPU = arm

# To support shared development, install the Sierra Wireless
# Development Kit into /usr/local.  To do this:
#   * sudo -s
#   * mkdirs -p /usr/local/SierraWirelesss/SLQS04.00.18
#   * cd /usr/local/SierraWirelesss/SLQS04.00.18
#   * Copy the SierraWireless tarball to this folder (SLQS04.00.18.bin.tar.gz)
#   * tar zxfv ./SLQS04.00.18.bin.tar.gz
IDIR = /usr/local/SierraWireless/SLQS04.00.18
# Needed for SqwaQ specific libraries and headers

CONFDIR= /usr/local/sqwaq

# Combine up a bunch of needed Paths for the preprocessor

INCLUDES = -I ./include \
	   -I $(IDIR)/ \
	   -I $(CONFDIR)/include/ \
	   -I $(CONFDIR)/include/ConfAccess \
	   -I $(CONFDIR)/include/USBTools \
	   -I $(CONFDIR)/include/SqwaqLog \
           -I /usr/local/sqwaq/include



# C pre-processor flags
CPPFLAGS = $(INCLUDES)

# compiler flags
CFLAGS = -Wall

# linker flags
LDFLAGS = -L /usr/local/sqwaq/lib \
#           -L $(IDIR)/build/lib/$(CPU) 
#CONFLAGS=  -L $(CONFDIR)/lib
LDLIBS =  -lusbtools -lrt -lpthread  -lm


# Project name.  This is used in building the file name for the backup target
PROJECTNAME = CTL 


# Enter the names of the C++ source files that you want to compile
# with this project

#SRCS = sq_config.c config_io.c
SRCS = $(wildcard *.c)
HEADERS = $(wildcard *.h)

# Enter the name for the executable to be created
EXEC =  gpstools 


#
# You normally don't need to change anything below here.
# ======================================================
#
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(EXEC)

install: 
	cp gpstools /usr/local/sqwaq/bin
	ln -f /usr/local/sqwaq/bin/gpstools /usr/local/bin

uninstall: 
	rm -f /usr/local/sqwaq/bin/gpstools
	rm -f /usr/local/bin/gpstools

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
%.d:%.c
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


# Include the dependency files created by the PreProcessor.  The
# dash in front of the command keeps the system from complaining
# if the files do not exist.

-include $(SRCS:.c=.d)

