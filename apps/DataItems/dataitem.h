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
#ifndef DATAITEM_H
#define DATAITEM_H

#include <QObject>
#include "bytecodes.h"

#define typeCode_t unsigned char

class DataItem : public QObject
{
    Q_OBJECT
public:
           explicit  DataItem( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL );
             qint32  typeSize( typeCode_t tc = AO_UNDEFINED_DATAITEM ) const;
             qint32  typeSize( const QByteArray &di ) const;
  static     qint32  typeSizeTable( typeCode_t tc );
  static typeCode_t  typeCodeOf( const QByteArray &di );
               bool  checksumValidated() const { return csVal; }
 virtual QByteArray  toDataItem( bool cf = false ) const { (void)cf; return QByteArray(); } // Implemented in all child classes

protected:
  typeCode_t  typeCode; // what kind of data item is this?
        bool  csVal; // has the checksum been validated (during a data item initialization, or assignment)?
};

#endif // DATAITEM_H
