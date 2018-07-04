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
#include "genesisForm.h"
#include <QFileDialog>

GenesisForm::GenesisForm( QWidget *cw, MainWinCommon *mw, AssetsEngine *iae ) :
    QScrollArea(cw),
    ui(new Ui::GenesisForm)
{ if ( iae ) ae = iae; else
    { ae = new AssetsEngine( new CryptoEngine(this), this );
      qDebug( "GenesisForm() creating local asset engine (with embedded local CryptoEngine)" );
    }

  ui->setupUi(this);
  new QVBoxLayout( cw );
  cw->layout()->addWidget( this );
  if ( mw )
    { connect( mw, SIGNAL(restoringConfig()), SLOT(restoreConfig()));
      connect( mw, SIGNAL(   savingConfig()), SLOT(   saveConfig()));
    }
  connect( ui->hashType,       SIGNAL(   currentIndexChanged(int)), SLOT(updateHash()) );
  connect( ui->protocol,       SIGNAL(   currentIndexChanged(int)), SLOT(updateHash()) );
  connect( ui->protocolRev,    SIGNAL(          valueChanged(int)), SLOT(updateHash()) );
  connect( ui->symbol,         SIGNAL(       textChanged(QString)), SLOT(updateHash()) );
  connect( ui->description,    SIGNAL(              textChanged()), SLOT(updateHash()) );
  connect( ui->signingKey,     SIGNAL(currentTextChanged(QString)), SLOT(updateHash()) );
  connect( ui->startingShares, SIGNAL(          valueChanged(int)), SLOT(updateHash()) );
  connect( ui->minBlockTime,   SIGNAL(       valueChanged(double)), SLOT(updateHash()) );
  connect( ui->totalCoins,     SIGNAL(          valueChanged(int)), SLOT(updateHash()) );
  connect( ui->recordingTax,   SIGNAL(          valueChanged(int)), SLOT(updateHash()) );
  connect( ui->hashData,       SIGNAL(              clicked(bool)), SLOT(updateHash()) );
  updateHash();
  connect( ae, SIGNAL(newKeyAdded()), SLOT(updateKeyList()) );
}

GenesisForm::~GenesisForm()
{ delete ui; }

void  GenesisForm::restoreConfig()
{ QSettings s;
  if ( s.contains( "description"    ) ) ui->description   ->setText        ( s.value( "description"    ).toString() );
  if ( s.contains( "protocol"       ) ) ui->protocol      ->setCurrentIndex( s.value( "protocol"       ).toInt()    );
  if ( s.contains( "symbol"         ) ) ui->symbol        ->setText        ( s.value( "symbol"         ).toString() );
  if ( s.contains( "icon"           ) ) ui->icon          ->setText        ( s.value( "icon"           ).toString() );
  if ( s.contains( "image"          ) ) ui->image         ->setText        ( s.value( "image"          ).toString() );
  if ( s.contains( "signingKey"     ) ) ui->signingKey    ->setCurrentText ( s.value( "signingKey"     ).toString() );
  if ( s.contains( "startingShares" ) ) ui->startingShares->setValue       ( s.value( "startingShares" ).toInt()    );
  if ( s.contains( "totalCoins"     ) ) ui->totalCoins    ->setValue       ( s.value( "totalCoins"     ).toInt()    );
  if ( s.contains( "recordingTax"   ) ) ui->recordingTax  ->setValue       ( s.value( "recordingTax"   ).toInt()    );
  if ( s.contains( "minBlockTime"   ) ) ui->minBlockTime  ->setValue       ( s.value( "minBlockTime"   ).toDouble() );
}

void  GenesisForm::saveConfig()
{ QSettings s;
  s.setValue( "description"   , ui->description   ->toPlainText()  );
  s.setValue( "protocol"      , ui->protocol      ->currentIndex() );
  s.setValue( "symbol"        , ui->symbol        ->text()         );
  s.setValue( "icon"          , ui->icon          ->text()         );
  s.setValue( "image"         , ui->image         ->text()         );
  s.setValue( "signingKey"    , ui->signingKey    ->currentText()  );
  s.setValue( "startingShares", ui->startingShares->value()        );
  s.setValue( "totalCoins"    , ui->totalCoins    ->value()        );
  s.setValue( "recordingTax"  , ui->recordingTax  ->value()        );
  s.setValue( "minBlockTime"  , ui->minBlockTime  ->value()        );
}

void  GenesisForm::on_chooseIcon_clicked()
{ QString name = QFileDialog::getOpenFileName( this, "Icon file" );
  if ( name.size() > 0 )
    ui->icon->setText( name );
}

void  GenesisForm::on_chooseImage_clicked()
{ QString name = QFileDialog::getOpenFileName( this, "Image file" );
  if ( name.size() > 0 )
    ui->image->setText( name );
}

void  GenesisForm::on_importGenesisBlock_clicked()
{

}

#include "aocoins.h"
#include "aotime.h"
#include "authorization.h"
#include "databytearray.h"
#include "data64.h"
#include "genericcollection.h"
#include "shares.h"

/**
 * @brief GenesisForm::on_publishGenesisBlock_clicked - saving to file at the moment
 *   when we have a recorder to talk to, can communicate it directly to the recorder.
 */
void  GenesisForm::on_publishGenesisBlock_clicked()
{ QString name = QFileDialog::getSaveFileName( this, "save Genesis Block to file:" );
  if ( name.size() < 1 )
    return;
  GenericCollection *gb = calculateGenesisBlock();
  QFile file( name );
  if ( !file.open( QIODevice::WriteOnly ) )
    { qDebug() << gb->toDataItem();
      return;
    }
  file.write( ui->hashData->isChecked() ? gb->toHashData() : gb->toDataItem() );
  gb->deleteLater();
}

GenericCollection *GenesisForm::calculateGenesisBlock()
{ GenericCollection *gb = new GenericCollection( GB_GENESIS_BLOCK );
  __int128_t tv;
  gb->insert( new DataVbc64    ( ui->protocol   ->currentIndex()        , GB_PROTOCOL,     gb ) );
  gb->insert( new DataVbc64    ( ui->protocolRev->value()               , GB_PROTOCOL_REV, gb ) );
  gb->insert( new DataVarLength( ui->symbol     ->text().toUtf8()       , GB_TEXT_SYMBOL,  gb ) );
  if ( ui->description->toPlainText().size() > 0 )
  gb->insert( new DataVarLength( ui->description->toPlainText().toUtf8(), GB_DESCRIPTION,  gb ) );
  //  gb->add( GB_ICON           , DataByteArray( ) ) // TODO: file reader
  //  gb->add( GB_IMAGE          , DataByteArray( ) ) // TODO: file reader
  tv = 1; tv = tv << ui->startingShares->value();
  gb->insert( new Shares   ( tv, GB_STARTING_SHARES, gb ) );
  tv = 1; tv = tv << 64; tv = tv * ui->minBlockTime->value();
  gb->insert( new AOTime   ( tv, GB_MIN_BLOCK_INT  , gb ) );
  tv = 1; tv = tv << (ui->totalCoins->value() + 64);
  gb->insert( new AOCoins  ( tv, GB_N_COINS_TOTAL  , gb ) );
  tv = 1; tv = tv << (ui->recordingTax->value() + 64);
  gb->insert( new AOCoins  ( tv, GB_RECORDING_TAX  , gb ) );
  gb->insert( new DataVbc64( 0, CB_FIRST_ID_SEQ_NUM, gb ) );
  gb->insert( new DataVbc64( 1, CB_N_ID_SEQ_NUM    , gb ) );

  Authorization *auth = new Authorization( gb );
  Assignment    *asgn = new Assignment( gb );
  Participant   *part = new Participant( gb );
  QByteArray pubKeyBA = QByteArray::fromHex( ui->signingKey->currentText().toUtf8() );

  part->setIndex( 0 );
  DataItem *di = ae->getUnusedKeyPair( pubKeyBA );
  if ( di )
    { KeyAsset *ka = qobject_cast<KeyAsset *>(di);
      if ( ka )
        if ( ka->getTypeCode() == AO_KEY_ASSET )
          { di = ka->value( AO_KEYPAIR );
            if ( di )
              { KeyPair *kp = qobject_cast<KeyPair *>(di);
                if ( kp )
                  { PubKey *pk = kp->getPubKey();
                    if ( pk )
                      part->setKey( new PubKey( pk->toDataItem() ) );
                  }
              }
          }
    }
  tv = 1; tv = tv << ui->startingShares->value();
  part->setAmount( tv );
  asgn->append( *part );
  auth->setAssignment( asgn );
  auth->setNSigs( asgn->getNParticipants() );
  // auth.sigs.append( signature of assignment );
  // gb->insert( auth );
  return gb;
}

void GenesisForm::updateHash()
{ GenericCollection *gb = calculateGenesisBlock();
  typeCode_t ht = AO_HASH256;
  switch( ui->hashType->currentIndex() )
    { case 0: ht = AO_HASH256; break;
      case 1: ht = AO_HASH512; break;
      case 2: ht = AO_HASH224SALT32; break;
    }
  ui->hash->setPlainText( QString::fromUtf8( gb->getHash(ht).toHex() ) );
  ui->block->setPlainText( QString::fromUtf8( ui->hashData->isChecked() ? gb->toHashData().toHex() : gb->toDataItem().toHex() ) );
  gb->deleteLater();
}

/**
 * @brief GenesisForm::updateKeyList - get a new list of unused key pair ids from the
 *   asset engine and update the signingKey combo box to show them.
 */
void GenesisForm::updateKeyList()
{ QString cur = ui->signingKey->currentText();
  ui->signingKey->clear();
  if ( !ae ) return;
  QByteArrayList kl = ae->getUnusedKeyPairIDs();
  std::sort( kl.begin(), kl.end() );
  QStringList klh;
  foreach( QByteArray pkid, kl )
    klh.append( QString::fromUtf8( pkid.toHex() ) );
  ui->signingKey->addItems( klh );
  ui->signingKey->setCurrentText( cur );
}
