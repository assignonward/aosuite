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
#ifndef TESTS_H
#define TESTS_H

#include <QScrollArea>
#include "blockOb.h"

namespace Ui {
class Tests;
}

class Tests : public QScrollArea
{
    Q_OBJECT

public:
   explicit  Tests( QWidget *cw = nullptr );
            ~Tests();
       void  liveDelay( int t );
       bool  testDict( QString &, qint32 & );
       bool  testInt32( QString &, qint32 & );
       bool  testInt32( BlockValueInt32 &, qint32, qint32 &, QString & );
       bool  testInt64( QString &, qint32 & );
       bool  testInt64( BlockValueInt64 &, qint64, qint32 &, QString & );
       bool  testRicey( QString &, qint32 & );
       bool  testRicey( BlockValueRiceyCode &, RiceyInt, qint32 &, QString & );
       bool  testString( QString &, qint32 & );
       bool  testString( BlockValueString &, const Utf8String &, qint32 &, QString & );
       bool  testByteArray( QString &, qint32 & );
       bool  testByteArray( BlockValueByteArray &, const QByteArray &, qint32 &, QString & );
       bool  testInt32A( QString &, qint32 & );
       bool  testInt32A( BlockArrayInt32 &, const QList<qint32> &, qint32 &, QString & );
       bool  testInt64A( QString &, qint32 & );
       bool  testInt64A( BlockArrayInt64 &, const QList<qint64> &, qint32 &, QString & );
       bool  testRiceyA( QString &, qint32 & );
       bool  testRiceyA( BlockArrayRicey &, const QList<RiceyCode> &, qint32 &, QString & );
       bool  testRiceyA( BlockArrayRicey &, const QList<RiceyInt>  &, qint32 &, QString & );

public slots:
       void  on_start_clicked();

public:
         Ui::Tests *ui;
};

#endif // TESTS_H
