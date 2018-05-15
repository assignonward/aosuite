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

INCLUDEPATH += $${PWD}
INCLUDEPATH += /usr/include
LIBS        += -lgpgme -lgcrypt -ldl -lgpg-error

SOURCES     += $${PWD}/dataitem.cpp         \
               $${PWD}/data16.cpp           \
               $${PWD}/data128.cpp          \
               $${PWD}/datafixedlength.cpp  \
               $${PWD}/datavarlenlong.cpp   \
               $${PWD}/index.cpp            \
               $${PWD}/listsize.cpp         \
               $${PWD}/aotime.cpp           \
               $${PWD}/shares.cpp           \
               $${PWD}/hash.cpp             \
               $${PWD}/hash256.cpp          \
               $${PWD}/hash512.cpp          \
               $${PWD}/pubkey.cpp           \
               $${PWD}/publickeyecdsa.cpp   \
               $${PWD}/publickeyrsa3072.cpp \
               $${PWD}/salt256.cpp          \
               $${PWD}/note.cpp             \
               $${PWD}/sigecdsa.cpp         \
               $${PWD}/sigrsa3072.cpp       \
               $${PWD}/datavarlenshort.cpp

HEADERS     += $${PWD}/bytecodes.h        \
               $${PWD}/dataitem.h         \
               $${PWD}/data16.h           \
               $${PWD}/data128.h          \
               $${PWD}/datafixedlength.h  \
               $${PWD}/datavarlenlong.h   \
               $${PWD}/index.h            \
               $${PWD}/listsize.h         \
               $${PWD}/aotime.h           \
               $${PWD}/shares.h           \
               $${PWD}/hash.h             \
               $${PWD}/hash256.h          \
               $${PWD}/hash512.h          \
               $${PWD}/pubkey.h           \
               $${PWD}/publickeyecdsa.h   \
               $${PWD}/publickeyrsa3072.h \
               $${PWD}/salt256.h          \
               $${PWD}/note.h             \
               $${PWD}/sigecdsa.h         \
               $${PWD}/sigrsa3072.h       \
               $${PWD}/datavarlenshort.h
