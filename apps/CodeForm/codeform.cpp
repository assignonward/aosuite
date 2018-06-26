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
#include "bytecodedef.h"
#include "codeform.h"

extern ByteCodeDefinitions bcds;

CodeForm::CodeForm( QWidget *cw, MainWinCommon *mw ) :
    QScrollArea(cw),
    ui(new Ui::CodeForm)
{
    ui->setupUi(this);
    new QVBoxLayout( cw );
    cw->layout()->addWidget( this );

    (void)mw;
    /*
    if ( mw )
      { connect( mw, SIGNAL(restoringConfig()), SLOT(restoreConfig()));
        connect( mw, SIGNAL(   savingConfig()), SLOT(   saveConfig()));
      }
    */

    ByteCodeDefinitions bcds;
    QMap<typeCode_t,QString>codeNames;
    foreach( ByteCodeDef bcd, bcds.bcdList )
      { ui->bytecodeDefinitions->appendPlainText( bcd.toDefine() );
        if (( bcd.sz >= 0 ) || ( bcd.sz == -2 )) ui->typeSizeTable->appendPlainText( bcd.toSizeCase() );
        ui->copyDataItem->appendPlainText( bcd.toCaseDataItem() );
        ui->copyDataItemFromBytes->appendPlainText( bcd.toCase() );

        // Ensure that the separable bit is properly implemented
        if ( bcd.sepr )
          { if (( bcd.code & AO_SEPARABLE_TYPE ) == 0)
              qDebug( "%s typecode %x indicates not separable but the separable flag is set.", qPrintable( bcd.pdef ), bcd.code );
          }
         else
          { if (( bcd.code & AO_SEPARABLE_TYPE ) != 0)
              qDebug( "%s typecode %x indicates separable but the separable flag is not set.", qPrintable( bcd.pdef ), bcd.code );
          }

        // Check for duplicates
        if ( codeNames.contains( bcd.code ) )
          qDebug( "%s already defined %d, %s also trying to use it.", qPrintable( codeNames[bcd.code] ), bcd.code, qPrintable( bcd.pdef ) );
         else
          codeNames.insert( bcd.code, bcd.pdef );
      }
    ui->json->appendPlainText( bcds.toString() );
}

CodeForm::~CodeForm()
{
    delete ui;
}

void CodeForm::on_massage_clicked()
{ qDebug( "Massage clicked" );

  ui->json->clear();

  QMap<typeCode_t,QString>codeNames;
  ByteCodeDefinitions bcds;
  ByteCodeDefinitions bcdn("");
  foreach( ByteCodeDef bcd, bcds.bcdList )
    { bool done = false;
      while ( !done )
        { done = true;
          if ( bcd.sepr )
            bcd.code |= AO_SEPARABLE_TYPE;
           else
            bcd.code &= ~(AO_SEPARABLE_TYPE);

          if ( !codeNames.contains( bcd.code ) )
            codeNames.insert( bcd.code, bcd.pdef );
           else
            { done = false;
              while ( codeNames.contains( bcd.code ) )
               bcd.code++;
              qDebug( "found new code %d for %s", bcd.code, qPrintable( bcd.pdef ) );
            }
        }
      bcdn.bcdList.append( bcd );
    }
  ui->json->appendPlainText( bcdn.toString() );
}

