#!/bin/bash
#
# https://git.gnupg.org/cgi-bin/gitweb.cgi?p=gnupg.git;a=blob;f=README

pushd npth
make clean
rm configure
popd

pushd libgpg-error
make clean
rm configure
popd

pushd libgcrypt
make clean
rm configure
popd

pushd libksba
make clean
rm configure
popd

pushd libassuan
make clean
rm configure
popd

pushd gnupg
make clean
rm configure
popd


pushd gpgme
make clean
rm configure
popd

