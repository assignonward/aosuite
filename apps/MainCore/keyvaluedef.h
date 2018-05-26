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
#ifndef KEYVALUEDEF_H
#define KEYVALUEDEF_H

#include "dataitem.h"
#include <QJsonObject>

/**
 * @brief The KeyValueDef class - definition of what goes in a particular key-value key
 */
class KeyValueDef : public QObject
{
    Q_OBJECT
public:
    explicit  KeyValueDef( const QJsonObject &jo = QJsonObject(), QObject *p = NULL )
                : QObject( p ) { fromJsonObject( jo ); }
              KeyValueDef( const KeyValueDef &k, QObject *p = NULL )
                : QObject( p ? p : k.parent() ), key( k.key ), tn( k.tn ), desc( k.desc ), pdef( k.pdef ) {}
        void  operator = ( const KeyValueDef &k ) { key = k.key; tn = k.tn; desc = k.desc; pdef = k.pdef; }
 QJsonObject  toJsonObject() const;
        void  fromJsonObject( const QJsonObject &jo );

      qint16  key;  // numerical value of the key
     QString  tn;   // DataItem subclass name that the value is stored as
     QString  desc; // text description of the key-value
     QString  pdef; // #define short name used in program code
};

/**
 * @brief The KeyValueDefinitions class - the list of KeyValueDef objects stored in the resource file
 */
class KeyValueDefinitions : public QObject
{
    Q_OBJECT
public:
    explicit  KeyValueDefinitions( const QString &filename = ":/files/keyValueDefinitions.json", QObject *parent = NULL )
                : QObject( parent ) { if ( filename.size() > 0 ) fromFile( filename ); }
        void  fromFile( const QString &filename );
        void  toFile( const QString &filename );

    QList<KeyValueDef> kvdList;
};

#endif // KEYVALUEDEF_H
