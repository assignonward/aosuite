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
#ifndef BYTECODEDEF_H
#define BYTECODEDEF_H

#include "dataitem.h"
#include <QJsonObject>

/**
 * @brief The ByteCodeDef class - what goes in a particular byte code definition
 */
class ByteCodeDef : public QObject
{
    Q_OBJECT
public:
    explicit  ByteCodeDef( const QJsonObject &jo = QJsonObject(), QObject *p = NULL )
                : QObject( p ) { fromJsonObject( jo ); }
              ByteCodeDef( const ByteCodeDef &k, QObject *p = NULL )
                : QObject( p ? p : k.parent() ),
                  code( k.code ),
                  cont( k.cont ),
                  sepr( k.sepr ),
                    sz( k.sz   ),
                  gbcr( k.gbcr ),
                    tn( k.tn   ),
                  desc( k.desc ),
                  pdef( k.pdef ) {}
        void  operator = ( const ByteCodeDef &k )
                { code = k.code;
                  cont = k.cont;
                  sepr = k.sepr;
                    sz = k.sz  ;
                  gbcr = k.gbcr;
                    tn = k.tn  ;
                  desc = k.desc;
                  pdef = k.pdef; }
        void  fromJsonObject( const QJsonObject &jo );
 QJsonObject  toJsonObject() const;
     QString  toDefine( qint32 maxLenPdef = 26 );
     QString  toCase( qint32 maxLenPdef = 26 );
     QString  toCaseDataItem( qint32 maxLenPdef = 26 );
     QString  toSizeCase( qint32 maxLenPdef = 26 );

  typeCode_t  code; // numerical value of the bytecode
     QString  cont; // Potential content of a GenericCollection, listed as comma separated pdef names
        bool  sepr; // separable?  defaults to false, true means that the hash of the data is hashed in the collection, instead of the data itself
      qint32  sz;   // size of the following data in bytes, or -1 if variable size
     QString  gbcr; // Genesis block creator ui type
     QString  tn;   // DataItem subclass name that the value is stored as
     QString  desc; // text description of the bytecode usage
     QString  pdef; // #define short name used in program code
};

/**
 * @brief The ByteCodeDefinitions class - the list of ByteCodeDef objects stored in the resource file
 */
class ByteCodeDefinitions : public QObject
{
    Q_OBJECT
public:
    explicit  ByteCodeDefinitions( const QString &filename = ":/files/byteCodeDefinitions.json", QObject *parent = NULL )
                : QObject( parent ) { if ( filename.size() > 0 ) fromFile( filename ); }
        void  fromFile( const QString &filename );
     QString  toString() const;

  QList<ByteCodeDef> bcdList;
};

#endif // BYTECODEDEF_H
