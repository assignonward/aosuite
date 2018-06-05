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
#include "keypair.h"

/**
 * @brief KeyPair::KeyPair - constructor
 * @param di - optional data item
 * @param p - optional parent object
 */
KeyPair::KeyPair( DataItemBA di, QObject *p )
  : DataVarLength( AO_KEYPAIR, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_KEYPAIR )
        { // TODO: log an error
          return;
        }
       else
        { DataVarLength temp( di );        // It's our type
          DataItemBA items = temp.get();  // typeCode has been stripped off
          while ( items.size() > 0 )
            { int sz = typeSize( items );
              if ( sz <= 0 )
                { // TODO: log error
                  return;
                }
               else
                { switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                    { case AO_ECDSA_PUB_KEY2:
                      case AO_ECDSA_PUB_KEY3:
                      case AO_RSA3072_PUB_KEY:
                        pubKey = items;
                        break;

                      case AO_ECDSA_PRI_KEY:
                      case AO_RSA3072_PRI_KEY:
                        priKey = items;
                        break;

                      default:
                        // TODO: log anomaly - unrecognized data type
                        break;
                    }
                  items = items.mid( sz ); // move on to the next
                }
            }
        }
    }
}

/**
 * @brief KeyPair::operator =
 * @param di - data item to assign
 */
void KeyPair::operator = ( const DataItemBA &di )
{ KeyPair temp( di );
  pubKey   = temp.pubKey;
  priKey   = temp.priKey;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief KeyPair::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
DataItemBA  KeyPair::toDataItem( bool cf )
{ QByteArrayList dil;
  if ( pubKey.isValid() )
    dil.append( pubKey.toDataItem(cf) );
  if ( priKey.isValid() )
    dil.append( priKey.toDataItem(cf) );
  std::sort( dil.begin(), dil.end() );
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}

#include "gpgme.h"
#include "gpg-error.h"
/**
 * @brief KeyPair::makeNewPair
 * @param tc - type of keyPair to make (using the private key type).
 * @return true if successful;
 */
bool KeyPair::makeNewPair( typeCode_t tc )
{ qDebug( "KeyPair::makeNewPair( 0x%x )", tc );
  if (( tc != AO_ECDSA_PRI_KEY ) &&
      ( tc != AO_RSA3072_PRI_KEY ))
    { qDebug( "KeyPair::makeNewPair unrecognized type (0x%x)", tc );
      return false;
    }
  gpgme_ctx_t mContext;
  gpgme_engine_info_t info;
  gpgme_error_t  error;
  const char * CONFIG_DIR = "~/";

  // Initializes gpgme
  gpgme_check_version(NULL);

  // Initialize the locale environment.
  setlocale(LC_ALL, "");
  gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
  #ifdef LC_MESSAGES
    gpgme_set_locale(NULL, LC_MESSAGES, setlocale(LC_MESSAGES, NULL));
  #endif
  error = gpgme_set_engine_info(GPGME_PROTOCOL_OpenPGP, NULL, CONFIG_DIR);
  if (error)
    return false;
  error = gpgme_new(&mContext);
  if (error)
    return false;
  // Check OpenPGP
  error = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP);
  if ( error )
    return false;
  // load engine info
  error = gpgme_get_engine_info(&info);
  if ( error )
    return false;
  while ( info && info->protocol != gpgme_get_protocol(mContext) )
    info = info->next;

  // set path to our config file
  error = gpgme_ctx_set_engine_info(mContext, GPGME_PROTOCOL_OpenPGP, NULL, CONFIG_DIR);
  if ( error )
    return false;

  std::string def = "<GnupgKeyParms format=\"internal\"> \n"
                             " Key-Type: ECDSA \n"
                             " Key-Length: 256 \n"
                             " Subkey-Type: ECDSA \n"
                             " Name-Real: Joe Tester3 \n"
                             " Name-Comment: with stupid passphrase \n"
                             " Name-Email: joe3 at foo.bar \n"
                             " Expire-Date: 0 \n"
                             " Passphrase: abc \n"
                             " </GnupgKeyParms> \n";

  error = gpgme_op_genkey(mContext, def.c_str(), NULL, NULL );

  if ( GPG_ERR_INV_VALUE  == gpgme_err_code(error) )
    qDebug( "Value error" );
  if ( GPG_ERR_NOT_SUPPORTED == gpgme_err_code(error) )
    qDebug( "Not supported error" );
  if ( GPG_ERR_GENERAL  ==  gpgme_err_code(error) )
    qDebug( "general error" );
  if (error == gpgme_err_code(GPG_ERR_NO_ERROR) )
    { gpgme_genkey_result_t res = gpgme_op_genkey_result(mContext);
      if (res->primary && res->sub)
        { qDebug( "KeyPair::makeNewPair( 0x%x ) success?", tc );
          return true;
        }
    }
  qDebug( "fallthrough" );
  return false;
}
