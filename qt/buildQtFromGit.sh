#!/bin/bash
#/*************************************************
#*       Copyright © 2021, Medtronic, Inc.        *
#* Confidential, Proprietary, All Rights Reserved *
#*************************************************/
# buildQtFromGit.sh
# Build Qt from the git source tree, using the VERSION tag
# install that version to the system and set it as default
# for system and sw user through qtchooser configuration.
#
set -e
if [[ "$USER" != "sw" ]]
then
echo "System should be configured to auto-login user sw."
echo "buildQtFromGit.sh must be run as user sw."
exit 1
fi

echo ""
echo "# buildQtFromGit.sh"
echo ""
set -x

# Tricky define names, pay close attention:
if [ -z "$1" ]
then
  VERSION="5.12.11"
 else
  VERSION=$1
fi
QFOLDER="Qt${VERSION}"
OFOLDER="/opt/${QFOLDER}"
CHOOSER="/usr/share/qtchooser/${QFOLDER}-locallyBuilt.conf"
CHOUSER="/home/sw/.config/qtchooser"
CHOUSEF="${CHOUSER}/default.conf"
DEFAULT="/usr/lib/x86_64-linux-gnu/qt-default/qtchooser/default.conf"

sudo ./installQtBuildPackages.sh

pushd /home/sw
  rm -rf qt5
  git clone https://code.qt.io/qt/qt5.git
  cd qt5
  git checkout origin/$VERSION
  perl init-repository --module-subset=default,-qtwebkit,-qtwebkit-examples,-qtwebengine,-qtsensors,-qtpurchasing,-qtcharts,-qtdatavis3d,-qtgamepad,-qtspeech,-qtlocation
  cd ..
  rm -rf build
  mkdir build
  cd build
  ../qt5/configure -prefix $OFOLDER -opensource -confirm-license -platform linux-g++ -optimized-qmake -nomake examples -nomake tests
  make -j8
  sudo make install

# Configure qtchooser to use our freshly built Qt as the system and user default
  sudo rm -f $CHOOSER
  sudo touch $CHOOSER
  echo "${OFOLDER}/bin" | sudo tee -a $CHOOSER
  echo "${OFOLDER}/lib" | sudo tee -a $CHOOSER
  sudo rm -f $DEFAULT
  sudo ln -s $CHOOSER $DEFAULT
  mkdir -p $CHOUSER
  sudo rm -f $CHOUSEF
  ln -s $CHOOSER $CHOUSEF

# Clean up the build and source folders
  cd ..
  rm -rf build
  rm -rf qt5
popd

echo "buildQtFromGit.sh complete."
