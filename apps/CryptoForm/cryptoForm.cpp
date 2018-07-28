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
#include "cryptoForm.h"
#include <QFileDialog>
#include "Key.h"
#include "random.h"
#include "gcrypt.h"

CryptoForm::CryptoForm( QWidget *cw, MainWinCommon *mw, CryptoEngine *ice ) :
    QScrollArea(cw),
    ui(new Ui::CryptoForm)
{ if ( ice )
    ce = ice;
   else
    ce = new CryptoEngine( this );
  ui->setupUi(this);
  new QVBoxLayout( cw );
  cw->layout()->addWidget( this );
  if ( mw )
    { connect( mw, SIGNAL(restoringConfig()), SLOT(restoreConfig()));
      connect( mw, SIGNAL(   savingConfig()), SLOT(   saveConfig()));
    }
  getGpgInfo();
  on_reread_clicked();
}

CryptoForm::~CryptoForm()
{ delete ui; }

void  CryptoForm::restoreConfig()
{ QSettings s;
  if ( s.contains( "homeFolder" ) )
    { ce->configFolder = s.value( "homeFolder" ).toString();
      ui->homeFolder->setText( ce->configFolder );
    }
  if ( s.contains( "passphrase" ) )
    { ce->setPassphrase( s.value( "passphrase" ).toString() );
      ui->passphrase->setText( ce->passphrase );
    }
}

void  CryptoForm::saveConfig()
{ QSettings s;
  s.setValue( "homeFolder", ce->configFolder );
  s.setValue( "passphrase", ce->passphrase   );
}

void CryptoForm::on_reread_clicked()
{ ui->selectedKeyNumber   ->setValue(1);
  ui->selectedSubkeyNumber->setValue(1);
  getKeyInfo();
}

void  CryptoForm::on_homeFolder_textEdited( QString hf )
{ ce->configFolder = hf; }

void  CryptoForm::on_passphrase_textEdited( QString pp )
{ QByteArray ppa = pp.toUtf8();
  QCryptographicHash h( QCryptographicHash::Sha256 );
  h.addData( ppa );
  QByteArray ha = h.result();
  QString hs = ha.toBase64();
  ui->hashed->setText( hs.left( 16 ) );
  ce->setPassphrase( pp );
}


/**
 * @brief CryptoForm::getKeyInfo - count the keys and store them for individual display
 * @return false if failed somewhere
 */
bool CryptoForm::getKeyInfo()
{ qint32 n = ce->getKeyInfo();
  if ( n < 1 )
    return false;
  ui->selectedKeyNumber->setSuffix( QString( " of %1" ).arg(n) );
  ui->selectedKeyNumber->setMinimum( 1 );
  ui->selectedKeyNumber->setMaximum( n );

  on_selectedKeyNumber_valueChanged( ui->selectedKeyNumber->value() );
  return true;
}

/**
 * @brief CryptoForm::on_selectedKeyNumber_valueChanged - display info about the selected key
 * @param v - 1 based index of which key to display
 */
void CryptoForm::on_selectedKeyNumber_valueChanged( int v )
{ QString info;
  int n = v-1;
  gpgme_ctx_t   ctx;
  gpgme_error_t err;
  SHOW_IF_GPGERR( gpgme_new( &ctx ) )
  gpgme_data_t keydata;
  gpgme_key_t  key = ce->gpgKeys[n];
  if ( !key )
    return;
  info.append( QString( "id:%1\nname:%2\nemail:%3\ncomment:%4" )
               .arg( key->subkeys->keyid )
               .arg( (key->uids && key->uids->name   ) ? key->uids->name    : "undefined" )
               .arg( (key->uids && key->uids->email  ) ? key->uids->email   : "undefined" )
               .arg( (key->uids && key->uids->comment) ? key->uids->comment : "undefined" )
             );
  ui->selectedKeyInfo->setText( info );

  SHOW_IF_GPGERR( gpgme_data_new(&keydata) )
  gpgme_key_t showKeyList[2];
  showKeyList[0] = key;
  showKeyList[1] = NULL;
  gpgme_data_seek( keydata, 0, SEEK_SET );
  SHOW_IF_GPGERR( gpgme_op_export_keys( ctx, showKeyList, GPGME_EXPORT_MODE_MINIMAL, keydata ) )
  gpgme_data_seek( keydata, 0, SEEK_SET );
  gpgme_data_type_t dt = gpgme_data_identify(keydata,0);
  if ( dt != GPGME_DATA_TYPE_PGP_KEY )
    { ui->openPGPkeyInfo->setText( QString( "Unexpected data type %1" ).arg( dt ) );
    }
   else
    { char buf[131072];
      ssize_t sz = gpgme_data_read( keydata, buf, 131072 );
      info = QString( "Read %1 bytes\n" ).arg( sz );
      QByteArray ba = QByteArray( buf, sz );
      // info.append( ba.toHex() );
      OpenPGP::Key sk( ba.toStdString() );
      info.append( QString::fromStdString( sk.show(1,2) ) );
      ui->openPGPkeyInfo->setText( info );
    }
  gpgme_data_release( keydata );
  gpgme_release( ctx );

  // Count the subkeys
  gpgme_subkey_t subkey = key->subkeys;
  n = 0;
  while ( subkey )
    { n++;
      subkey = subkey->next;
    }
  if ( n > 0 )
    { ui->selectedSubkeyNumber->setSuffix( QString( " of %1" ).arg(n) );
      ui->selectedSubkeyNumber->setMinimum( 1 );
      ui->selectedSubkeyNumber->setMaximum( n );
    }
   else
    { ui->selectedSubkeyNumber->setSuffix( " of none" );
      ui->selectedSubkeyNumber->setMinimum( 0 );
      ui->selectedSubkeyNumber->setMaximum( 0 );
    }
  on_selectedSubkeyNumber_valueChanged( ui->selectedSubkeyNumber->value() );
}

/**
 * @brief CryptoForm::on_selectedSubkeyNumber_valueChanged - show the subkey info
 * @param v - which subkey to show
 */
void CryptoForm::on_selectedSubkeyNumber_valueChanged( int v )
{ if ( v <= 0 )
    { ui->selectedSubkeyInfo->setText( "no subkeys" );
      return;
    }
  int n = ui->selectedKeyNumber->value() - 1;
  if ( n < 0 )
    { ui->selectedSubkeyInfo->setText( "no keys" );
      return;
    }
  if ( !ce->gpgKeys[n] )
    return;
  gpgme_subkey_t subkey = ce->gpgKeys[n]->subkeys;
  if ( !subkey )
    { ui->selectedSubkeyInfo->setText( "subkeys empty" );
      return;
    }
  while ( v > 1 )
    { subkey = subkey->next;
      v--;
      if ( !subkey )
        { ui->selectedSubkeyInfo->setText( QString( "subkey list %d short" ).arg( v ) );
          return;
        }
    }
  QString info;
  info.append( QString( "           keyid: %1\n" ).arg(subkey->keyid            ));
  info.append( QString( "Can authenticate: %1\n" ).arg(subkey->can_authenticate ));
  info.append( QString( "     Can certify: %1\n" ).arg(subkey->can_certify      ));
  info.append( QString( "     Can encrypt: %1\n" ).arg(subkey->can_encrypt      ));
  info.append( QString( "        Can sign: %1\n" ).arg(subkey->can_sign         ));
  info.append( QString( "     Card number: %1\n" ).arg(subkey->card_number      ));
  info.append( QString( "           Curve: %1\n" ).arg(subkey->curve            ));
  info.append( QString( "        Disabled: %1\n" ).arg(subkey->disabled         ));
  info.append( QString( "         Expired: %1\n" ).arg(subkey->expired          ));
  info.append( QString( "         Expires: %1\n" ).arg(subkey->expires          ));
  info.append( QString( "     fingerprint: %1\n" ).arg(subkey->fpr              ));
  info.append( QString( "         invalid: %1\n" ).arg(subkey->invalid          ));
  info.append( QString( "     is card key: %1\n" ).arg(subkey->is_cardkey       ));
  info.append( QString( "        is de vs: %1\n" ).arg(subkey->is_de_vs         ));
  info.append( QString( "    is qualified: %1\n" ).arg(subkey->is_qualified     ));
  info.append( QString( "         keygrip: %1\n" ).arg(subkey->keygrip          ));
  info.append( QString( "          length: %1\n" ).arg(subkey->length           ));
  info.append( QString( "         revoked: %1\n" ).arg(subkey->revoked          ));
  info.append( QString( "          secret: %1\n" ).arg(subkey->secret           ));
  info.append( QString( "       timestamp: %1\n" ).arg(subkey->timestamp        ));
  ui->selectedSubkeyInfo->setText(info);
}


/**
 * @brief CryptoForm::getGpgInfo - populate the form with basic library info
 * @return false if something fails
 */
bool CryptoForm::getGpgInfo()
{ gpgme_ctx_t   ctx;
  gpgme_error_t err;

  FAIL_IF_GPGERR( ce->initGpgme() )
  FAIL_IF_GPGERR( gpgme_new( &ctx ) )
  FAIL_IF_GPGERR( gpgme_set_protocol( ctx, GPGME_PROTOCOL_OpenPGP ) )
  gpgme_engine_info_t ei = gpgme_ctx_get_engine_info( ctx );
  ui->gpgme_version             ->setText( gpgme_check_version( NULL ) );
  ui->engine_protocol           ->setText( gpgme_get_protocol_name( ei->protocol ) );
  ui->engine_filename           ->setText( ei->file_name );
  ui->engine_home_dir           ->setText( ei->home_dir  );
  ui->engine_version            ->setText( ei->version );
  ui->engine_required_version   ->setText( ei->req_version );
  ui->engine_ascii_armor        ->setText( gpgme_get_armor   (ctx) ? "ASCII Armored" : "not Armored" );
  ui->engine_canonical_text_mode->setText( gpgme_get_textmode(ctx) ? "Canonical text mode" : "not text mode" );
  ui->engine_offline_mode       ->setText( gpgme_get_offline (ctx) ? "enabled" : "not enabled" );
  ui->engine_keylist_mode       ->setText( QString( "0x%1" ).arg( gpgme_get_keylist_mode(ctx), 2, 16, QChar('0') ) );
  ui->gcry_config               ->setText( gcry_get_config(0,NULL) );
  gpgme_release( ctx );
  return true;
}

/**
 * @brief CryptoForm::getGpgKeys - a big disorganized dump of the keyring
 * @return false if something fails
 */
bool CryptoForm::getGpgKeys()
{ gpgme_ctx_t   ctx;
  gpgme_error_t err;
  gpgme_data_t  keydata,tempkeydata;

  FAIL_IF_GPGERR( ce->initGpgme() )
  FAIL_IF_GPGERR( gpgme_new( &ctx ) )
  FAIL_IF_GPGERR( gpgme_set_protocol( ctx, GPGME_PROTOCOL_OpenPGP ) )
  // gpgme_set_armor( ctx, 1 );

  FAIL_IF_GPGERR( gpgme_data_new(&keydata) )
  FAIL_IF_GPGERR( gpgme_op_keylist_start( ctx, NULL, 0 ) )

  int maxLoops = 3;
  int n = 0;
  bool done = false;
  while ( !done )
    { qDebug( "Getting key %d", n );
      err = gpgme_op_keylist_next( ctx, &(ce->gpgKeys[n]) );
      if ( gpg_err_code(err) == GPG_ERR_EOF )
        done = true;
       else
        { FAIL_IF_GPGERR(err)
          gpgme_key_t key = ce->gpgKeys[n];
          qDebug( "%s", qPrintable( QString( " got key %1 id:%2 name:%3 email:%4" )
                       .arg( n, 4 ).arg( key->subkeys->keyid )
                       .arg( (key->uids && key->uids->name ) ? key->uids->name  : "undefined" )
                       .arg( (key->uids && key->uids->email) ? key->uids->email : "undefined" ) ) );
          gpgme_subkey_t subkey = key->subkeys;
          err = 0;
          while ( subkey && !err )
            { qDebug( "Examining subkey:%s", subkey->keyid );
              subkey = subkey->next;
            }

          if ( ++n >= MAX_KEYS )
            { qDebug( "ran out of GPG keys storage, coded max of %d, looping.", MAX_KEYS );
              n = 0;
              if ( maxLoops-- <= 0 )
                { qDebug( "too many keys in GPG keyring, stopping." );
                  FAIL_IF_GPGERR( gpgme_op_keylist_end( ctx ) )
                  done = true;
                }
            }
        }
    }
  qDebug( "%d keys found", n );

  qDebug( "Looking for key Ⓐ" );
  SHOW_IF_GPGERR( gpgme_op_export( ctx, "Ⓐ", GPGME_EXPORT_MODE_MINIMAL, keydata ) )
  gpgme_data_seek( keydata, 0, SEEK_SET );
  gpgme_data_type_t dt = gpgme_data_identify(keydata,0);
  qDebug( "keydata type %d %s", dt, dt == GPGME_DATA_TYPE_PGP_KEY ? "That's a PGP KEY" : "unexpected type" );
  char buf[8192];
  ssize_t sz = gpgme_data_read( keydata, buf, 8192 );
  qDebug( "Read %ld bytes", sz );
  QByteArray ba( buf, sz );
  qDebug( "%s", qPrintable( QString::fromUtf8( ba.toHex() ) ) );

  qDebug( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );

  OpenPGP::Key ks( ba.toStdString() );

  qDebug( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );

  qDebug( "ks.show():%s", qPrintable( QString::fromStdString( ks.show() ) ) );

  qDebug( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );

  gpgme_data_seek( keydata, 0, SEEK_SET );
  SHOW_IF_GPGERR( gpgme_op_export_keys( ctx, ce->gpgKeys, GPGME_EXPORT_MODE_MINIMAL | GPGME_EXPORT_MODE_SECRET, keydata ) )
  gpgme_data_seek( keydata, 0, SEEK_SET );
  dt = gpgme_data_identify(keydata,0);
  qDebug( "keydata type %d %s", dt, dt == GPGME_DATA_TYPE_PGP_KEY ? "That's a PGP KEY" : "unexpected type" );
  sz = gpgme_data_read( keydata, buf, 8192 );
  qDebug( "Read %ld bytes", sz );
//  QByteArray ba2( buf, sz );
//  qDebug( "%s", qPrintable( QString::fromUtf8( ba2.toHex() ) ) );

  n = 0;
  done = false;
  while ( !done )
    { gpgme_key_t key = ce->gpgKeys[n];
      if ( !key )
        done = true;
       else
        { qDebug( "%s", qPrintable( QString( " got key %1 id: %2 name:%3 email:%4" )
               .arg( n, 4 ).arg( key->subkeys->keyid )
               .arg( (key->uids && key->uids->name ) ? key->uids->name  : "undefined" )
               .arg( (key->uids && key->uids->email) ? key->uids->email : "undefined" ) ) );

          FAIL_IF_GPGERR( gpgme_data_new(&tempkeydata) )
          gpgme_key_t showKeyList[2];
          showKeyList[0] = key;
          showKeyList[1] = NULL;
          gpgme_data_seek( keydata, 0, SEEK_SET );
          SHOW_IF_GPGERR( gpgme_op_export_keys( ctx, showKeyList, GPGME_EXPORT_MODE_MINIMAL, tempkeydata ) )
          gpgme_data_seek( tempkeydata, 0, SEEK_SET );
          dt = gpgme_data_identify(tempkeydata,0);
          qDebug( "keydata type %d %s", dt, dt == GPGME_DATA_TYPE_PGP_KEY ? "That's a PGP KEY" : "unexpected type" );
          ssize_t sz = gpgme_data_read( tempkeydata, buf, 8192 );
          qDebug( "Read %ld bytes", sz );
          ba = QByteArray( buf, sz );
          qDebug( "%s", qPrintable( QString::fromUtf8( ba.toHex() ) ) );
          OpenPGP::Key sk( ba.toStdString() );
          qDebug( "sk.show():%s", qPrintable( QString::fromStdString( sk.show() ) ) );
          gpgme_data_release( tempkeydata );

          gpgme_subkey_t subkey = key->subkeys;
          err = 0;
          while ( subkey && !err )
            { qDebug( "Examining subkey:" );
              qDebug( "           keyid: %s", subkey->keyid            );
              qDebug( "Can authenticate: %d", subkey->can_authenticate );
              qDebug( "     Can certify: %d", subkey->can_certify      );
              qDebug( "     Can encrypt: %d", subkey->can_encrypt      );
              qDebug( "        Can sign: %d", subkey->can_sign         );
              qDebug( "     Card number: %s", subkey->card_number      );
              qDebug( "           Curve: %s", subkey->curve            );
              qDebug( "        Disabled: %d", subkey->disabled         );
              qDebug( "         Expired: %d", subkey->expired          );
              qDebug( "         Expires: %ld",subkey->expires          );
              qDebug( "     fingerprint: %s", subkey->fpr              );
              qDebug( "         invalid: %d", subkey->invalid          );
              qDebug( "     is card key: %d", subkey->is_cardkey       );
              qDebug( "        is de vs: %d", subkey->is_de_vs         );
              qDebug( "    is qualified: %d", subkey->is_qualified     );
              qDebug( "         keygrip: %s", subkey->keygrip          );
              qDebug( "          length: %d", subkey->length           );
              qDebug( "         revoked: %d", subkey->revoked          );
              qDebug( "          secret: %d", subkey->secret           );
              qDebug( "       timestamp: %ld",subkey->timestamp        );
              subkey = subkey->next;
            }
          n++;
        }
    }

  gpgme_data_release( keydata );
  gpgme_release( ctx );
  return true;
}
