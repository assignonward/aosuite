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
#include <QtWidgets>
#include "tests.h"
#include "ui_tests.h"
#include "blockOb.h"

Tests::Tests( QWidget *cw ) :
    QScrollArea(cw),
    ui(new Ui::Tests)
{ ui->setupUi(this);
  if ( cw )
    { new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
    }
}

Tests::~Tests()
{ delete ui; }

void  Tests::on_start_clicked()
{ ui->report->clear();
  bool pass = true;

  qint32 count = 0;
  QList<RiceyInt> rList = dict.ciByNum.keys();
  foreach ( RiceyInt i, rList )
    { Utf8String name = dict.nameFromCode(i);
        RiceyInt j    = dict.codeFromCodeName( name );
        RiceyInt k    = dict.codeFromCodeName( name );
       RiceyCode code = dict.riceyFromCodeName( name );
      Utf8String cnme = dict.nameFromCode( code );
            bool cc1  = dict.codesContainName( name );
            bool cc2  = dict.codesContainCode( i );
            bool cc3  = dict.codesContainCode( code );
            bool tp   = true;
      if (( j != i ) || ( k != i ) || !cc1 || !cc2 || !cc3 ||
          ( riceToInt(code) != i ) || ( cnme != name ) ||
          ( intToRice(i) != code ))
        pass = tp = false;
      if ( tp )
        count++;
       else
        ui->report->append( QString( "FAIL i %1 j %2 k %3 cc1 %4 cc2 %5 cc3 %6 code %7 name %8 cnme %9 rti %10 itr %11" )
                               .arg(i).arg(j).arg(k).arg(cc1).arg(cc2).arg(cc3)
                               .arg(QString::fromUtf8(code.toHex()))
                               .arg(QString::fromUtf8(name))
                               .arg(QString::fromUtf8(cnme))
                               .arg(riceToInt(code))
                               .arg(QString::fromUtf8(intToRice(i).toHex()))
                            );
    }
  if ( pass )
    ui->report->append( QString("Pass dictionary self consistency %1 entries.").arg(count) );


}
