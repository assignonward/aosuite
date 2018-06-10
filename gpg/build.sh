#!/bin/bash
#
# https://git.gnupg.org/cgi-bin/gitweb.cgi?p=gnupg.git;a=blob;f=README

./setVersions.sh

set -e

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

pushd npth
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libgpg-error
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libgcrypt
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libksba
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd libassuan
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

pushd gnupg
./autogen.sh --force
./configure --enable-maintainer-mode --enable-all-tests
make -j
make check
sudo make install
popd

pushd gpgme
./autogen.sh --force
./configure --enable-maintainer-mode
make -j
make check
sudo make install
popd

