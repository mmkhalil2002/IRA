#/bin/bash

OPER=vzw
TAG=undefined

echo  $1 $2 $3 $4

if [ $1 = "-oper" ]
then
OPER=$2
fi

if [ $3 = "-oper" ]
then
OPER=$4
fi

if [ $1 = "-tag" ]
then
TAG=$2
fi

if [ $3 = "-tag" ]
then
TAG=$4
fi

echo  generation of dynamic info started  $OPER  $TAG
configen  -tag $TAG> /tmp/macros.txt
mkdir -p /usr/local/sqwaq/etc
chmod 755 /usr/local/sqwaq/etc


if [ $OPER = "vzw" ]
then
echo  generating Verizon conf file
cat /tmp/macros.txt /usr/local/sqwaq/etc/templates/config_template_vzw.conf | m4 > /tmp/nsq_config.conf
pwd
fi
if [ $OPER = "att" ]
then
echo  generating ATT conf file
cat /tmp/macros.txt /usr/local/sqwaq/etc/templates/config_template_att.conf | m4 > /tmp/nsq_config.conf
pwd
fi
if [ $OPER = "sprint" ]
then
echo  generating Sprint conf file
cat /tmp/macros.txt /usr/local/sqwaq/etc/templates/config_template_sprint.conf | m4 > /tmp/nsq_config.conf
pwd
fi

awk 'NF' /tmp/nsq_config.conf > /tmp/tmp_nsq_config.conf
mv   /tmp/tmp_nsq_config.conf /tmp/nsq_config.conf
filename="sq_config_"$TAG".conf"
echo $filename
mv   /tmp/nsq_config.conf /usr/local/sqwaq/etc/$filename
chmod 444 /usr/local/sqwaq/etc/$filename
echo  generation of dynamic info  completed

