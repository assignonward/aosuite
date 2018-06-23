# MIT License
#
# Copyright (c) 2018 Assign Onward
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

QT          += core gui widgets
TEMPLATE     = app

DEPTH = ../..

# sudo apt-get install libgpgme11-dev libgcrypt11-dev libbz2-dev lib64z1-dev
INCLUDEPATH += $${DEPTH}/OpenPGP
INCLUDEPATH += /usr/local/include
LIBS        += -lOpenPGP -lgmp -lgmpxx -lbz2 -lz -ldl -lgcrypt -lgpgme -lgpg-error -L$$PWD/$${DEPTH}/OpenPGP -L/usr/local/lib
DEFINES     += _FILE_OFFSET_BITS=64

include($${DEPTH}/qamqp.pri)
INCLUDEPATH += $${QAMQP_INCLUDEPATH}
LIBS        += -L$${DEPTH}/src $${QAMQP_LIBS}
macx:CONFIG -= app_bundle

include(../AboutForm/AboutForm.pri)
include(../CryptoForm/CryptoForm.pri)
include(../DataCollections/DataCollections.pri)
include(../DataItems/DataItems.pri)
include(../OrganizerData/OrganizerData.pri)
include(../Random/Random.pri)
include(../SingleApplication/singleapplication.pri)
DEFINES     += QAPPLICATION_CLASS=QApplication

INCLUDEPATH += $${PWD}

SOURCES     += $${PWD}/main.cpp          \
               $${PWD}/MainWinCommon.cpp

HEADERS     += $${PWD}/MainWinCommon.h

