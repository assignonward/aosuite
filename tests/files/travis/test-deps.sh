#!/bin/sh

if [ "$QT_SELECT" = "qt4" ]; then
    sudo apt-get update
    sudo apt-get install libqt4-dev
else
    sudo add-apt-repository -y ppa:ubuntu-sdk-team/ppa
    sudo apt-get update
    sudo apt-get install qtbase5-dev
fi