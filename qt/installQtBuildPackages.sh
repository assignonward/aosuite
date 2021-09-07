#!/bin/bash
#/*************************************************
#*       Copyright © 2021, Medtronic, Inc.        *
#* Confidential, Proprietary, All Rights Reserved *
#*************************************************/
# installQtBuildPackages.sh
# experimental
#
set -e
if [[ $EUID -ne 0 ]]
then
echo "installQtBuildPackages.sh must be run with root privileges."
exit 1
fi

echo ""
echo "installQtBuildPackages.sh"
echo ""
set -x

apt-get -y build-dep qt5-default
apt-get install -y libxcb-xinerama0-dev
apt-get install -y build-essential
apt-get install -y perl
apt-get install -y python
apt-get install -y git
apt-get install -y '^libxcb.*-dev'
apt-get install -y libx11-xcb-dev
apt-get install -y libglu1-mesa-dev
apt-get install -y libxrender-dev
apt-get install -y libxi-dev
apt-get install -y libxkbcommon-dev
apt-get install -y libxkbcommon-x11-dev
apt-get install -y flex
apt-get install -y bison
apt-get install -y gperf
apt-get install -y libicu-dev
apt-get install -y libxslt-dev
apt-get install -y ruby
apt-get install -y libxcursor-dev
apt-get install -y libxcomposite-dev
apt-get install -y libxdamage-dev
apt-get install -y libxrandr-dev
apt-get install -y libxtst-dev
apt-get install -y libxss-dev
apt-get install -y libdbus-1-dev
apt-get install -y libevent-dev
apt-get install -y libfontconfig1-dev
apt-get install -y libcap-dev
apt-get install -y libpulse-dev
apt-get install -y libudev-dev
apt-get install -y libpci-dev
apt-get install -y libnss3-dev
apt-get install -y libasound2-dev
apt-get install -y libegl1-mesa-dev
apt-get install -y nodejs
apt-get install -y libclang-6.0-dev
apt-get install -y llvm-6.0
apt-get autoremove -y

echo "installQtBuildPackages.sh complete."
