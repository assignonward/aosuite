#!/bin/bash
#
# https://git.gnupg.org/cgi-bin/gitweb.cgi?p=gnupg.git;a=blob;f=README

sudo rm -rf npth
sudo rm -rf libgpg-error
sudo rm -rf libgcrypt
sudo rm -rf libksba
sudo rm -rf libassuan
sudo rm -rf gnupg
sudo rm -rf gpgme
git submodule init
git submodule update

