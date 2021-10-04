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
  panelA = new BlockPanel( "A", BlockPanel::Mode::build, ui->frameA );
  panelX = new BlockPanel( "X", BlockPanel::Mode::make , ui->frameX );
  panelY = new BlockPanel( "Y", BlockPanel::Mode::idle , ui->frameY );
  connect( this, SIGNAL(showA(KeyValuePair *)), panelA, SLOT(setBlock(KeyValuePair *)));
  connect( this, SIGNAL(showX(KeyValuePair *)), panelX, SLOT(setBlock(KeyValuePair *)));
  connect( this, SIGNAL(showY(KeyValuePair *)), panelY, SLOT(setBlock(KeyValuePair *)));
}

BlockTool::~BlockTool()
{ delete ui; }

void  BlockTool::on_makeX_toggled(bool c)
{ if ( c & ui->buildX->isChecked() )
    ui->buildA->setChecked(true);
  panelX->setMode( c ? BlockPanel::Mode::make : BlockPanel::Mode::idle );
}
void  BlockTool::on_makeY_toggled(bool c)
{ if ( c & ui->buildY->isChecked() )
    ui->buildA->setChecked(true);
  panelY->setMode( c ? BlockPanel::Mode::make : BlockPanel::Mode::idle );
}
void  BlockTool::on_buildA_toggled(bool c)
{ panelA->setMode( c ? BlockPanel::Mode::build : BlockPanel::Mode::idle );
}
void  BlockTool::on_buildX_toggled(bool c)
{ if ( c & ui->makeX->isChecked() )
    ui->makeY->setChecked(true);
  panelX->setMode( c ? BlockPanel::Mode::build : BlockPanel::Mode::idle );
}
void  BlockTool::on_buildY_toggled(bool c)
{ if ( c & ui->makeY->isChecked() )
    ui->makeX->setChecked(true);
  panelY->setMode( c ? BlockPanel::Mode::build : BlockPanel::Mode::idle );
}

bool  BlockTool::setBuild( KeyValuePair *kvp )
{ if ( ui->buildA->isChecked() ) { panelA->setBlock( kvp ); return true; }
  if ( ui->buildX->isChecked() ) { panelX->setBlock( kvp ); return true; }
  if ( ui->buildY->isChecked() ) { panelY->setBlock( kvp ); return true; }
  qWarning( "BlockTool::setBuild() no build panel checked" );
  return false;
}

bool  BlockTool::setMake( KeyValuePair *kvp )
{ if ( ui->makeX->isChecked() ) { panelX->setBlock( kvp ); return true; }
  if ( ui->makeY->isChecked() ) { panelY->setBlock( kvp ); return true; }
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

KeyValuePair *BlockTool::buildKvp()
{ if ( ui->buildA->isChecked() ) { return panelA->kvp(); }
  if ( ui->buildX->isChecked() ) { return panelX->kvp(); }
  if ( ui->buildY->isChecked() ) { return panelY->kvp(); }
  qWarning( "BlockTool::buildKvp() no build panel checked" );
  return nullptr;
}

KeyValuePair *BlockTool::makeKvp()
{ if ( ui->makeX->isChecked() ) { return panelX->kvp(); }
  if ( ui->makeY->isChecked() ) { return panelY->kvp(); }
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
        { KeyValuePair *kvp = makeKvp();
          kvp->setJson( js );
          setMake( kvp );
        }
      return;
    }
  BsonSerial bs = file.readAll();
  if ( bs.size() > 0 )
    { KeyValuePair *kvp = makeKvp();
      kvp->setBsonish( bs );
      setMake( kvp );
    }
}

void  BlockTool::on_save_clicked()
{ KeyValuePair *kvp = buildKvp();
  if ( kvp == nullptr )
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
      ts << jsonReformat( kvp->json() );
      return;
    }
  file.write( kvp->bsonish() );
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
  BlockArrayObject *idl = new BlockArrayObject( RCD_ItemDefList_O, this );
  hdo->insert( RCD_ItemDefList_O, idl );

  BlockValueObject *ido = new BlockValueObject( this );
  idl->append( ido );
  BlockValueRiceyCode *idc = new BlockValueRiceyCode( RCD_requestRecordStorage_o, this );
  ido->insert( RCD_type_y, idc );
  BlockArrayRicey *ril = new BlockArrayRicey( RCD_DefinedSubItems_Y, this );
  ido->insert( RCD_DefinedSubItems_Y, ril );
    BlockValueRiceyCode *ri1 = new BlockValueRiceyCode( RCD_data_b, this );
    ril->append( ri1 );
  // BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_userId_b, this );
  // ril->append( ri2 );
  BlockArrayObject *orl = new BlockArrayObject( RCD_OperReqList_O, this );
  ido->insert( RCD_OperReqList_O, orl );

  BlockValueObject *iro = new BlockValueObject( this );
  idl->append( iro );
  BlockValueRiceyCode *irc = new BlockValueRiceyCode( RCD_recordStorageResult_o, this );
  iro->insert( RCD_type_y, irc );
  BlockArrayRicey *rilr = new BlockArrayRicey( RCD_DefinedSubItems_Y, this );
  iro->insert( RCD_DefinedSubItems_Y, rilr );
    BlockValueRiceyCode *ri1r = new BlockValueRiceyCode( RCD_type_y, this );
    rilr->append( ri1r );
    BlockValueRiceyCode *oi1r = new BlockValueRiceyCode( RCD_data_b, this );
    rilr->append( oi1r );
    // BlockValueRiceyCode *ri2r = new BlockValueRiceyCode( RCD_userId_b, this );
    // rilr->append( ri2r );
  BlockArrayObject *orlr = new BlockArrayObject( RCD_OperReqList_O, this );
  iro->insert( RCD_OperReqList_O, orlr );

  BlockValueObject *idgo = new BlockValueObject( this );
  idl->append( idgo );
  BlockValueRiceyCode *idg = new BlockValueRiceyCode( RCD_requestRecordRetrieval_o, this );
  idgo->insert( RCD_type_y, idg );
  BlockArrayRicey *rigl = new BlockArrayRicey( RCD_DefinedSubItems_Y, this );
  idgo->insert( RCD_DefinedSubItems_Y, rigl );
    BlockValueRiceyCode *rig1 = new BlockValueRiceyCode( RCD_data_b, this );
    rigl->append( rig1 );
    // BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_userId_b, this );
    // ril->append( ri2 );
  BlockArrayObject *orgl = new BlockArrayObject( RCD_OperReqList_O, this );
  idgo->insert( RCD_OperReqList_O, orgl );

  BlockValueObject *rro = new BlockValueObject( this );
  idl->append( rro );
  BlockValueRiceyCode *rrc = new BlockValueRiceyCode( RCD_recordRetrievalResult_o, this );
  rro->insert( RCD_type_y, rrc );
  BlockArrayRicey *rrlr = new BlockArrayRicey( RCD_DefinedSubItems_Y, this );
  rro->insert( RCD_DefinedSubItems_Y, rrlr );
    BlockValueRiceyCode *rr1r = new BlockValueRiceyCode( RCD_type_y, this );
    rrlr->append( rr1r );
    BlockValueRiceyCode *or1r = new BlockValueRiceyCode( RCD_data_b, this );
    rrlr->append( or1r );
    // BlockValueRiceyCode *rr2r = new BlockValueRiceyCode( RCD_userId_b, this );
    // rilr->append( rr2r );
  BlockArrayObject *orcc = new BlockArrayObject( RCD_OperReqList_O, this );
  rro->insert( RCD_OperReqList_O, orcc );


  BlockArrayObject *adl = new BlockArrayObject( RCD_ActorDefList_O, this );
  hdo->insert( RCD_ActorDefList_O, adl );

  BlockValueObject *ado1 = new BlockValueObject( this );
  adl->append( ado1 );
  BlockValueRiceyCode *ad1 = new BlockValueRiceyCode( RCD_actorWriterClient_y, this );
  ado1->insert( RCD_type_y, ad1 );
  BlockArrayObject *ado1o = new BlockArrayObject( RCD_OutgoingItemsList_O, this );
  ado1->insert( RCD_OutgoingItemsList_O, ado1o );
    BlockValueObject *ado1o1 = new BlockValueObject( this );
    ado1o->append( ado1o1 );
    BlockValueRiceyCode *ado1o1i = new BlockValueRiceyCode( RCD_requestRecordStorage_o, this );
    ado1o1->insert( RCD_type_y, ado1o1i );
  BlockArrayObject *ado1i = new BlockArrayObject( RCD_IncomingItemsList_O, this );
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

  ui->report->append( jsonReformat( kvp->json() ) );
  ui->report->append( kvp->bsonish().toHex() );
  // ui->report->append( kvp->dot() );
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
  BlockArrayObject *poa = new BlockArrayObject( RCD_parentHash_O, this );
  hdo->insert( RCD_parentHash_O, poa );

  BlockValueObject *hsop = new BlockValueObject( this );
  poa->append( hsop );
  BlockValueRiceyCode *htcp = new BlockValueRiceyCode( RCD_SHA3b512_y, this );
  hsop->insert( RCD_type_y, htcp );
  BlockValueInt64 *htip = new BlockValueInt64( 1122, this );
  hsop->insert( RCD_time_i, htip );
  BlockValueByteArray *hdpp = new BlockValueByteArray( "HashTrample", this );
  hsop->insert( RCD_data_b, hdpp );

  BlockArrayObject *sil = new BlockArrayObject( RCD_separableItems_O, this );
  cbo->insert( RCD_separableItems_O, sil );
  BlockArrayObject *sih = new BlockArrayObject( RCD_separableItemsHashes_O, this );
  hdo->insert( RCD_separableItemsHashes_O, sih );

  ui->report->append( jsonReformat( kvp->json() ) );
  ui->report->append( kvp->bsonish().toHex() );
//  ui->report->append( kvp->dot() );
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
  BlockArrayObject *idl = new BlockArrayObject( RCD_ItemDefList_O, this );
  hdo->insert( RCD_ItemDefList_O, idl );

  BlockValueObject *ido = new BlockValueObject( this );
  idl->append( ido );
  BlockValueRiceyCode *idc = new BlockValueRiceyCode( RCD_hash_o, this );
  ido->insert( RCD_type_y, idc );
  BlockArrayRicey *ril = new BlockArrayRicey( RCD_DefinedSubItems_Y, this );
  ido->insert( RCD_DefinedSubItems_Y, ril );
  BlockValueRiceyCode *ri1 = new BlockValueRiceyCode( RCD_type_y, this );
  ril->append( ri1 );
  BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_data_b, this );
  ril->append( ri2 );
  BlockValueRiceyCode *ri3 = new BlockValueRiceyCode( RCD_time_i, this );
  ril->append( ri3 );
  BlockArrayObject *orl = new BlockArrayObject( RCD_OperReqList_O, this );
  ido->insert( RCD_OperReqList_O, orl );

  BlockValueObject *or1 = new BlockValueObject( this );
  orl->append( or1 );
    BlockValueRiceyCode *or1t = new BlockValueRiceyCode( RCD_type_y, this );
    or1->insert( RCD_type_y, or1t );
    BlockArrayRicey *or1o = new BlockArrayRicey( RCD_OpMemberOf_Y, this );
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
    BlockArrayRicey *or3o = new BlockArrayRicey( RCD_OpGreaterThan_Y, this );
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
    BlockArrayObject *or4o = new BlockArrayObject( RCD_OpHash_O, this );
    or4->insert( RCD_OpHash_O, or4o );
      BlockValueObject *or41 = new BlockValueObject( this );
      or4o->append( or41 );
        BlockArrayRicey *or411 = new BlockArrayRicey( this );
        or41->insert( RCD_riceyArray_Y, or411 );
          BlockValueRiceyCode *or4111 = new BlockValueRiceyCode( RCD_type_y, this );
          or411->append( or4111 );
      BlockValueObject *or42 = new BlockValueObject( this );
      or4o->append( or42 );
        BlockArrayRicey *or421 = new BlockArrayRicey( this );
        or42->insert( RCD_riceyArray_Y, or421 );
          BlockValueRiceyCode *or4211 = new BlockValueRiceyCode( RCD_time_i, this );
          or421->append( or4211 );
     BlockValueObject *or43 = new BlockValueObject( this );
     or4o->append( or43 );
       BlockArrayRicey *or431 = new BlockArrayRicey( this );
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
  ui->report->append( jsonReformat( kvp->json() ) );
  ui->report->append( kvp->bsonish().toHex() );
 // ui->report->append( kvp->dot() );
  setBuild( kvp );
  kvp->deleteLater();
}

