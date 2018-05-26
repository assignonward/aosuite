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
 * @brief The ByteCodeDef class - definition of what goes in a particular key-value key
 */
class ByteCodeDef : public QObject
{
    Q_OBJECT
public:
    explicit  ByteCodeDef( const QJsonObject &jo = QJsonObject(), QObject *p = NULL )
                : QObject( p ) { fromJsonObject( jo ); }
              ByteCodeDef( const ByteCodeDef &k, QObject *p = NULL )
                : QObject( p ? p : k.parent() ), byte( k.byte ), tn( k.tn ), desc( k.desc ), pdef( k.pdef ) {}
        void  operator = ( const ByteCodeDef &k ) { byte = k.byte; tn = k.tn; desc = k.desc; pdef = k.pdef; }
        void  fromJsonObject( const QJsonObject &jo );
     QString  toDefine( qint32 maxLenPdef = 24 );

  typeCode_t  byte; // numerical value of the bytecode
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

  QList<ByteCodeDef> bcdList;
};

#endif // BYTECODEDEF_H
