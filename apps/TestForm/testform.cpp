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
#include "testform.h"
#include "random.h"
#include "datavarlength.h"

TestForm::TestForm( CryptoForm *cp, QWidget *cw, MainWinCommon *mw ) :
    QScrollArea(cw),
    cf(cp),
    ui(new Ui::TestForm)
{ (void)mw;
  ui->setupUi(this);
  new QVBoxLayout( cw );
  cw->layout()->addWidget( this );

  /*
    if ( mw )
      { connect( mw, SIGNAL(restoringConfig()), SLOT(restoreConfig()));
        connect( mw, SIGNAL(   savingConfig()), SLOT(   saveConfig()));
      }
  */
}

TestForm::~TestForm()
{ delete ui;
}

void TestForm::on_test_clicked()
{ testGenesisBlock();
}

void TestForm::on_generateKey_clicked()
{ if ( cf )
    cf->makeNewPair( (ui->keyType->currentText() == "ECDSA256") ? AO_ECDSA_PRI_KEY : AO_RSA3072_PRI_KEY );
}

void TestForm::on_randomName_clicked()
{ QStringList nameEmail = rng.rnd_nameAndEmail();
  if ( nameEmail.size() < 2 )
    qDebug( "FAIL: rnd_nameAndEmail() did not return two strings" );
   else
    qDebug( "%s %s",qPrintable( nameEmail.at(0) ),qPrintable( nameEmail.at(1) ) );
}

#include "aotime.h"
#include "aocoins.h"
#include "data16.h"
#include "genericcollection.h"
#include "shares.h"
void  TestForm::testGenesisBlock()
{ GenericCollection gb( GB_GENESIS_BLOCK );
  __int128_t tv;
  gb.insert( GB_PROTOCOL    , new Data16       (      1, GB_PROTOCOL    , &gb ) );
  gb.insert( GB_PROTOCOL_REV, new Data16       (      2, GB_PROTOCOL_REV, &gb ) );
  gb.insert( GB_TEXT_SYMBOL , new DataVarLength( "tSmb", GB_TEXT_SYMBOL , &gb ) );
  gb.insert( GB_DESCRIPTION , new DataVarLength( "Test description string of reasonably long length, exceeding 128 bytes so as to trigger some multi-byte length code action.", GB_DESCRIPTION, &gb ) );
//  gb.add( GB_ICON           , DataByteArray( ) ) // TODO: file reader
//  gb.add( GB_IMAGE          , DataByteArray( ) ) // TODO: file reader
  tv = 1; tv = tv << 86;
  gb.insert( GB_STARTING_SHARES, new Shares( tv, GB_STARTING_SHARES, &gb ) );
  tv = 1; tv = tv << 64; tv = tv * 600.1;
  gb.insert( GB_MIN_BLOCK_INT  , new AOTime( tv, GB_MIN_BLOCK_INT  , &gb ) );
  tv = 1; tv = tv << (33 + 64);
  gb.insert( GB_N_COINS_TOTAL  , new AOCoins( tv, GB_N_COINS_TOTAL, &gb ) );
  tv = 1; tv = tv << (-30 + 64);
  gb.insert( GB_RECORDING_TAX  , new AOCoins( tv, GB_RECORDING_TAX, &gb ) );
  gb.testHashVerify();
}
