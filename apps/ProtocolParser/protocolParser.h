/* MIT License
 *
 * Copyright (c) 2021 Assign Onward
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
#ifndef PROTOCOLPARSER_H
#define PROTOCOLPARSER_H

#include "blockOb.h"

/**
 * @brief The ProtocolParser class - reads a single protocol and passes requested data to actor-users
 *   TODO: a ProtocolListParser to read a list of protocols and separate them into
 *         individual protocols that this class can handle.
 */
class ProtocolParser : public QObject
{
    Q_OBJECT
public:
              ProtocolParser( const BaoSerial &b, QObject *parent = nullptr ) : QObject( parent ) { pr = new KeyValuePair( b, this ); }
             ~ProtocolParser() {}
        bool  isValid();
  Utf8String  name();

  QPointer<KeyValuePair> pr;
};

class ProtocolUser : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolUser(QObject *parent = nullptr) : QObject(parent) {}
virtual void protocolSet() = 0;

signals:
    void newName( QString );
    void transactionRecord( QString );

public slots:
    void setProtocol( BaoSerial p );

public:
   QPointer<ProtocolParser> pp;
};

class ProtocolActor : public ProtocolUser
{
    Q_OBJECT
public:
              ProtocolActor( RiceyInt at, QObject *parent = nullptr ) : ProtocolUser(parent) { setActorType(at); }
        void  setActorType( RiceyInt at ) { actTyp = at; }
    RiceyInt  actType() { return actTyp; }
virtual void  protocolSet();

signals:
        void  newProtocolSet( ProtocolActor * );

public:
    QPointer<BlockValueObject>   sendableItemDefs;
    QPointer<BlockValueObject> receivableItemDefs;
                     RiceyInt  actTyp;

};

#endif // PROTOCOLPARSER_H
