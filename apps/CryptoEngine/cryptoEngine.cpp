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
#include "cryptoEngine.h"
#include "Key.h"
#include "random.h"

CryptoEngine::CryptoEngine( QObject *p ) : QObject( p )
{ gpgKeys[0] = NULL;
}

CryptoEngine::~CryptoEngine()
{}


gpgme_error_t CryptoEngine::initGpgme()
{ if ( !gpgme_check_version( "1.8.1" ) )
    { qDebug( "libpgme version fails to meet 1.8.1 expectation" ); }
  setlocale( LC_ALL, "" );
  gpgme_set_locale( NULL, LC_CTYPE   , setlocale( LC_CTYPE   , NULL ) );
  gpgme_set_locale( NULL, LC_MESSAGES, setlocale( LC_MESSAGES, NULL ) );
  gpgme_error_t err;
  SHOW_IF_GPGERR( gpgme_set_engine_info( GPGME_PROTOCOL_OpenPGP, "/usr/local/gpgbin/gpg", NULL ) )
  return gpgme_engine_check_version( GPGME_PROTOCOL_OpenPGP );
}

/**
 * @brief CryptoEngine:makeNewPair
 * @param tc - type of keyPair to make (using the private key type).
 * @return fingerprint string of the new key pair if successful, empty byte array if fail.
 */
QByteArray CryptoEngine::makeNewPair( typeCode_t tc )
{ qDebug( "KeyPair::makeNewPair( 0x%x )", tc );
  if (( tc != AO_ECDSA_PRI_KEY ) &&
      ( tc != AO_RSA3072_PRI_KEY ))
    { qDebug( "KeyPair::makeNewPair unrecognized type (0x%x)", tc );
      return QByteArray();
    }
  gpgme_ctx_t mContext;
  gpgme_engine_info_t info;
  gpgme_error_t  err = GPG_ERR_NO_ERROR;
  QByteArray cfs = configFolder.toUtf8();
  const char * CONFIG_DIR = cfs.data();

  // Initializes gpgme
  gpgme_check_version(NULL);

  // Initialize the locale environment.
  setlocale(LC_ALL, "");
  gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
  #ifdef LC_MESSAGES
    gpgme_set_locale(NULL, LC_MESSAGES, setlocale(LC_MESSAGES, NULL));
  #endif
  BAFAIL_IF_GPGERR( gpgme_set_engine_info(GPGME_PROTOCOL_OpenPGP, NULL, CONFIG_DIR) );
  BAFAIL_IF_GPGERR( gpgme_new(&mContext) );
  BAFAIL_IF_GPGERR( gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP) ); // Check OpenPGP
  BAFAIL_IF_GPGERR( gpgme_get_engine_info(&info) );                      // load engine info
  while ( info && info->protocol != gpgme_get_protocol(mContext) )
    info = info->next;
  BAFAIL_IF_GPGERR( gpgme_ctx_set_engine_info(mContext, GPGME_PROTOCOL_OpenPGP, NULL, CONFIG_DIR) );  // set path to our config file

  QStringList ne = rng.rnd_nameAndEmail();
  QString keyType = ( tc == AO_ECDSA_PRI_KEY ) ?
              "key-type:      ECDSA\n"
              "key-curve:     brainpoolP256r1\n"
              "subkey-type:   ECDH\n"
              "subkey-curve:  brainpoolP256r1\n" :

              "key-type:      RSA\n"
              "key-length:    3072\n"
              "subkey-type:   RSA\n"
              "subkey-length: 3072\n" ;

  QString def = QString( "<GnupgKeyParms format=\"internal\">\n%3"
                         "key-usage:     sign\n"
                         "subkey-usage:  encrypt\n"
                         "name-real:     %1\n"
                         "name-email:    %2\n"
                         "name-comment:  Ⓐ\n"
                         "expire-date:   0\n"
                         "passphrase:    %4\n"
                         "</GnupgKeyParms>\n" ).arg(ne.at(0))
                                               .arg(ne.at(1))
                                               .arg( keyType )
                                               .arg( passphrase );

  BAFAIL_IF_GPGERR( gpgme_op_genkey(mContext, def.toStdString().c_str(), NULL, NULL ) );
  if (err == gpgme_err_code(GPG_ERR_NO_ERROR) )
    { gpgme_genkey_result_t res = gpgme_op_genkey_result(mContext);
      qDebug( "KeyPair::makeNewPair(0x%x) primary:%d sub:%d uid:%d fpr:%s"
              , tc, res->primary, res->sub, res->uid, res->fpr );

      if (res->primary && res->sub)
        return QByteArray( res->fpr );
    }
  qDebug( "fallthrough" );
  return QByteArray();
}

/**
 * @brief CryptoEngine::getKeyInfo - count the keys and store them for individual display
 * @return false if failed somewhere
 */
qint32 CryptoEngine::getKeyInfo()
{ gpgme_ctx_t   ctx;
  gpgme_error_t err;
  IFAIL_IF_GPGERR( initGpgme() )
  IFAIL_IF_GPGERR( gpgme_new( &ctx ) )

  // Count the keys:
  IFAIL_IF_GPGERR( gpgme_op_keylist_start( ctx, NULL, 0 ) )
  qint32 n = 0;
  bool done = false;
  while ( !done )
    { err = gpgme_op_keylist_next( ctx, &(gpgKeys[n]) );
      if ( gpg_err_code(err) == GPG_ERR_EOF )
        done = true;
       else
        { IFAIL_IF_GPGERR(err)
          if ( ++n >= MAX_KEYS )
            { qDebug( "ran out of GPG keys storage, coded max of %d.", MAX_KEYS );
              done = true;
            }
        }
    }
  return n;
}

/**
 * @brief CryptoEngine::exportKey
 * @param fingerprint - of key to export
 * @return exported key description (binary blob)
 */
QByteArray  CryptoEngine::exportKey( QByteArray fingerprint )
{ qDebug( "CryptoEngine::exportKey" );

  gpgme_ctx_t ctx;
  gpgme_engine_info_t info;
  gpgme_error_t  err = GPG_ERR_NO_ERROR;
  QByteArray cfs = configFolder.toUtf8();
  const char * CONFIG_DIR = cfs.data();
  const char * pattern = fingerprint.data();

  // Initializes gpgme
  gpgme_check_version(NULL);

  // Initialize the locale environment.
  setlocale(LC_ALL, "");
  gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
  #ifdef LC_MESSAGES
    gpgme_set_locale(NULL, LC_MESSAGES, setlocale(LC_MESSAGES, NULL));
  #endif
  BAFAIL_IF_GPGERR( gpgme_set_engine_info(GPGME_PROTOCOL_OpenPGP, NULL, CONFIG_DIR) );
  BAFAIL_IF_GPGERR( gpgme_new(&ctx) );
  BAFAIL_IF_GPGERR( gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP) ); // Check OpenPGP
  BAFAIL_IF_GPGERR( gpgme_get_engine_info(&info) );                      // load engine info
  while ( info && info->protocol != gpgme_get_protocol(ctx) )
    info = info->next;
  BAFAIL_IF_GPGERR( gpgme_ctx_set_engine_info(ctx, GPGME_PROTOCOL_OpenPGP, NULL, CONFIG_DIR) );  // set path to our config file

  gpgme_data_t keydata;
  BAFAIL_IF_GPGERR( gpgme_data_new(&keydata) )
  qDebug( "about to export %s", pattern );
  BAFAIL_IF_GPGERR( gpgme_op_export( ctx, pattern, GPGME_EXPORT_MODE_MINIMAL | GPGME_EXPORT_MODE_SECRET, keydata ) )
  gpgme_data_seek( keydata, 0, SEEK_SET );
  char buf[131072];
  ssize_t sz = gpgme_data_read( keydata, buf, 131072 );
  qDebug( "Read %ld bytes\n", sz );
  return QByteArray( buf, sz );
}
