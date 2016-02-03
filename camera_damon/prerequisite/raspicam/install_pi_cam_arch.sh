#!/bin/bash

PARM_REQ_PACKAGES="git autoconf gettext libtool libjpeg"
PARM_GITHUB_URL="git://git.linuxtv.org/v4l-utils.git"
PARM_TESTING_URL="http://www.cityu.edu.hk/"
PARM_GITHUB_REPO="v4l-utils"
PARM_CAM_MODULES="bcm2835-v4l2"
PARM_MODULES_CONF="/etc/modules-load.d/raspberrypi.conf"

function check_before_run
{
	echo "Checking prerequisite before running..."
	if [ "$(whoami)" != "root" ];then
		echo -e "Warning: This script need to be run as root"
		exit
	fi
	
	wget -q --tries=10 --timeout=20 --spider $PARM_TESTING_URL
	if [ $? -ne 0 ];then
		echo -e "Fatal: No Internet Connection"
		exit
	fi
}

function check_packages_and_install
{
	echo "Checking if all required packages are installed..."
	for i in $PARM_REQ_PACKAGES;do
		pacman -Q | grep $i > /dev/null 2>&1
		if [ $? -ne 0 ];then
			echo "package $i is not installed"
			pacman -Syy $i
		fi
	done
}

function clone_source_code_and_compile
{
	echo "Cloning the source code from GitHub..."
	git clone $PARM_GITHUB_URL
	echo "Compiling..."
	(cd $PARM_GITHUB_REPO && ./bootstrap.sh && ./configure && make && make install)
	rm -rf $PARM_GITHUB_REPO
}

function configure_modules
{
	echo "Add modules to /etc/modules"
	cat $PARM_CAM_MODULES >> $PARM_MODULES_CONF
	modprobe $PARM_CAM_MODULES
}

check_before_run
check_packages_and_install
clone_source_code_and_compile
configure_modules