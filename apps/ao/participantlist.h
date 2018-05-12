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
#ifndef PARTICIPANTLIST_H
#define PARTICIPANTLIST_H

#include "participant.h"
#include "listsize.h"

class ParticipantList : public DataVarLenLong
{
    Q_OBJECT
public:
    explicit  ParticipantList( const QByteArray &di = QByteArray(), QObject *p = nullptr );
  QByteArray  toDataItem( typeCode_t tc );
        void  clear();
    ListSize  append( const Participant &part );

  QList<Participant> list; // public so assignment contracts and signature lists may access it directly
private:
           ListSize  size; // Used primarily as a check during serialization and deserialization
};

#endif // PARTICIPANTLIST_H
