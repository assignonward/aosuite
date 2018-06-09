#!/bin/bash
#
# https://git.gnupg.org/cgi-bin/gitweb.cgi?p=gnupg.git;a=blob;f=README

set -e

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

pushd npth
git checkout npth-1.5
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libgpg-error
git checkout libgpg-error-1.31
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libgcrypt
git checkout libgcrypt-1.8.2
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libksba
git checkout libksba-1.3.5
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libassuan
git checkout libassuan-2.5.1
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd gnupg
git checkout gnupg-2.2.8
./autogen.sh --force
./configure --enable-maintainer-mode --enable-all-tests
make -j
make check
sudo make install
popd


pushd gpgme
git checkout gpgme-1.11.1
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

