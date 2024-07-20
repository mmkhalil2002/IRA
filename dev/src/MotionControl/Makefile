#!/usr/bin/make -f
#
# Filename:		Makefile
# Date:			05/07/2019
# Author:		Stephen Perkins
# Email:		stephen.perkins@sqwaq.com
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

# C pre-processor flags
CPPFLAGS =  $(INCLUDES)

# compiler flags
CFLAGS = -Wall

# linker flags
LDFLAGS =
LDLIBS =  -lwiringPi -lpthread -lm\

INCLUDES = -I /usr/local/robot//include



# Project name.  This is used in building the file name for the backup target
PROJECTNAME = robot


# Enter the names of the C++ source files that you want to compile
# with this project

#SRCS = sq_config.c config_io.c
SRCS = $(wildcard *.c)
HEADERS = $(wildcard *.h)

# Enter the name for the executable to be created
#EXEC = conf_access_test
LIBFILE = librobot.a

#
# You normally don't need to change anything below here.
# ======================================================
#
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

dev: lib/$(LIBFILE)


clean:
	rm -fr lib
	rm -f $(OBJS) *.d *~ .*~ TAGS

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

ifneq ($(wildcard /etc/robot/.),)
	@echo /etc/robot already exists
else
	ln -fs /usr/local/robot/etc /etc/robot
endif


uninstall: 
	sudo rm /usr/local/robot/lib/$(LIBFILE)
	sudo rm /usr/local/robot/include/$(HEADERS)


# This target installs the CONF file AND the headers/libraries
# needed to do development against the CONF file
install: 
	sudo mkdir -p /usr/local/robot/lib
	sudo chmod 755 /usr/local/robot/lib
	sudo cp lib/$(LIBFILE) /usr/local/robot/lib
	sudo chmod 444 /usr/local/robot/lib/$(LIBFILE)
	sudo mkdir -p /usr/local/robot/include/
	sudo chmod 755 /usr/local/robot/include/
	sudo cp -f $(HEADERS) /usr/local/robot/include/
	sudo chmod 444 /usr/local/robot/include/*


# Pattern for .d files.
%.d:%.c
	@echo Updating .d Dependency File
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


#  This is a rule to link the files.  Pretty standard
lib/$(LIBFILE): $(OBJS)
	mkdir -p lib 
	ar rcs lib/$(LIBFILE) $(OBJS)

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

