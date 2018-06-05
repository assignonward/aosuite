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
#ifndef SIGNATURE_H
#define SIGNATURE_H

#include "aotime.h"
#include "data16.h"
#include "datavarlength.h"
#include "pubkey.h"
#include "sigecdsa.h"
#include "sigrsa3072.h"

// https://www.gnupg.org/documentation/manuals/gcrypt/Cryptographic-Functions.html
// Function: gcry_error_t gcry_pk_verify (gcry_sexp_t sig, gcry_sexp_t data, gcry_sexp_t pkey)
// This is used to check whether the signature sig matches the data. The public key pkey must
// be provided to perform this verification. This function is similar in its parameters to
// gcry_pk_sign with the exceptions that the public key is used instead of the private key and
// that no signature is created but a signature, in a format as created by gcry_pk_sign, is
// passed to the function in sig.
//
// The result is 0 for success (i.e. the data matches the signature), or an error code where
// the most relevant code is GCRY_ERR_BAD_SIGNATURE to indicate that the signature does not
// match the provided data.

/**
 * @brief The Signature class - multi-container for various types of signatures
 *   of course, a signature type must correspond to the PubKey type to validate.
 */
class Signature : public DataVarLength
{
    Q_OBJECT
public:
    explicit  Signature( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL );
              Signature( const DataItemBA &di, QObject *p = NULL );
              Signature( const Signature &s, QObject *p = NULL );
        void  operator = ( const Signature &s )
                { sigType    = s.sigType;
                  sigEcdsa   = s.sigEcdsa;
                  sigRsa3072 = s.sigRsa3072;
                  sigTime    = s.sigTime;
                  index      = s.index; }
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false );
  QByteArray  getSig() const;
        void  setSig( const QByteArray &s, typeCode_t tc = AO_UNDEFINED_DATAITEM );
      AOTime  getTime() const { return sigTime; }
        void  setTime( const AOTime &t ) { sigTime = t; }
  typeCode_t  getType() const { return sigType; }
      Data16  getIndex() const { return index; }
        void  setIndex( const Data16 &i ) { index = i; }

private:
  // Every signature has a time of signing
      AOTime  sigTime;      // AO_TIME_OF_SIG
  // Only one of these types of signatures is used at a time
  typeCode_t  sigType;      // This is needed because of the inclusion of sigTime and index in the Signature object, might restructure and have a generic signature container without time and index that could be included here
    SigEcdsa  sigEcdsa;
  SigRsa3072  sigRsa3072;

      Data16  index;   // AO_INDEX, Optional: used in Authorizations to match to index numbers in the participant list
      PubKey  pubKey;  // Optional: when included, contains the pubKey used to sign
};

#endif // SIGNATURE_H
