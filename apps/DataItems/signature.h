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

#include "dataitem.h"
#include "sigecdsa.h"
#include "sigrsa3072.h"

/**
 * @brief The Signature class - multi-container for various types of signatures
 *   of course, a signature type must correspond to the PubKey type to validate.
 */
class Signature : public DataItem
{
    Q_OBJECT
public:
    explicit  Signature( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = nullptr );
              Signature( const QByteArray &di, QObject *p = nullptr );
              Signature( const Signature &s, QObject *p = nullptr );
        void  operator = ( const Signature &s ) { typeCode = s.typeCode; sigEcdsa = s.sigEcdsa; sigRsa3072 = s.sigRsa3072; }
        void  operator = ( const QByteArray &di );
  QByteArray  toDataItem() const;
  QByteArray  get() const;

private:
    SigEcdsa  sigEcdsa;
  SigRsa3072  sigRsa3072;
};

#endif // SIGNATURE_H
