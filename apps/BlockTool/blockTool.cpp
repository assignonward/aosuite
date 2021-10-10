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
#include "blockTool.h"
#include "ui_blockTool.h"
#include "blockOb.h"
#include <unistd.h>

BlockTool::BlockTool( QWidget *cw ) :
    QScrollArea(cw),
    ui(new Ui::BlockTool)
{ ui->setupUi(this);
  if ( cw )
    { QVBoxLayout *vb = new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      vb->setContentsMargins( 0,0,0,0 );
    }
  panelA = new BlockPanel( "A", ValueBase::Mode::build, ui->frameA );
  panelX = new BlockPanel( "X", ValueBase::Mode::make , ui->frameX );
  panelY = new BlockPanel( "Y", ValueBase::Mode::idle , ui->frameY );
  connect( this, SIGNAL(showA(KeyValueBase*)), panelA, SLOT(setBlock(KeyValueBase*)));
  connect( this, SIGNAL(showX(KeyValueBase*)), panelX, SLOT(setBlock(KeyValueBase*)));
  connect( this, SIGNAL(showY(KeyValueBase*)), panelY, SLOT(setBlock(KeyValueBase*)));

  sortKeys();
}

BlockTool::~BlockTool()
{ delete ui; }

void  BlockTool::on_set_clicked()
{ Utf8String nKey = ui->key->currentText().toUtf8();
  if ( !dict.codesContainName( nKey ) )
    { qWarning( "key comboBox text returns unknown key: %s", nKey.data() );
      return;
    }
  ValueBase *vbp = nullptr;
  quint8 tp = dict.codeFromCodeName( nKey ) & RDT_OBTYPEMASK;
  if ( tp == RDT_NULL )
    vbp = new BlockValueNull(this);
   else
    { switch ( tp )
        { case RDT_OBJECT   : vbp = new BlockValueObject   ( this ); break;
          case RDT_INT64    : vbp = new BlockValueInt64    ( ui->intEdit      ->text() .toInt(), this ); break;
          case RDT_RCODE    : vbp = new BlockValueRiceyCode( dict.codeFromCodeName( ui->rcodeEdit->currentText().toUtf8() ), this ); break;
          case RDT_MPZ      : vbp = new BlockValueMPZ      ( ui->mpzEdit      ->text().toUtf8(), this ); break;
          case RDT_MPQ      : vbp = new BlockValueMPQ      ( ui->mpqEdit      ->text().toUtf8(), this ); break;
          case RDT_BYTEARRAY: vbp = new BlockValueByteArray( ui->byteArrayEdit->text().toUtf8(), this ); break;
          case RDT_STRING   : vbp = new BlockValueString   ( ui->stringEdit   ->text().toUtf8(), this ); break;
          case RDT_OBJECT_ARRAY   : vbp = new BlockValueObjectArray   ( this ); break;
          case RDT_INT64_ARRAY    : vbp = new BlockValueInt64Array    ( this ); break;
          case RDT_RCODE_ARRAY    : vbp = new BlockValueRiceyCodeArray( this ); break;
          case RDT_MPZ_ARRAY      : vbp = new BlockValueMPZArray      ( this ); break;
          case RDT_MPQ_ARRAY      : vbp = new BlockValueMPQArray      ( this ); break;
          case RDT_BYTEARRAY_ARRAY: vbp = new BlockValueByteArrayArray( this ); break;
          case RDT_STRING_ARRAY   : vbp = new BlockValueStringArray   ( this ); break;
        }
    }
  if ( vbp == nullptr )
    { qWarning( "no value or array established." );
      ui->report->append( "oops" );
      return;
    }
  ui->report->clear();
  ui->report->append( "set clicked" );
  KeyValueBase *kvb = nullptr;
  if ( ( tp & RDT_ARRAY ) == 0 )
    kvb = new KeyValuePair( dict.codeFromCodeName( nKey ), vbp, this );
   else
    kvb = new KeyValueArray( dict.codeFromCodeName( nKey ), (ValueBaseArray *)vbp, this );
  setMake( kvb );
  if ( ui->showJson->isChecked() )ui->report->append( jsonReformat( kvb->json() ) );
  if ( ui->showHex ->isChecked() )ui->report->append( kvb->bsonish().toHex() );
  if ( ui->showDot ->isChecked() )ui->report->append( kvb->dot(ValueBase::Mode::idle) ); // TODO: get the current mode from BlockPanel
  // kvp->deleteLater(); child of vbp, no need to delete this
  vbp->deleteLater();
}


void  BlockTool::on_key_currentTextChanged( const QString &nuKey )
{ Utf8String nKey = nuKey.toUtf8();
  if ( !dict.codesContainName( nKey ) )
    { qWarning( "key comboBox selected unknown key: %s", nKey.data() );
      return;
    }
  quint8 tp = dict.codeFromCodeName( nKey ) & RDT_OBTYPEMASK;
  switch( tp )
    { case RDT_NULL           : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_null      )); break;
      case RDT_OBJECT         : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_object    )); break;
      case RDT_INT64          : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_int       )); break;
      case RDT_RCODE          : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_rcode     )); break;
      case RDT_MPZ            : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_mpz       )); break;
      case RDT_MPQ            : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_mpq       )); break;
      case RDT_BYTEARRAY      : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_byteArray )); break;
      case RDT_STRING         : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_string    )); break;
      case RDT_OBJECT_ARRAY   :
      case RDT_INT64_ARRAY    :
      case RDT_RCODE_ARRAY    :
      case RDT_MPZ_ARRAY      :
      case RDT_MPQ_ARRAY      :
      case RDT_BYTEARRAY_ARRAY:
      case RDT_STRING_ARRAY   : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_array     )); break;
      default: qWarning( "key comboBox selected unhandled type %d key: %s", tp, nKey.data() );
    }
}

void  BlockTool::on_sortName_toggled(bool) { sortKeys(); }
void  BlockTool::on_sortId_toggled  (bool) { sortKeys(); }
void  BlockTool::on_sortDict_toggled(bool) { sortKeys(); }
void  BlockTool::sortKeys()
{ QSignalBlocker blockKey(ui->key);
  QSignalBlocker blockRcodeEdit(ui->rcodeEdit);
  ui->key->clear();
  ui->rcodeEdit->clear();

  if ( ui->sortName->isChecked() )
    { QList<Utf8String> keyNames = dict.names();
      std::sort(keyNames.begin(), keyNames.end());
      for ( qint32 i = 0; i < keyNames.size(); i++ )
        { ui->key      ->insertItem( i, keyNames.at(i) );
          ui->rcodeEdit->insertItem( i, keyNames.at(i) );
        }
    }
   else if ( ui->sortDict->isChecked() )
    { QList<qint32> ind = dict.diNames.keys();
      std::sort(ind.begin(), ind.end());
      for ( qint32 i = 0; i < ind.size(); i++ )
        { ui->key      ->insertItem( i, dict.diNames[ind.at(i)] );
          ui->rcodeEdit->insertItem( i, dict.diNames[ind.at(i)] );
        }
    }
   else if ( ui->sortId->isChecked() )
    { QList<RiceyInt> ri = dict.ciByNum.keys();
      std::sort(ri.begin(), ri.end());
      for ( qint32 i = 0; i < ri.size(); i++ )
        { ui->key      ->insertItem( i, dict.nameFromCode(ri.at(i)) );
          ui->rcodeEdit->insertItem( i, dict.nameFromCode(ri.at(i)) );
        }
    }
   else
    qWarning( "no key sort method checked" );
  on_key_currentTextChanged( ui->key->currentText() );
}

void  BlockTool::on_makeX_toggled(bool c)
{ if ( c & ui->buildX->isChecked() )
    ui->buildA->setChecked(true);
  panelX->setMode( c ? ValueBase::Mode::make : ValueBase::Mode::idle );
}
void  BlockTool::on_makeY_toggled(bool c)
{ if ( c & ui->buildY->isChecked() )
    ui->buildA->setChecked(true);
  panelY->setMode( c ? ValueBase::Mode::make : ValueBase::Mode::idle );
}
void  BlockTool::on_buildA_toggled(bool c)
{ if ( !c ) if ( selBB ) selBB->clearSel();
  if ( c )
    selectRoot( panelA );
  panelA->setMode( c ? ValueBase::Mode::build : ValueBase::Mode::idle );
}
void  BlockTool::on_buildX_toggled(bool c)
{ if ( !c ) if ( selBB ) selBB->clearSel();
  if ( c & ui->makeX->isChecked() )
    ui->makeY->setChecked(true);
  if ( c )
    selectRoot( panelX );
  panelX->setMode( c ? ValueBase::Mode::build : ValueBase::Mode::idle );
}
void  BlockTool::on_buildY_toggled(bool c)
{ if ( !c ) if ( selBB ) selBB->clearSel();
  if ( c & ui->makeY->isChecked() )
    ui->makeX->setChecked(true);
  if ( c )
    selectRoot( panelY );
  panelY->setMode( c ? ValueBase::Mode::build : ValueBase::Mode::idle );
}
void  BlockTool::selectRoot(BlockPanel *bp)
{ if ( bp->m_kvb == nullptr )
    return;
  KeyValueBase *p_kvb = bp->m_kvb;
  if (( p_kvb->key() & RDT_ARRAY ) == 0 )
    selBB = ( (KeyValuePair *)p_kvb)->value();
   else
    selBB = ((KeyValueArray *)p_kvb)->value();
   if ( selBB )
     selBB->setSel();
}

bool  BlockTool::setBuild( KeyValueBase *kvb )
{ if ( ui->buildA->isChecked() ) { panelA->setBlock( kvb, false ); selectRoot( panelA ); panelA->update(); return true; }
  if ( ui->buildX->isChecked() ) { panelX->setBlock( kvb, false ); selectRoot( panelX ); panelX->update(); return true; }
  if ( ui->buildY->isChecked() ) { panelY->setBlock( kvb, false ); selectRoot( panelY ); panelY->update(); return true; }
  qWarning( "BlockTool::setBuild() no build panel checked" );
  return false;
}

bool  BlockTool::setMake( KeyValueBase *kvb )
{ if ( ui->makeX->isChecked() ) { panelX->setBlock( kvb ); return true; }
  if ( ui->makeY->isChecked() ) { panelY->setBlock( kvb ); return true; }
  qWarning( "BlockTool::setMake() no make panel checked" );
  return false;
}

QString  BlockTool::buildLabel()
{ if ( ui->buildA->isChecked() ) { return panelA->label(); }
  if ( ui->buildX->isChecked() ) { return panelX->label(); }
  if ( ui->buildY->isChecked() ) { return panelY->label(); }
  qWarning( "BlockTool::buildLabel() no build panel checked" );
  return "-";
}

QString  BlockTool::makeLabel()
{ if ( ui->makeX->isChecked() ) { return panelX->label(); }
  if ( ui->makeY->isChecked() ) { return panelY->label(); }
  qWarning( "BlockTool::makeLabel() no make panel checked" );
  return "-";
}

KeyValueBase *BlockTool::buildKvb()
{ if ( ui->buildA->isChecked() ) { return panelA->kvb(); }
  if ( ui->buildX->isChecked() ) { return panelX->kvb(); }
  if ( ui->buildY->isChecked() ) { return panelY->kvb(); }
  qWarning( "BlockTool::buildKvp() no build panel checked" );
  return nullptr;
}

KeyValueBase *BlockTool::makeKvb()
{ if ( ui->makeX->isChecked() ) { return panelX->kvb(); }
  if ( ui->makeY->isChecked() ) { return panelY->kvb(); }
  qWarning( "BlockTool::makeKvp() no make panel checked" );
  return nullptr;
}

bool  BlockTool::makeClear()
{ if ( ui->makeX->isChecked() ) { panelX->clear(); return true; }
  if ( ui->makeY->isChecked() ) { panelY->clear(); return true; }
  qWarning( "no make panel checked" );
  return false;
}

void  BlockTool::on_read_clicked()
{ QString readFile = QFileDialog::getOpenFileName(this,QString("Read Into %1").arg( makeLabel() ), fileDir );
  if ( readFile.size() < 1 )
    return;
  QFile file(readFile);
  if ( !file.open(QIODevice::ReadOnly) )
    { qWarning( "could not open %s for reading", readFile.toUtf8().data() );
      return;
    }
  QFileInfo fi = QFileInfo( file );
  QDir di = fi.dir();
  fileDir = di.absolutePath();
  if ( readFile.endsWith( ".json", Qt::CaseInsensitive ) )
    { JsonSerial js = file.readAll();
      if ( js.size() > 4 )
        { KeyValueBase *kvb = makeKvb();
          kvb->setJson( js );
          setMake( kvb );
        }
      return;
    }
  BsonSerial bs = file.readAll();
  if ( bs.size() > 0 )
    { KeyValueBase *kvb = makeKvb();
      kvb->setBsonish( bs );
      setMake( kvb );
    }
}

void  BlockTool::on_save_clicked()
{ KeyValueBase *kvb = buildKvb();
  if ( kvb == nullptr )
    return;
  QString saveFile = QFileDialog::getSaveFileName(this,QString("Save %1").arg( buildLabel() ), fileDir );
  if ( saveFile.size() < 1 )
    return;
  QFile file(saveFile);
  if ( !file.open(QIODevice::WriteOnly) )
    { qWarning( "could not open %s for writing", saveFile.toUtf8().data() );
      return;
    }
  QFileInfo fi = QFileInfo( file );
  QDir di = fi.dir();
  fileDir = di.absolutePath();
  if ( saveFile.endsWith( ".json", Qt::CaseInsensitive ) )
    { QTextStream ts( &file );
      ts << jsonReformat( kvb->json() );
      return;
    }
  file.write( kvb->bsonish() );
}

void BlockTool::liveDelay( int t )
{ QTimer timer;
  timer.setSingleShot( true );
  timer.start( t );
  QCoreApplication::processEvents( QEventLoop::AllEvents, t );
  while ( timer.isActive() )
    { usleep( 5000 ); // Get 5ms of "real sleep"
      QCoreApplication::processEvents( QEventLoop::AllEvents, 50 );
    }
}

QByteArray  BlockTool::jsonReformat( QByteArray j )
{ QJsonDocument jd = QJsonDocument::fromJson( j );
  return jd.toJson();
}

void  BlockTool::on_DAO0_clicked()
{ ui->report->clear();
  ui->report->append( "DⒶ0 protocol" );
  BlockValueObject *pdo = new BlockValueObject( this );
  KeyValuePair     *kvp = new KeyValuePair(RCD_ProtocolDef_o,pdo,this);
  BlockValueObject *hdo = new BlockValueObject( this );
  pdo->insert( RCD_hashedOb_o, hdo );
  BlockValueObject *hso = new BlockValueObject( this );
  pdo->insert( RCD_hash_o, hso );
  BlockValueRiceyCode *htc = new BlockValueRiceyCode( RCD_SHA3b512_y, this );
  hso->insert( RCD_type_y, htc );
  BlockValueInt64 *hti = new BlockValueInt64( 123, this );
  hso->insert( RCD_time_i, hti );
  BlockValueByteArray *hdp = new BlockValueByteArray( "SampleHash", this );
  hso->insert( RCD_data_b, hdp );

  BlockValueRiceyCode *pcc = new BlockValueRiceyCode( RCD_ProtocolDAO0_y, this );
  hdo->insert( RCD_type_y, pcc );
  BlockValueString *stp = new BlockValueString( "DⒶ0", this );
  hdo->insert( RCD_text_s, stp );
  BlockValueObjectArray *idl = new BlockValueObjectArray( this );
  hdo->insert( RCD_ItemDefList_O, idl );

  BlockValueObject *ido = new BlockValueObject( this );
  idl->append( ido );
  BlockValueRiceyCode *idc = new BlockValueRiceyCode( RCD_requestRecordStorage_o, this );
  ido->insert( RCD_type_y, idc );
  BlockValueRiceyCodeArray *ril = new BlockValueRiceyCodeArray( this );
  ido->insert( RCD_DefinedSubItems_Y, ril );
    BlockValueRiceyCode *ri1 = new BlockValueRiceyCode( RCD_data_b, this );
    ril->append( ri1 );
  // BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_userId_b, this );
  // ril->append( ri2 );
  BlockValueObjectArray *orl = new BlockValueObjectArray( this );
  ido->insert( RCD_OperReqList_O, orl );

  BlockValueObject *iro = new BlockValueObject( this );
  idl->append( iro );
  BlockValueRiceyCode *irc = new BlockValueRiceyCode( RCD_recordStorageResult_o, this );
  iro->insert( RCD_type_y, irc );
  BlockValueRiceyCodeArray *rilr = new BlockValueRiceyCodeArray( this );
  iro->insert( RCD_DefinedSubItems_Y, rilr );
    BlockValueRiceyCode *ri1r = new BlockValueRiceyCode( RCD_type_y, this );
    rilr->append( ri1r );
    BlockValueRiceyCode *oi1r = new BlockValueRiceyCode( RCD_data_b, this );
    rilr->append( oi1r );
    // BlockValueRiceyCode *ri2r = new BlockValueRiceyCode( RCD_userId_b, this );
    // rilr->append( ri2r );
  BlockValueObjectArray *orlr = new BlockValueObjectArray( this );
  iro->insert( RCD_OperReqList_O, orlr );

  BlockValueObject *idgo = new BlockValueObject( this );
  idl->append( idgo );
  BlockValueRiceyCode *idg = new BlockValueRiceyCode( RCD_requestRecordRetrieval_o, this );
  idgo->insert( RCD_type_y, idg );
  BlockValueRiceyCodeArray *rigl = new BlockValueRiceyCodeArray( this );
  idgo->insert( RCD_DefinedSubItems_Y, rigl );
    BlockValueRiceyCode *rig1 = new BlockValueRiceyCode( RCD_data_b, this );
    rigl->append( rig1 );
    // BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_userId_b, this );
    // ril->append( ri2 );
  BlockValueObjectArray *orgl = new BlockValueObjectArray( this );
  idgo->insert( RCD_OperReqList_O, orgl );

  BlockValueObject *rro = new BlockValueObject( this );
  idl->append( rro );
  BlockValueRiceyCode *rrc = new BlockValueRiceyCode( RCD_recordRetrievalResult_o, this );
  rro->insert( RCD_type_y, rrc );
  BlockValueRiceyCodeArray *rrlr = new BlockValueRiceyCodeArray( this );
  rro->insert( RCD_DefinedSubItems_Y, rrlr );
    BlockValueRiceyCode *rr1r = new BlockValueRiceyCode( RCD_type_y, this );
    rrlr->append( rr1r );
    BlockValueRiceyCode *or1r = new BlockValueRiceyCode( RCD_data_b, this );
    rrlr->append( or1r );
    // BlockValueRiceyCode *rr2r = new BlockValueRiceyCode( RCD_userId_b, this );
    // rilr->append( rr2r );
  BlockValueObjectArray *orcc = new BlockValueObjectArray( this );
  rro->insert( RCD_OperReqList_O, orcc );


  BlockValueObjectArray *adl = new BlockValueObjectArray( this );
  hdo->insert( RCD_ActorDefList_O, adl );

  BlockValueObject *ado1 = new BlockValueObject( this );
  adl->append( ado1 );
  BlockValueRiceyCode *ad1 = new BlockValueRiceyCode( RCD_actorWriterClient_y, this );
  ado1->insert( RCD_type_y, ad1 );
  BlockValueObjectArray *ado1o = new BlockValueObjectArray( this );
  ado1->insert( RCD_OutgoingItemsList_O, ado1o );
    BlockValueObject *ado1o1 = new BlockValueObject( this );
    ado1o->append( ado1o1 );
    BlockValueRiceyCode *ado1o1i = new BlockValueRiceyCode( RCD_requestRecordStorage_o, this );
    ado1o1->insert( RCD_type_y, ado1o1i );
  BlockValueObjectArray *ado1i = new BlockValueObjectArray( this );
  ado1->insert( RCD_IncomingItemsList_O, ado1i );
    BlockValueObject *ado1i1 = new BlockValueObject( this );
    ado1i->append( ado1i1 );
    BlockValueRiceyCode *ado1i1i = new BlockValueRiceyCode( RCD_recordStorageResult_o, this );
    ado1i1->insert( RCD_type_y, ado1i1i );

  BlockValueObject *ado2 = new BlockValueObject( this );
  adl->append( ado2 );
  BlockValueRiceyCode *ad2 = new BlockValueRiceyCode( RCD_actorWriterServer_y, this );
  ado2->insert( RCD_type_y, ad2 );

  BlockValueObject *ado3 = new BlockValueObject( this );
  adl->append( ado3 );
  BlockValueRiceyCode *ad3 = new BlockValueRiceyCode( RCD_actorReaderClient_y, this );
  ado3->insert( RCD_type_y, ad3 );

  BlockValueObject *ado4 = new BlockValueObject( this );
  adl->append( ado4 );
  BlockValueRiceyCode *ad4 = new BlockValueRiceyCode( RCD_actorReaderServer_y, this );
  ado4->insert( RCD_type_y, ad4 );

  if ( ui->showJson->isChecked() ) ui->report->append( jsonReformat( kvp->json() ) );
  if ( ui->showHex ->isChecked() ) ui->report->append( kvp->bsonish().toHex() );
  if ( ui->showDot ->isChecked() ) ui->report->append( kvp->dot(ValueBase::Mode::build) );
  setBuild( kvp );
  kvp->deleteLater();
}

void  BlockTool::on_chain_clicked()
{ ui->report->clear();
  ui->report->append( "Sample chain block" );
  BlockValueObject *cbo = new BlockValueObject( this );
  KeyValuePair     *kvp = new KeyValuePair(RCD_chainBlock_o,cbo,this);
  BlockValueObject *hdo = new BlockValueObject( this );
  cbo->insert( RCD_hashedOb_o, hdo );
  BlockValueObject *hso = new BlockValueObject( this );
  cbo->insert( RCD_hash_o, hso );
  BlockValueRiceyCode *htc = new BlockValueRiceyCode( RCD_SHA3b512_y, this );
  hso->insert( RCD_type_y, htc );
  BlockValueInt64 *hti = new BlockValueInt64( 1234, this );
  hso->insert( RCD_time_i, hti );
  BlockValueByteArray *hdp = new BlockValueByteArray( "SampleHash", this );
  hso->insert( RCD_data_b, hdp );
  BlockValueByteArray *dbp = new BlockValueByteArray( "BinarySample", this );
  hdo->insert( RCD_data_b, dbp );
  BlockValueString *stp = new BlockValueString( "StringSample", this );
  hdo->insert( RCD_text_s, stp );
  BlockValueObjectArray *poa = new BlockValueObjectArray( this );
  hdo->insert( RCD_parentHash_O, poa );

  BlockValueObject *hsop = new BlockValueObject( this );
  poa->append( hsop );
  BlockValueRiceyCode *htcp = new BlockValueRiceyCode( RCD_SHA3b512_y, this );
  hsop->insert( RCD_type_y, htcp );
  BlockValueInt64 *htip = new BlockValueInt64( 1122, this );
  hsop->insert( RCD_time_i, htip );
  BlockValueByteArray *hdpp = new BlockValueByteArray( "HashTrample", this );
  hsop->insert( RCD_data_b, hdpp );

  BlockValueObjectArray *sil = new BlockValueObjectArray( this );
  cbo->insert( RCD_separableItems_O, sil );
  BlockValueObjectArray *sih = new BlockValueObjectArray( this );
  hdo->insert( RCD_separableItemsHashes_O, sih );

  if ( ui->showJson->isChecked() ) ui->report->append( jsonReformat( kvp->json() ) );
  if ( ui->showHex ->isChecked() ) ui->report->append( kvp->bsonish().toHex() );
  if ( ui->showDot ->isChecked() ) ui->report->append( kvp->dot(ValueBase::Mode::build) );
  setBuild( kvp );
  kvp->deleteLater();
}

void  BlockTool::on_hash_clicked()
{ ui->report->clear();
  ui->report->append( "Sample protocol block" );
  BlockValueObject *pdo = new BlockValueObject( this );
  KeyValuePair     *kvp = new KeyValuePair(RCD_ProtocolDef_o,pdo,this);
  BlockValueObject *hdo = new BlockValueObject( this );
  pdo->insert( RCD_hashedOb_o, hdo );
  BlockValueObject *hso = new BlockValueObject( this );
  pdo->insert( RCD_hash_o, hso );
  BlockValueRiceyCode *htc = new BlockValueRiceyCode( RCD_SHA3b512_y, this );
  hso->insert( RCD_type_y, htc );
  BlockValueInt64 *hti = new BlockValueInt64( 123, this );
  hso->insert( RCD_time_i, hti );
  BlockValueByteArray *hdp = new BlockValueByteArray( "SampleHash", this );
  hso->insert( RCD_data_b, hdp );

  BlockValueRiceyCode *pcc = new BlockValueRiceyCode( RCD_ProtocolDAO2_y, this );
  hdo->insert( RCD_type_y, pcc );
  BlockValueString *stp = new BlockValueString( "DⒶ2", this );
  hdo->insert( RCD_text_s, stp );
  BlockValueObjectArray *idl = new BlockValueObjectArray( this );
  hdo->insert( RCD_ItemDefList_O, idl );

  BlockValueObject *ido = new BlockValueObject( this );
  idl->append( ido );
  BlockValueRiceyCode *idc = new BlockValueRiceyCode( RCD_hash_o, this );
  ido->insert( RCD_type_y, idc );
  BlockValueRiceyCodeArray *ril = new BlockValueRiceyCodeArray( this );
  ido->insert( RCD_DefinedSubItems_Y, ril );
  BlockValueRiceyCode *ri1 = new BlockValueRiceyCode( RCD_type_y, this );
  ril->append( ri1 );
  BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_data_b, this );
  ril->append( ri2 );
  BlockValueRiceyCode *ri3 = new BlockValueRiceyCode( RCD_time_i, this );
  ril->append( ri3 );
  BlockValueObjectArray *orl = new BlockValueObjectArray( this );
  ido->insert( RCD_OperReqList_O, orl );

  BlockValueObject *or1 = new BlockValueObject( this );
  orl->append( or1 );
    BlockValueRiceyCode *or1t = new BlockValueRiceyCode( RCD_type_y, this );
    or1->insert( RCD_type_y, or1t );
    BlockValueRiceyCodeArray *or1o = new BlockValueRiceyCodeArray( this );
    or1->insert( RCD_OpMemberOf_Y, or1o );
      BlockValueRiceyCode *or1m1 = new BlockValueRiceyCode( RCD_SHA256_y, this );
      or1o->append( or1m1 );
      BlockValueRiceyCode *or1m2 = new BlockValueRiceyCode( RCD_SHA3b512_y, this );
      or1o->append( or1m2 );
  BlockValueObject *or2 = new BlockValueObject( this );
  orl->append( or2 );
    BlockValueRiceyCode *or2t = new BlockValueRiceyCode( RCD_time_i, this );
    or2->insert( RCD_type_y, or2t );
    BlockValueRiceyCode *or2to = new BlockValueRiceyCode( RCD_time_i, this );
    or2->insert( RCD_OpTimeValue_y, or2to );

  BlockValueObject *or3 = new BlockValueObject( this );
  orl->append( or3 );
    BlockValueRiceyCode *or3t = new BlockValueRiceyCode( RCD_time_i, this );
    or3->insert( RCD_type_y, or3t );
    BlockValueRiceyCodeArray *or3o = new BlockValueRiceyCodeArray( this );
    or3->insert( RCD_OpGreaterThan_Y, or3o );
      BlockValueRiceyCode *or3s1 = new BlockValueRiceyCode( RCD_navUpOne_y, this );
      or3o->append( or3s1 );
      BlockValueRiceyCode *or3s2 = new BlockValueRiceyCode( RCD_hashedOb_o, this );
      or3o->append( or3s2 );
      BlockValueRiceyCode *or3s3 = new BlockValueRiceyCode( RCD_navIfPresent_y, this );
      or3o->append( or3s3 );
      BlockValueRiceyCode *or3s4 = new BlockValueRiceyCode( RCD_parentHash_O, this );
      or3o->append( or3s4 );
      BlockValueRiceyCode *or3s5 = new BlockValueRiceyCode( RCD_time_i, this );
      or3o->append( or3s5 );

  BlockValueObject *or4 = new BlockValueObject( this );
  orl->append( or4 );
    BlockValueRiceyCode *or4t = new BlockValueRiceyCode( RCD_data_b, this );
    or4->insert( RCD_type_y, or4t );
    BlockValueObjectArray *or4o = new BlockValueObjectArray( this );
    or4->insert( RCD_OpHash_O, or4o );
      BlockValueObject *or41 = new BlockValueObject( this );
      or4o->append( or41 );
        BlockValueRiceyCodeArray *or411 = new BlockValueRiceyCodeArray( this );
        or41->insert( RCD_riceyArray_Y, or411 );
          BlockValueRiceyCode *or4111 = new BlockValueRiceyCode( RCD_type_y, this );
          or411->append( or4111 );
      BlockValueObject *or42 = new BlockValueObject( this );
      or4o->append( or42 );
        BlockValueRiceyCodeArray *or421 = new BlockValueRiceyCodeArray( this );
        or42->insert( RCD_riceyArray_Y, or421 );
          BlockValueRiceyCode *or4211 = new BlockValueRiceyCode( RCD_time_i, this );
          or421->append( or4211 );
     BlockValueObject *or43 = new BlockValueObject( this );
     or4o->append( or43 );
       BlockValueRiceyCodeArray *or431 = new BlockValueRiceyCodeArray( this );
       or43->insert( RCD_riceyArray_Y, or431 );
         BlockValueRiceyCode *or4311 = new BlockValueRiceyCode( RCD_navUpOne_y, this );
         or431->append( or4311 );
         BlockValueRiceyCode *or4312 = new BlockValueRiceyCode( RCD_hashedOb_o, this );
         or431->append( or4312 );

/*
  BlockValueByteArray *dbp = new BlockValueByteArray( "BinarySample", this );
  hdo->insert( RCD_data_b, dbp );
  BlockValueString *stp = new BlockValueString( "StringSample", this );
  hdo->insert( RCD_text_s, stp );

  QList<RiceyInt> ta;
  ta.append( RCD_ProtocolDef_o );
  ta.append( RCD_RequiredItems_O );
  ta.append( RCD_OptionalItems_O );
  BlockArrayRicey *bar = new BlockArrayRicey( RCD_riceyArray_Y, ta, this );
  hdo->insert( RCD_riceyArray_Y, bar );
*/
  if ( ui->showJson->isChecked() ) ui->report->append( jsonReformat( kvp->json() ) );
  if ( ui->showHex ->isChecked() ) ui->report->append( kvp->bsonish().toHex() );
  if ( ui->showDot ->isChecked() ) ui->report->append( kvp->dot(ValueBase::Mode::build) );
  setBuild( kvp );
  kvp->deleteLater();
}

