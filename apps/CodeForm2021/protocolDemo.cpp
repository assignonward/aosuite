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
#include "protocolDemo.h"

ProtocolDemo::ProtocolDemo(QObject *parent) : QObject(parent)
{
  connect( this, SIGNAL( setProtocol(BsonSerial) ), &wc, SLOT( setProtocol(BsonSerial) ) );
  connect( this, SIGNAL( setProtocol(BsonSerial) ), &rc, SLOT( setProtocol(BsonSerial) ) );
  connect( this, SIGNAL( setProtocol(BsonSerial) ), &ws, SLOT( setProtocol(BsonSerial) ) );
  connect( this, SIGNAL( setProtocol(BsonSerial) ), &rs, SLOT( setProtocol(BsonSerial) ) );
}

void WriterClient::receiveResponse( QByteArray resp )
{ (void)resp;
}

void ReaderClient::receiveResponse( QByteArray resp )
{ (void)resp;
}

void WriterServer::receiveRequest( QByteArray req )
{ (void)req;
}

void ReaderServer::receiveRequest( QByteArray req )
{ (void)req;
}

