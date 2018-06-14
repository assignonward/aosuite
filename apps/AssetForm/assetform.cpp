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
#include "assetform.h"
#include "sharesref.h"
#include <QSettings>

AssetForm::AssetForm( QWidget *cw, MainWinCommon *mw ) :
    QScrollArea(cw),
    ui(new Ui::AssetForm)
{ assets.setTypeCode( AO_ASSETS );
  ui->setupUi(this);
  new QVBoxLayout( cw );
  cw->layout()->addWidget( this );

  if ( mw )
    { connect( mw, SIGNAL(restoringConfig()), SLOT(restoreConfig()));
      connect( mw, SIGNAL(   savingConfig()), SLOT(   saveConfig()));
    }
}

AssetForm::~AssetForm()
{ delete ui; }

/**
 * @brief AssetForm::restoreConfig - presently a very quick and dirty recording
 *   of the assets.  Might at least want to put some security on them.
 */
void  AssetForm::restoreConfig()
{ QSettings s;
  if ( s.contains( "assets" ) )
    assets = s.value( "assets" ).toByteArray();
  updateLabels();
}

void  AssetForm::saveConfig()
{ QSettings s;
  s.setValue( "assets", assets.toDataItem() );
}

void  AssetForm::updateLabels()
{ int unused             = 0;
  int receiptNegotiating = 0;
  int receiptRecording   = 0;
  int controlShares      = 0;
  int sharesAssigned     = 0;
  int assignmentPending  = 0;
  int sharesEscrowed     = 0;
  QMapIterator DataItemMap_t it( assets.mmap() );
  while ( it.hasNext() )
    { it.next();
      DataItem *di = it.value();
      if ( di->getTypeCode() == AO_KEY_ASSET )
        { GenericCollection *ka = qobject_cast<GenericCollection *>(di);
          if ( ka->contains( AO_KEYPAIR ) )
            { if ( ka->contains( AO_SHARES_REF ) )
                { SharesRef *sr = qobject_cast<SharesRef *>(DataItem::fromDataItem( ka->value(AO_SHARES_REF) ));
                  switch ( sr->shareState.value() )
                    { case KEYS_UNUSED:              unused++;             break;
                      case KEYS_RECEIPT_NEGOTIATING: receiptNegotiating++; break;
                      case KEYS_RECEIPT_RECORDING:   receiptRecording++;   break;
                      case KEYS_CONTROL_SHARES:      controlShares++;      break;
                      case KEYS_SHARES_ASSIGNED:     sharesAssigned++;     break;
                      case KEYS_ASSIGNMENT_PENDING:  assignmentPending++;  break;
                      case KEYS_SHARES_ESCROWED:     sharesEscrowed++;     break;
                      default: qDebug( "unexpected shareState value: %d", sr->shareState.value() );
                    }
                }
               else
                unused++;  // No AO_SHARES_REF implies that the keypair is unused
            }
           else
            { qDebug( "unexpected: AO_KEY_ASSET with no AO_KEYPAIR" ); }
        }
       else
        { qDebug( "TODO: handle additional typecode 0x%x", di->getTypeCode() ); }
    }
  ui->keysUnused            ->setText( QString::number( unused             ) );
  ui->keysReceiptNegotiating->setText( QString::number( receiptNegotiating ) );
  ui->keysReceiptRecording  ->setText( QString::number( receiptRecording   ) );
  ui->keysControlShares     ->setText( QString::number( controlShares      ) );
  ui->keysSharesAssigned    ->setText( QString::number( sharesAssigned     ) );
  ui->keysAssignmentPending ->setText( QString::number( assignmentPending  ) );
  ui->keysSharesEscrowed    ->setText( QString::number( sharesEscrowed     ) );
}
