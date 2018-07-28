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
{ gpgKeys[0] = nullptr;
}

CryptoEngine::~CryptoEngine()
{}

/**
 * @brief CryptoEngine::makeNewGCryPair
 * @param tc - type of keypair to make
 * @return ByteArray with the key data?
 *
 * gcry_error_t gcry_pk_genkey(gcry_sexp_t *r_key, gcry_sexp_t parms)
 *   This function create a new key pair using information given in the
 *   S-expression parms and stores the private and the public key in one new
 *   S-expression at the address given by r_key. In case of an error, r_key is
 *   set to NULL. The return code is 0 for success or an error code otherwise.
 */
KeyPair *CryptoEngine::makeNewGCryPair( typeCode_t tc, QObject *p )
{ KeyPair *kpp = nullptr;
  gpgme_error_t err;
  gcry_sexp_t parms = nullptr;

  if ( tc == AO_ECDSA_PRI_KEY )
    { SHOW_IF_GPGERR( gcry_sexp_build(&parms, nullptr, "(genkey (ecc (curve brainpoolP256r1)))") ) }
   else if ( tc == AO_RSA3072_PRI_KEY )
    { SHOW_IF_GPGERR( gcry_sexp_build(&parms, nullptr, "(genkey (rsa (nbits 4:3072)))") ) }
   else
    { qDebug( "CryptoEngine::makeNewGCryPair(%lld) invalid type", tc );
      gcry_sexp_release(parms);
      return kpp;
    }
  gcry_sexp_t keypair;
  SHOW_IF_GPGERR( gcry_pk_genkey(&keypair, parms) )

#define MAX_KEY_SZ 16384
   void *buffer = malloc( MAX_KEY_SZ );
      size_t sz = gcry_sexp_sprint( keypair, GCRYSEXP_FMT_CANON, (char *)buffer, MAX_KEY_SZ );
  QByteArray ba = QByteArray( (const char *)buffer, sz ); // Make a deep copy of buffer

  // https://www.gnupg.org/documentation/manuals/gcrypt/Working-with-S_002dexpressions.html

  // size_t gcry_sexp_sprint (gcry_sexp_t sexp, int mode, char *buffer, size_t maxlength)
  // Copies the S-expression object sexp into buffer using the format specified in mode.
  // maxlength must be set to the allocated length of buffer. The function returns the
  // actual length of valid bytes put into buffer or 0 if the provided buffer is too short.
  // Passing NULL for buffer returns the required length for buffer. For convenience reasons
  // an extra byte with value 0 is appended to the buffer.
  // GCRYSEXP_FMT_CANON Return the S-expression in canonical format.

  // gcry_error_t gcry_sexp_new(gcry_sexp_t *r_sexp, const void *buffer, size_t length, int autodetect)
  // This is the generic function to create an new S-expression object from its external representation
  // in buffer of length bytes. On success the result is stored at the address given by r_sexp. With
  // autodetect set to 0, the data in buffer is expected to be in canonized format

  PriKey *priKp = nullptr;
  PubKey *pubKp = nullptr;
  if ( tc == AO_ECDSA_PRI_KEY )
    { PrivateKeyEcdsa *eccPriKp = new PrivateKeyEcdsa( ba, p ); // storing the whole S expression in canonical form
      priKp = new PriKey( eccPriKp, p ); // priKp tookover ownership of eccPriKp
      gcry_sexp_t itSexp = gcry_sexp_find_token(keypair, "q", 1);
      sz = gcry_sexp_sprint( itSexp, GCRYSEXP_FMT_DEFAULT, buffer, MAX_KEY_SZ );
      const char *data = gcry_sexp_nth_data(itSexp, 1, &sz);
      ba = QByteArray( data, sz );
      PublicKeyEcdsa *eccPubKp = new PublicKeyEcdsa( (DataItemBA)ba, p );
      pubKp = new PubKey( eccPubKp, p );
      gcry_sexp_release(itSexp);
    }
   else if( tc == AO_RSA3072_PRI_KEY )
    { PrivateKeyRsa3072 *rsaPriKp = new PrivateKeyRsa3072( ba, p ); // storing the whole S expression in canonical form
      priKp = new PriKey( rsaPriKp, p );
      // TODO: properly strip down to the RSA private key, this is likely wrong.
      gcry_sexp_t itSexp = gcry_sexp_find_token(keypair, "q", 1);
      sz = gcry_sexp_sprint( itSexp, GCRYSEXP_FMT_DEFAULT, buffer, MAX_KEY_SZ );
      const char *data = gcry_sexp_nth_data(itSexp, 1, &sz);
      ba = QByteArray( data, sz );
      PublicKeyRsa3072 *rsaPubKp = new PublicKeyRsa3072( (DataItemBA)ba, p );
      pubKp = new PubKey( rsaPubKp, p );
      delete rsaPriKp;
      delete rsaPubKp;
      gcry_sexp_release(itSexp);
    }
   else
    { qDebug( "should never get here due to checks above." ); }

  if ( priKp && pubKp )
    kpp = new KeyPair( priKp, pubKp, p );
   else
    { qDebug( "didn't get both keys in the pair" );
      if ( priKp ) delete priKp;
      if ( pubKp ) delete pubKp;
    }
  free( buffer );
  gcry_sexp_release(keypair);
  gcry_sexp_release(parms);

  return kpp;
}


//////////////////////////////////////////////////////////////////////////////////

gpgme_error_t CryptoEngine::initGpgme()
{ if ( !gpgme_check_version( "1.8.1" ) )
    { qDebug( "libpgme version fails to meet 1.8.1 expectation" ); }
  setlocale( LC_ALL, "" );
  gpgme_set_locale( nullptr, LC_CTYPE   , setlocale( LC_CTYPE   , nullptr ) );
  gpgme_set_locale( nullptr, LC_MESSAGES, setlocale( LC_MESSAGES, nullptr ) );
  gpgme_error_t err;
  SHOW_IF_GPGERR( gpgme_set_engine_info( GPGME_PROTOCOL_OpenPGP, "/usr/local/bin/gpg2", nullptr ) )
  return gpgme_engine_check_version( GPGME_PROTOCOL_OpenPGP );
}

/**
 * @brief CryptoEngine:makeNewGpgPair
 * @param tc - type of keyPair to make (using the private key type).
 * @return fingerprint string of the new key pair if successful, empty byte array if fail.
 */
QByteArray CryptoEngine::makeNewGpgPair( typeCode_t tc )
{ qDebug( "CryptoEngine::makeNewGpgPair( %lld )", tc );
  if (( tc != AO_ECDSA_PRI_KEY ) &&
      ( tc != AO_RSA3072_PRI_KEY ))
    { qDebug( "CryptoEngine::makeNewGpgPair unrecognized type (%lld)", tc );
      return QByteArray();
    }
  gpgme_ctx_t ctx;
  gpgme_engine_info_t info;
  gpgme_error_t  err = GPG_ERR_NO_ERROR;
  QByteArray cfs = configFolder.toUtf8();
  const char * CONFIG_DIR = cfs.data();

  // Initializes gpgme
  gpgme_check_version(nullptr);

  // Initialize the locale environment.
  setlocale(LC_ALL, "");
  gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));
  #ifdef LC_MESSAGES
    gpgme_set_locale(nullptr, LC_MESSAGES, setlocale(LC_MESSAGES, nullptr));
  #endif
  BAFAIL_IF_GPGERR( gpgme_set_engine_info(GPGME_PROTOCOL_OpenPGP, nullptr, CONFIG_DIR) );
  BAFAIL_IF_GPGERR( gpgme_new(&ctx) );
  BAFAIL_IF_GPGERR( gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP) ); // Check OpenPGP
  BAFAIL_IF_GPGERR( gpgme_get_engine_info(&info) );                      // load engine info
  while ( info && info->protocol != gpgme_get_protocol(ctx) )
    info = info->next;
  BAFAIL_IF_GPGERR( gpgme_ctx_set_engine_info(ctx, GPGME_PROTOCOL_OpenPGP, nullptr, CONFIG_DIR) );  // set path to our config file

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

  BAFAIL_IF_GPGERR( gpgme_op_genkey(ctx, def.toStdString().c_str(), nullptr, nullptr ) );
  if (err == gpgme_err_code(GPG_ERR_NO_ERROR) )
    { gpgme_genkey_result_t res = gpgme_op_genkey_result(ctx);
      qDebug( "KeyPair::makeNewPair(%lld) primary:%d sub:%d uid:%d fpr:%s"
              , tc, res->primary, res->sub, res->uid, res->fpr );

      if (res->primary && res->sub)
        { QByteArray fp = QByteArray( res->fpr );
          gpgme_release(ctx);
          return fp;
        }
    }
  qDebug( "fallthrough" );
  gpgme_release(ctx);

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
  IFAIL_IF_GPGERR( gpgme_op_keylist_start( ctx, nullptr, 0 ) )
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
  gpgme_release(ctx);

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
  gpgme_check_version(nullptr);

  // Initialize the locale environment.
  setlocale(LC_ALL, "");
  gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));
  #ifdef LC_MESSAGES
    gpgme_set_locale(nullptr, LC_MESSAGES, setlocale(LC_MESSAGES, nullptr));
  #endif
  BAFAIL_IF_GPGERR( gpgme_set_engine_info(GPGME_PROTOCOL_OpenPGP, nullptr, CONFIG_DIR) );
  BAFAIL_IF_GPGERR( gpgme_new(&ctx) );
  BAFAIL_IF_GPGERR( gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP) ); // Check OpenPGP
  BAFAIL_IF_GPGERR( gpgme_get_engine_info(&info) );                      // load engine info
  while ( info && info->protocol != gpgme_get_protocol(ctx) )
    info = info->next;
  BAFAIL_IF_GPGERR( gpgme_ctx_set_engine_info(ctx, GPGME_PROTOCOL_OpenPGP, nullptr, CONFIG_DIR) );  // set path to our config file

  gpgme_data_t keydata;
  BAFAIL_IF_GPGERR( gpgme_data_new(&keydata) )
  qDebug( "about to export %s", pattern );
  BAFAIL_IF_GPGERR( gpgme_op_export( ctx, pattern, GPGME_EXPORT_MODE_MINIMAL | GPGME_EXPORT_MODE_SECRET, keydata ) )
  gpgme_data_seek( keydata, 0, SEEK_SET );
  char buf[131072];
  ssize_t sz = gpgme_data_read( keydata, buf, 131072 );
  qDebug( "Read %ld bytes\n", sz );

  // Find the key, then delete it from the keyring
  gpgme_key_t key;
  SHOW_IF_GPGERR( gpgme_op_keylist_start( ctx, pattern, 1 ) )
  SHOW_IF_GPGERR( gpgme_op_keylist_next ( ctx, &key ) )
  if ( !key )
    { qDebug( "key not found by fingerprint %s", pattern ); }
   else
    { if ( key->invalid )
        { qDebug( "key invalid" ); }
       else
        { SHOW_IF_GPGERR( gpgme_op_delete_ext( ctx, key, GPGME_DELETE_ALLOW_SECRET | GPGME_DELETE_FORCE ) ) }
    }
  gpgme_release(ctx);

  return QByteArray( buf, sz );
}

/**
 * @brief CryptoEngine::importKey - attempt to import a key to the gpg keyring from a binary blob
 * @param priKeyData - the same bytes that were returned from exportKey
 * @return true if successful
 */
bool CryptoEngine::importKey( QByteArray priKeyData )
{ qDebug( "CryptoEngine::importKey" );

  gpgme_ctx_t ctx;
  gpgme_engine_info_t info;
  gpgme_error_t  err = GPG_ERR_NO_ERROR;
  QByteArray cfs = configFolder.toUtf8();
  const char * CONFIG_DIR = cfs.data();

  // Initializes gpgme
  gpgme_check_version(nullptr);

  // Initialize the locale environment.
  setlocale(LC_ALL, "");
  gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));
  #ifdef LC_MESSAGES
    gpgme_set_locale(nullptr, LC_MESSAGES, setlocale(LC_MESSAGES, nullptr));
  #endif
  FAIL_IF_GPGERR( gpgme_set_engine_info(GPGME_PROTOCOL_OpenPGP, nullptr, CONFIG_DIR) );
  FAIL_IF_GPGERR( gpgme_new(&ctx) );
  FAIL_IF_GPGERR( gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP) ); // Check OpenPGP
  FAIL_IF_GPGERR( gpgme_get_engine_info(&info) );                      // load engine info
  while ( info && info->protocol != gpgme_get_protocol(ctx) )
    info = info->next;
  FAIL_IF_GPGERR( gpgme_ctx_set_engine_info(ctx, GPGME_PROTOCOL_OpenPGP, nullptr, CONFIG_DIR) );  // set path to our config file

  gpgme_data_t keydata;
  FAIL_IF_GPGERR( gpgme_data_new_from_mem(&keydata,priKeyData.data(),priKeyData.size(),0) )
  qDebug( "about to import" );
  FAIL_IF_GPGERR( gpgme_op_import(ctx, keydata) )
  gpgme_release(ctx);

  return true;
}
