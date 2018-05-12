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

include(../DataItems/DataItems.pri)

QT        += core gui widgets
TARGET     = ao
TEMPLATE   = app
DEFINES   += QT_DEPRECATED_WARNINGS
DEFINES   += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES   += main.cpp         \
             mainwindow.cpp   \
             random.cpp       \
             transaction.cpp  \
             underwriting.cpp \
             chainmaker.cpp   \
             participant.cpp  \
             pageref.cpp      \
             blockref.cpp \
    participantlist.cpp

HEADERS   += mainwindow.h     \
             random.h         \
             transaction.h    \
             underwriting.h   \
             chainmaker.h     \
             participant.h    \
             pageref.h        \
             blockref.h \
    participantlist.h

FORMS     += mainwindow.ui

RESOURCES += resources.qrc

