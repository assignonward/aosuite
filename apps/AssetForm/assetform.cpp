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
#include "prikey.h"
#include "sharesref.h"
#include <QSettings>
#include <QPointer>

AssetForm::AssetForm( QWidget *cw, MainWinCommon *mw, CryptoEngine *ice, AssetsEngine *iae ) :
    QScrollArea(cw),
    ui(new Ui::AssetForm)
{ if ( ice ) ce = ice; else
    { ce = new CryptoEngine( this );
      qDebug( "AssetForm() creating local CryptoEngine" );
    }
  if ( iae ) ae = iae; else
    { ae = new AssetsEngine( ce, this );
      qDebug( "AssetForm() creating local AssetsEngine" );
    }
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
{ ae->restoreConfig();
  updateLabels();
}

void  AssetForm::saveConfig()
{ ae->saveConfig();
}

void  AssetForm::updateLabels()
{ int unused             = 0;
  int receiptNegotiating = 0;
  int receiptRecording   = 0;
  int controlShares      = 0;
  int sharesAssigned     = 0;
  int assignmentPending  = 0;
  int sharesEscrowed     = 0;
  QList<DataItem *>dipl = ae->mmap().values();
  // qDebug( "reading assets.mmap() size %d", assets.mmap().size() );

  foreach( DataItem *di, dipl )
    { // qDebug( "item (of %d) type 0x%x", assets.itemMM.size(), di->getTypeCode() );
      if ( di->getTypeCode() == AO_KEY_ASSET )
        { KeyAsset *ka = qobject_cast<KeyAsset *>(di);
          if ( !ka )
            { qDebug( "AO_KEY_ASSET did not qobject_cast to a KeyAsset" ); }
           else
            { // qDebug( "ka typeCode 0x%x %d items", ka->getTypeCode(), ka->itemMM.size() );
              if ( ka->contains( AO_KEYPAIR ) )
                { if ( ka->contains( AO_SHARES_REF ) )
                    { SharesRef *sr = qobject_cast<SharesRef *>(DataItem::fromDataItem( ka->value(AO_SHARES_REF) ));
                      switch ( sr->shareState.value() )
                        { case KEYS_UNUSED:              unused++;             break;  // Can get here for instance when a negotiation falls through
                          case KEYS_RECEIPT_NEGOTIATING: receiptNegotiating++; break;
                          case KEYS_RECEIPT_RECORDING:   receiptRecording++;   break;
                          case KEYS_CONTROL_SHARES:      controlShares++;      break;
                          case KEYS_SHARES_ASSIGNED:     sharesAssigned++;     break;
                          case KEYS_ASSIGNMENT_PENDING:  assignmentPending++;  break;
                          case KEYS_SHARES_ESCROWED:     sharesEscrowed++;     break;
                          default: qDebug( "unexpected shareState value: %lld", sr->shareState.value() );
                        }
                    }
                   else
                    unused++;  // No AO_SHARES_REF implies that the keypair is unused
                }
               else
                { qDebug( "unexpected: AO_KEY_ASSET with no AO_X_PRI_KEY or AO_KEYPAIR" ); }
            } // else - was a successful qobject_cast
        }
       else
        { qDebug( "TODO: handle additional typecode %lld", di->getTypeCode() ); }
    }
  ui->keysUnused            ->setText( QString::number( unused             ) );
  ui->keysReceiptNegotiating->setText( QString::number( receiptNegotiating ) );
  ui->keysReceiptRecording  ->setText( QString::number( receiptRecording   ) );
  ui->keysControlShares     ->setText( QString::number( controlShares      ) );
  ui->keysSharesAssigned    ->setText( QString::number( sharesAssigned     ) );
  ui->keysAssignmentPending ->setText( QString::number( assignmentPending  ) );
  ui->keysSharesEscrowed    ->setText( QString::number( sharesEscrowed     ) );
}

void  AssetForm::on_makeNewKey_clicked()
{ ui->keyAssetOperationLog->appendPlainText( "makeNewKey" );
  if ( !ae ) { qDebug( "AssetsEngine pointer is NULL" ); return; }
  typeCode_t keyType = (ui->keyType->currentText() != "ECDSA256") ? AO_RSA3072_PRI_KEY : AO_ECDSA_PRI_KEY;
  GenericCollection *kp = ae->getNewKeyPair( keyType );
  updateLabels();
  if ( kp )
    ui->keyAssetOperationLog->appendPlainText( QString::fromUtf8( kp->toDataItem().toHex() ) );
  saveConfig();
}

void  AssetForm::on_importToGpg_clicked()
{ QList<DataItem *>dipl = ae->mmap().values();
  qDebug( "reading assets.mmap() size %d", ae->mmap().size() );
  foreach( DataItem *di, dipl )
    { // qDebug( "item (of %d) type 0x%x", assets.itemMM.size(), di->getTypeCode() );
      if ( di->getTypeCode() == AO_KEY_ASSET )
        { KeyAsset *ka = qobject_cast<KeyAsset *>(di);
          if ( !ka )
            { qDebug( "AO_KEY_ASSET did not qobject_cast to a KeyAsset" ); }
           else
            { // qDebug( "ka typeCode 0x%x %d items", ka->getTypeCode(), ka->itemMM.size() );
              if ( ka->contains( AO_ECDSA_PRI_KEY ) ||
                   ka->contains( AO_RSA3072_PRI_KEY ) )
                { bool proceed = true;
                  if ( ka->contains( AO_SHARES_REF ) )
                    { SharesRef *sr = qobject_cast<SharesRef *>(DataItem::fromDataItem( ka->value(AO_SHARES_REF) ));
                      if ( sr->shareState.value() != KEYS_UNUSED )
                        proceed = false;
                    }
                  if ( proceed )
                    { DataItem *pdi;
                      QByteArray ba;
                      if ( ka->contains( AO_ECDSA_PRI_KEY ) )
                        { pdi = ka->value( AO_ECDSA_PRI_KEY );
                          PrivateKeyEcdsa *pk = qobject_cast<PrivateKeyEcdsa *>(pdi);
                          if ( pk )
                            ba = pk->get();
                           else
                            qDebug( "qobject_cast<PrivateKeyEcdsa *> returned NULL" );
                        }
                       else
                        { pdi = ka->value( AO_RSA3072_PRI_KEY );
                          PrivateKeyRsa3072 *pk = qobject_cast<PrivateKeyRsa3072 *>(pdi);
                          if ( pk )
                            ba = pk->get();
                           else
                            qDebug( "qobject_cast<PrivateKeyRsa3072 *> returned NULL" );
                        }
                      if ( ba.size() > 0 )
                        { if ( !ce->importKey( ba ) )
                            qDebug( "importKey failed" );
                        }
                    } // if proceed
                }    // ka contains a key
            }       // ka successfully cast to a Generic Collection
        }          // if di is a key asset
    }             // while it hasNext
}                // on_importToGpg_clicked()

