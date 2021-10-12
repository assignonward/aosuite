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
#ifndef PROTOCOLDEMO_H
#define PROTOCOLDEMO_H

#include <QObject>
#include "blockOb.h"

class WriterClient : public QObject
{
    Q_OBJECT
public:
    explicit WriterClient(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void sendRequest( QByteArray );

public slots:
    void setProtocol(  BaoSerial p ) { protocol = p; }
    void receiveResponse( QByteArray );

public:
   BaoSerial protocol;
};

class WriterServer : public QObject
{
    Q_OBJECT
public:
    explicit WriterServer(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void sendResponse( QByteArray );

public slots:
    void setProtocol(  BaoSerial p ) { protocol = p; }
    void receiveRequest( QByteArray );

public:
   BaoSerial protocol;
};

class ReaderClient : public QObject
{
    Q_OBJECT
public:
    explicit ReaderClient(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void sendRequest( QByteArray );

public slots:
    void setProtocol(  BaoSerial p ) { protocol = p; }
    void receiveResponse( QByteArray );

public:
   BaoSerial protocol;
};

class ReaderServer : public QObject
{
    Q_OBJECT
public:
    explicit ReaderServer(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void sendResponse( QByteArray );

public slots:
    void setProtocol(  BaoSerial p ) { protocol = p; }
    void receiveRequest( QByteArray );

public:
   BaoSerial protocol;
};

class ProtocolDemo : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolDemo(QObject *parent = nullptr);

signals:
    void setProtocol(  BaoSerial );

public:
    WriterClient  wc;
    WriterServer  ws;
    ReaderClient  rc;
    ReaderServer  rs;
};

#endif // PROTOCOLDEMO_H
