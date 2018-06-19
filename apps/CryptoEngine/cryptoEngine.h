/* MIT License
 *
 * Copyright (c) 2018 Assign Onward
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CRYPTOENGINE_H
#define CRYPTOENGINE_H

#include <QtGui>
#include "dataitem.h"

#include "gpgme.h"
#include "gpg-error.h"

#define MAX_KEYS 1000

#define SHOW_IF_GPGERR( op ) \
err = op;                     \
if ( err )                     \
  { qDebug( "Show %s: %s (%d)", \
     gpgme_strsource(err),       \
     gpgme_strerror (err),        \
                     err );        \
  }

#define BAFAIL_IF_GPGERR( op ) \
err = op;                     \
if ( err )                     \
  { qDebug( "Fail %s: %s (%d)", \
     gpgme_strsource(err),       \
     gpgme_strerror (err),        \
                     err );        \
    return QByteArray();            \
  }

#define IFAIL_IF_GPGERR( op ) \
err = op;                     \
if ( err )                     \
  { qDebug( "Fail %s: %s (%d)", \
     gpgme_strsource(err),       \
     gpgme_strerror (err),        \
                     err );        \
    return -1;                      \
  }

#define FAIL_IF_GPGERR( op ) \
err = op;                     \
if ( err )                     \
  { qDebug( "Fail %s: %s (%d)", \
     gpgme_strsource(err),       \
     gpgme_strerror (err),        \
                     err );        \
    return false;                   \
  }

class CryptoEngine : public QObject
{
    Q_OBJECT

public:
         explicit  CryptoEngine( QObject *p = NULL );
                  ~CryptoEngine();

       QByteArray  makeNewPair( typeCode_t tc );
       QByteArray  exportKey( QByteArray fingerprint );
             bool  importKey( QByteArray priKeyData );
    gpgme_error_t  initGpgme();
           qint32  getKeyInfo();
             void  setPassphrase( QString pp ) { passphrase = pp; }

public:
      gpgme_key_t  gpgKeys[MAX_KEYS+1];
          QString  configFolder;
          QString  passphrase;
};

#endif // CRYPTOENGINE_H
