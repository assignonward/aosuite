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
#include "random.h"
#include <QTextStream>

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
        if ( bcd.code != AO_UNDEFINED_DATAITEM )
          ui->copyDataItemFromBytes->appendPlainText( bcd.toCase() );

        // Ensure that the separable bit is properly implemented
        if ( bcd.sepr )
          { if ( !DataItem::typeCodeIsSeparable( bcd.code ) )
              qDebug( "%s typecode %lld indicates not separable but the separable flag is set.", qPrintable( bcd.acry ), bcd.code );
          }
         else
          { if ( DataItem::typeCodeIsSeparable( bcd.code ) )
              qDebug( "%s typecode %lld indicates separable but the separable flag is not set.", qPrintable( bcd.acry ), bcd.code );
          }

        // Check for duplicates
        if ( codeNames.contains( bcd.code ) )
          qDebug( "%s already defined %lld, %s also trying to use it.", qPrintable( codeNames[bcd.code] ), bcd.code, qPrintable( bcd.acry ) );
         else
          codeNames.insert( bcd.code, bcd.acry );
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
          if ( bcd.sepr != DataItem::typeCodeIsSeparable( bcd.code ) )
            { if ( rng.rnd_bool() )
                bcd.code ^= AO_SEPARABLE_TYPE1;
               else
                bcd.code ^= AO_SEPARABLE_TYPE2;
            }

          if ( !codeNames.contains( bcd.code ) )
            codeNames.insert( bcd.code, bcd.acry );
           else
            { done = false;
              while ( codeNames.contains( bcd.code ) )
               bcd.code++;
              qDebug( "found new code %lld for %s", bcd.code, qPrintable( bcd.acry ) );
            }
        }
      bcdn.bcdList.append( bcd );
    }
  ui->json->appendPlainText( bcdn.toString() );
}

void  CodeForm::on_writeFiles_clicked()
{ QFile file( ui->projectRoot->text() + "aosuite/apps/DataItems/dataitem-auto.h" );
  file.open( QIODevice::WriteOnly );
  QTextStream ts( &file );
  ts << ui->bytecodeDefinitions->toPlainText();

  file.close();
  file.setFileName( ui->projectRoot->text() + "aosuite/apps/DataItems/dataitem-auto.cpp" );
  file.open( QIODevice::WriteOnly );
  ts << QString( ui->cppShell->toPlainText() )
           .arg( ui->copyDataItemFromBytes->toPlainText() )
           .arg( ui->copyDataItem->toPlainText() )
           .arg( ui->typeSizeTable->toPlainText() );

  ui->writeFiles->setText( "Files Written" );
}


void CodeForm::on_vbcHex_textChanged( QString hex )
{ qint32 sz;
  QByteArray hb = QByteArray::fromHex( hex.toUtf8() );
  QString hs = QString::fromUtf8( hb.toHex(' ') );
  typeCode_t tc = VarSizeCode::bytesToCode( hb, sz );
  ui->intCode->setText( QString::number( tc ) );
  QByteArray ba = VarSizeCode::codeToBytes( tc );
  ui->encodedInt->setText( QString::fromUtf8( ba.toHex(' ') ) );
  if ( hs != ui->encodedInt->text() )
    hs.prepend( "!" );
  ui->hexString->setText( hs );
  ui->separable->setText( DataItem::typeCodeIsSeparable( tc ) ? "separable" : "not separable" );
}


