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
{ swapping = false;
  ui->setupUi(this);
  if ( cw )
    { QVBoxLayout *vb = new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      vb->setContentsMargins( 0,0,0,0 );
    }
  // ui->navGroup->setEnabled( false );
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

void  BlockTool::updateValueEditor()
{ if ( selBB == nullptr ) // No navigation point active
    { valueEditorNoNav();
      qWarning( "selBB nullptr" );
      return;
    }
    qWarning( "updateValueEditor   type=%x isArray=%d isContainer=%d m_key=%llx m_idx=%s",
               selBB->type(),selBB->isArray(),selBB->isContainer(),selBB->m_key,selBB->m_idx.data() );
  if ( (!selBB->isContainer()) || selBB->isArray() )
    editSelectedElement();
   else
    selectObjectForInsertion();
}

/**
 * @brief BlockTool::valueEditorNoNav - default configuration
 *   for when nothing is selected by the navigator.
 */
void  BlockTool::valueEditorNoNav()
{ qWarning( "BlockTool::valueEditorNoNav()" );
  ui->index      ->setVisible( false );
  ui->key        ->setVisible( true  );
  ui->key        ->setEnabled( true  );
  ui->valueWidget->setVisible( true  );
  ui->valueWidget->setEnabled( true  );
  ui->set        ->setVisible( false );
  ui->insert     ->setVisible( true  );
  ui->insert     ->setEnabled( true  );
  ui->remove     ->setVisible( false );
}

void  BlockTool::selectObjectForInsertion()
{ ValueBase *vb = selBB;
  if ( vb == nullptr )
    { qWarning( "BlockTool::selectObjectForInsertion() needs a valid selBB" );
      valueEditorNoNav();
      return;
    }
  qWarning( "selBB is an Object" );
  ui->remove->setVisible( true );
  if ( ui->navBuild->isChecked() )
    { ui->index      ->setVisible( false );
      ui->key        ->setVisible( false );
      ui->valueWidget->setVisible( false );
      ui->set        ->setVisible( false );
      ui->insert     ->setVisible( true  );
      ui->insert     ->setEnabled( makeKvb() != nullptr );
    }
}

/**
 * @brief BlockTool::editSelectedElement - an atomic value element is selected
 *   enable changing of that value with the set button
 */
void  BlockTool::editSelectedElement()
{ ValueBase *vb = selBB;
  if ( vb == nullptr )
    { qWarning( "BlockTool::editSelectedElement() needs a valid selBB" );
      valueEditorNoNav();
      return;
    }
  // Editing value of selected element
  RiceyInt k = vb->m_key;
  if (( k == RCD_null_z ) || ( !dict.codesContainCode( k ) ))
    { if ( k == RCD_null_z )
        qWarning( "null key" );
       else
        qWarning( "unknown key %llu", k );
      valueEditorNoNav();
      return;
    }
  int i = ui->key->findText( dict.nameFromCode( k ) );
  if ( i < 0 )
    { Utf8String keyName = dict.nameFromCode( k );
      qWarning( "key %s not found in ui", keyName.data() );
      valueEditorNoNav();
      return;
    }
  qWarning( "editSelectedElement type=%x isArray=%d isContainer=%d m_key=%llx m_idx=%s",
             vb->type(),vb->isArray(),vb->isContainer(),k,vb->m_idx.data() );
  ui->key->setVisible( true  );
  ui->key->setEnabled( false );
  ui->key->setCurrentIndex( i ); // this also selects the correct valueWidget page

  ui->index->setVisible( vb->isArray() );
  if ( vb->isArray() ) // set index value range to array current size
    ui->index->setMaximum( ((ValueBaseArray *)vb)->size() );
   else   // Why are we getting isArray for array elements?
    setEditorValue( vb );
  ui->valueWidget->setVisible( true );
  ui->valueWidget->setEnabled( true );
  ui->set        ->setVisible( !vb->isArray() );
  ui->set        ->setEnabled( true );
  ui->insert     ->setVisible(  vb->isArray() );
  ui->insert     ->setEnabled( true );
  ui->remove     ->setVisible( true );
}

/**
 * @brief BlockTool::setEditorValue - set the editor to show the current value of the selected object (atomic values)
 * @param vb - object to copy the current value from
 */
void  BlockTool::setEditorValue( ValueBase *vb )
{ qint32 i;
  switch ( vb->type() & RDT_TYPEMASK )
   { case RDT_INT64: ui->intEdit->setText( Utf8String::number(((BlockValueInt64 *)vb)->value())   ); break;
     case RDT_MPZ:   ui->mpzEdit->setText( ValueBase::removeQuotes(((BlockValueMPZ *)vb)->json()) ); break;
     case RDT_MPQ:   ui->mpqEdit->setText( ValueBase::removeQuotes(((BlockValueMPQ *)vb)->json()) ); break;
     case RDT_STRING:    ui->stringEdit->   setText( ((BlockValueString    *)vb)->value()         ); break;
     case RDT_BYTEARRAY: ui->byteArrayEdit->setText( ((BlockValueByteArray *)vb)->value().toHex() ); break;
     case RDT_RCODE:
       i = ui->rcodeEdit->findText( dict.nameFromCode( ((BlockValueRiceyCode *)vb)->value()) );
       if ( i > 0 )
         ui->rcodeEdit->setCurrentIndex( i );
       break;
   }
}

/**
 * @brief BlockTool::on_set_clicked - set has been clicked, hopefully while one of the atomic
 *   value types is selected.  Update its value with that from the ui edit field, or throw a
 *   warning message if there is a problem.
 */
void  BlockTool::on_set_clicked()
{ ValueBase *vb = selBB;
  if ( vb == nullptr ) { qWarning( "attempt to set when nothing is selected" );                    return; }
  if ( vb->isArray() ) { qWarning( "shouldn't be able to set in an array element (only insert)" ); return; }
  Utf8String codeName;
  switch ( vb->type() & RDT_TYPEMASK )
    { case RDT_MPZ:       ((BlockValueMPZ       *)vb)->set( ui->mpzEdit->text().toUtf8() );     updateNav(); break;
      case RDT_MPQ:       ((BlockValueMPQ       *)vb)->set( ui->mpqEdit->text().toUtf8() );     updateNav(); break;
      case RDT_INT64:     ((BlockValueInt64     *)vb)->set( ui->intEdit->text().toLongLong() ); updateNav(); break;
      case RDT_STRING:    ((BlockValueString    *)vb)->set( ui->stringEdit->text().toUtf8()  ); updateNav(); break;
      case RDT_BYTEARRAY: ((BlockValueByteArray *)vb)->set( QByteArray::fromHex( ui->byteArrayEdit->text().toUtf8() ) ); updateNav(); break;
      case RDT_RCODE:
        codeName = ui->rcodeEdit->currentText().toUtf8();
        if ( dict.codesContainName( codeName ) )
          { ((BlockValueRiceyCode *)vb)->set( dict.codeFromCodeName( codeName ) );
            updateNav();
          }
         else
          qWarning( "dictionary does not contain %s", codeName.data() );
        break;
      default:
        qWarning( "BlockTool::on_set_clicked() unrecognized type %x", vb->type() & RDT_TYPEMASK );
    }
}

void  BlockTool::on_remove_clicked()
{ qWarning( "BlockTool::on_remove_clicked()" );
  ValueBase *vb = selBB;
  if ( vb           == nullptr )     { qWarning( "BlockTool::on_remove_clicked() needs a valid selBB" );                              return; }
  if ( vb->vbParent == nullptr )     { qWarning( "BlockTool::on_remove_clicked() will not remove the root object, just use Clear." ); return; }
  if (!vb->vbParent->isContainer() ) { qWarning( "BlockTool::on_remove_clicked() Cannot remove from non-containers" );                return; }
  if ( vb->vbParent->isArray() )     { qWarning( "BlockTool::on_remove_clicked() TODO: handle removal from arrays" );                 return; }
  BlockValueObject *pbvo = (BlockValueObject *)((ValueBase *)vb->vbParent);
  RiceyInt key = vb->vKey();
  if ( !pbvo->contains( key ) ) { qWarning( "BlockTool::on_remove_clicked() key %llx not found in parent object", key ); return; }
  on_prev_clicked();
  pbvo->remove( key );
  updateNav();
}

void  BlockTool::on_insert_clicked()
{ if ( ui->navMake->isChecked() )
    { if ( makeKvb() == nullptr )
        { // New kvb in make register
          insertKeyName( ui->key->currentText().toUtf8() );
          updateValueEditor();
          ui->navGroup->setEnabled( true );
          return;
        }
    }
  if ( ui->navBuild->isChecked() )
    { // TODO: compatibility check between selBB and the current key
      if ( makeKvb()     == nullptr    ) { qWarning( "Make is empty"             ); return; }
      if ( selBB         == nullptr    ) { qWarning( "Nothing selected"          ); return; }
      if ( selBB->type() != RDT_OBJECT ) { qWarning( "Selected block not object" ); return; }
      BlockValueObject *bvo = (BlockValueObject *)((ValueBase *)selBB);
      RiceyInt key = makeKvb()->key();
      if ( bvo->contains(key) )
        { qWarning( "selected object already contains a member with type %llx", key );
          return;
        }
      if ( makeKvb()->isKeyValuePair() )
        { KeyValuePair *kvp = (KeyValuePair *)makeKvb();
          bvo->insert( kvp->key(), kvp->value() );
        }
       else
        { bvo->insert( (KeyValueArray *)makeKvb() );
        }
      on_clear_clicked();
      updateBuild();
    }
}

void  BlockTool::insertKeyName( Utf8String nKey )
{ if ( !dict.codesContainName( nKey ) )
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
          case RDT_INT64    : vbp = new BlockValueInt64    ( ui->intEdit  ->text().toLongLong(), this ); break;
          case RDT_RCODE    : vbp = new BlockValueRiceyCode( dict.codeFromCodeName( ui->rcodeEdit->currentText().toUtf8() ), this ); break;
          case RDT_MPZ      : vbp = new BlockValueMPZ      ( ui->mpzEdit->text().toUtf8(), this ); break;
          case RDT_MPQ      : vbp = new BlockValueMPQ      ( ui->mpqEdit->text().toUtf8(), this ); break;
          case RDT_BYTEARRAY: vbp = new BlockValueByteArray( QByteArray::fromHex( ui->byteArrayEdit->text().toUtf8() ), this ); break;
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
  KeyValueBase *kvb = makeKvb();
  if ( kvb != nullptr )
    { kvb->deleteLater();
      kvb = nullptr;
    }
  RiceyInt kc = dict.codeFromCodeName( nKey );
  if ( ( tp & RDT_ARRAY ) == 0 )
    kvb = new KeyValuePair( kc, vbp, this );
   else
    kvb = new KeyValueArray( kc, (ValueBaseArray *)vbp, this );
  setMake( kvb );
  makeKvb()->setValueKey( kc );
  if ( ui->showJson->isChecked() )ui->report->append( jsonReformat( kvb->json() ) );
  if ( ui->showHex ->isChecked() )ui->report->append( kvb->bao().toHex() );
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
      case RDT_OBJECT_ARRAY   :
      case RDT_OBJECT         : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_object    )); break;
      case RDT_INT64_ARRAY    :
      case RDT_INT64          : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_int       )); break;
      case RDT_RCODE_ARRAY    :
      case RDT_RCODE          : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_rcode     )); break;
      case RDT_MPZ_ARRAY      :
      case RDT_MPZ            : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_mpz       )); break;
      case RDT_MPQ_ARRAY      :
      case RDT_MPQ            : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_mpq       )); break;
      case RDT_BYTEARRAY_ARRAY:
      case RDT_BYTEARRAY      : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_byteArray )); break;
      case RDT_STRING_ARRAY   :
      case RDT_STRING         : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_string    )); break;
//      case X_ARRAY   : ui->valueWidget->setCurrentIndex( ui->valueWidget->indexOf(ui->page_array     )); break;
      default: qWarning( "key comboBox selected unhandled type %d key: %s", tp, nKey.data() );
    }
}


void  BlockTool::on_navMake_toggled( bool make )
{ if ( !make )
    return;
  if ( ui->makeX->isChecked() ) { if ( !swapping ) selectRoot( panelX ); panelX->update(); }
  if ( ui->makeY->isChecked() ) { if ( !swapping ) selectRoot( panelY ); panelY->update(); }
  updateBuild();
  updateValueEditor();
}

void  BlockTool::on_navBuild_toggled( bool build )
{ if ( !build )
    return;
  if ( ui->buildA->isChecked() ) { if ( !swapping ) selectRoot( panelA ); panelA->update(); }
  if ( ui->buildX->isChecked() ) { if ( !swapping ) selectRoot( panelX ); panelX->update(); }
  if ( ui->buildY->isChecked() ) { if ( !swapping ) selectRoot( panelY ); panelY->update(); }
  updateMake();
  updateValueEditor();
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
{ if ( !c && selBB && ui->navMake->isChecked() )
    { selBB->clearSel();
   //   ui->navGroup->setEnabled( false );
    }
  if ( c && ui->buildX->isChecked() )
    ui->buildA->setChecked(true);
  if ( c && ui->navMake->isChecked() )
    selectRoot( panelX );
   else
    updateValueEditor();
  panelX->setMode( c ? ValueBase::Mode::make : ValueBase::Mode::idle );
}
void  BlockTool::on_makeY_toggled(bool c)
{ if ( !c && selBB && ui->navMake->isChecked() )
    { selBB->clearSel();
   // ui->navGroup->setEnabled( false );
    }
  if ( c && ui->buildY->isChecked() )
    ui->buildA->setChecked(true);
  if ( c && ui->navMake->isChecked() )
    selectRoot( panelY );
  panelY->setMode( c ? ValueBase::Mode::make : ValueBase::Mode::idle );
  // updateValueEditor(); will happen on the makeX_toggled side
}
void  BlockTool::on_buildA_toggled(bool c)
{ if ( !c && selBB && ui->navBuild->isChecked() )
    { selBB->clearSel();
     // ui->navGroup->setEnabled( false );
    }
  if ( c && ui->navBuild->isChecked() )
    selectRoot( panelA );
  panelA->setMode( c ? ValueBase::Mode::build : ValueBase::Mode::idle );
}
void  BlockTool::on_buildX_toggled(bool c)
{ if ( !c && selBB && ui->navBuild->isChecked() )
    { selBB->clearSel();
    //  ui->navGroup->setEnabled( false );
    }
  if ( c && ui->makeX->isChecked() )
    ui->makeY->setChecked(true);
  if ( c && ui->navBuild->isChecked() )
    selectRoot( panelX );
  panelX->setMode( c ? ValueBase::Mode::build : ValueBase::Mode::idle );
}
void  BlockTool::on_buildY_toggled(bool c)
{ if ( !c && selBB && ui->navBuild->isChecked() )
    { selBB->clearSel();
    //  ui->navGroup->setEnabled( false );
    }
  if ( c && ui->makeY->isChecked() )
    ui->makeX->setChecked(true);
  if ( c && ui->navBuild->isChecked() )
    selectRoot( panelY );
  panelY->setMode( c ? ValueBase::Mode::build : ValueBase::Mode::idle );
}

void  BlockTool::selectRoot(BlockPanel *bp)
{ if ( bp == nullptr )
    return;
  if ( bp->m_kvb == nullptr )
    { updateBB( nullptr );
      return;
    }
  KeyValueBase *p_kvb = bp->m_kvb;
  ValueBase *vb = nullptr;
  if (( p_kvb->key() & RDT_ARRAY ) == 0 )
    vb = ( (KeyValuePair *)p_kvb)->value();
   else
    vb = ((KeyValueArray *)p_kvb)->value();
//   ui->navGroup->setEnabled( vb != nullptr );
   updateBB( vb );
}

void  BlockTool::on_swap_clicked()
{ BlockPanel *mp = makePanel();
  BlockPanel *bp = buildPanel();
  if (( mp == nullptr ) || ( bp == nullptr ))
    { qWarning( "swap - cannot swap, panel undefined" );
      return;
    }
  swapping = true;
  KeyValueBase *vb = mp->m_kvb;
  mp->m_kvb = bp->m_kvb;
  bp->m_kvb = vb;
  // Setting the opposite nav mode true (instead of setting the current one false)
  //  is important to the order of operations that results during updates and redraws.
  if ( ui->navBuild->isChecked() )
    ui->navMake ->setChecked( true );
   else
    ui->navBuild->setChecked( true );
  swapping = false;
}

void  BlockTool::on_prev_clicked()
{ if ( selBB == nullptr )
    { if ( ui->navBuild->isChecked() )
        ui->navMake->setChecked( true );
       else
        ui->navBuild->setChecked( true );
      return;
    }
  ValueBase *vb = nullptr;
  ValueBase *vbp = selBB->vbParent;
  if ( vbp == nullptr )
    { qWarning( "TODO: handle the case where the top level container is a KeyValueArray..." );
    }
   else
    { if ( vbp->isContainer() )
        vb = vbp->prevChild( selBB );
       else
        qWarning( "when is a parent not a container?" );
    }

  if ( vb )
    { ui->navGroup->setEnabled( false );
      updateBB( vb );
      navPanel = nullptr;
      if ( ui->navBuild->isChecked() ) navPanel = buildPanel();
      if ( ui->navMake ->isChecked() ) navPanel = makePanel();
      if ( navPanel )
        { connect( navPanel, SIGNAL(drawingComplete()), this, SLOT(navDrawComplete()) );
          navPanel->update();
        }
       else
        qWarning( "neither nav button checked." );
    }
}

void  BlockTool::navDrawComplete()
{ disconnect( navPanel, SIGNAL(drawingComplete()), this, SLOT(navDrawComplete()) );
  ui->navGroup->setEnabled( true );
}

/**
 * @brief BlockTool::on_next_clicked - depth first search
 */
void  BlockTool::on_next_clicked()
{ if ( selBB == nullptr )
    { if ( ui->navBuild->isChecked() )
        ui->navMake->setChecked( true );
       else
        ui->navBuild->setChecked( true );
      return;
    }
  ValueBase *vb = nullptr;
  if ( selBB->isContainer() )
    if ( selBB->size() > 0 )
      vb = selBB->firstChild();

  if ( vb == nullptr )
   if ( selBB->vbParent )
     vb = selBB->vbParent->nextChild( selBB );

  if ( vb )
    { ui->navGroup->setEnabled( false );
      updateBB( vb );
      navPanel = nullptr;
      if ( ui->navBuild->isChecked() )
        navPanel = updateBuild();
      if ( ui->navMake->isChecked() )
        navPanel = updateMake();
      if ( navPanel )
        connect( navPanel, SIGNAL(drawingComplete()), this, SLOT(navDrawComplete()) );
       else
        qWarning( "neither nav button checked." );
    }
}

void  BlockTool::updateBB( ValueBase *vb )
{ if ( selBB )
    selBB->clearSel();
  selBB = vb;
  if ( selBB )
    { selBB->setSel();
      updateValueEditor();
    }
}

void  BlockTool::on_now_clicked()
{ // TODO: parse and add time in the plus line edit
  qint64 t = QDateTime::currentMSecsSinceEpoch()*1000;
  ui->intEdit->setText( QString::number(t) );
}

BlockPanel *BlockTool::makePanel()
{ if ( ui->makeX->isChecked() ) return panelX;
  if ( ui->makeY->isChecked() ) return panelY;
  qWarning( "BlockTool::makePanel() no make panel checked" );
  return nullptr;
}

BlockPanel *BlockTool::buildPanel()
{ if ( ui->buildA->isChecked() ) return panelA;
  if ( ui->buildX->isChecked() ) return panelX;
  if ( ui->buildY->isChecked() ) return panelY;
  qWarning( "BlockTool::buildPanel() no build panel checked" );
  return nullptr;
}

BlockPanel *BlockTool::updateMake()
{ if ( ui->makeX->isChecked() ) { panelX->update(); return panelX; }
  if ( ui->makeY->isChecked() ) { panelY->update(); return panelY; }
  qWarning( "BlockTool::updateMake() no make panel checked" );
  return nullptr;
}

BlockPanel *BlockTool::updateBuild()
{ if ( ui->buildA->isChecked() ) { panelA->update(); return panelA; }
  if ( ui->buildX->isChecked() ) { panelX->update(); return panelX; }
  if ( ui->buildY->isChecked() ) { panelY->update(); return panelY; }
  qWarning( "BlockTool::updateBuild() no build panel checked" );
  return nullptr;
}

BlockPanel *BlockTool::updateNav()
{ if ( ui->navBuild->isChecked() ) return updateBuild();
  if ( ui->navMake ->isChecked() ) return updateMake();
  qWarning( "BlockTool::updateBuild() no nav choice checked" );
  return nullptr;
}


bool  BlockTool::setBuild( KeyValueBase *kvb )
{ if ( ui->buildA->isChecked() )
    { panelA->setBlock( kvb, false );
      if ( ui->navBuild->isChecked() )
        selectRoot( panelA );
      panelA->update();
      return true;
    }
  if ( ui->buildX->isChecked() )
    { panelX->setBlock( kvb, false );
      if ( ui->navBuild->isChecked() )
        selectRoot( panelX );
      panelX->update();
      return true;
    }
  if ( ui->buildY->isChecked() )
    { panelY->setBlock( kvb, false );
      if ( ui->navBuild->isChecked() )
        selectRoot( panelY );
      panelY->update();
      return true;
    }
  qWarning( "BlockTool::setBuild() no build panel checked" );
  return false;
}

bool  BlockTool::setMake( KeyValueBase *kvb )
{ if ( ui->makeX->isChecked() )
    { panelX->setBlock( kvb );
      if ( ui->navMake->isChecked() )
        selectRoot( panelX );
      panelX->update();
      return true;
    }
  if ( ui->makeY->isChecked() )
    { panelY->setBlock( kvb );
      if ( ui->navMake->isChecked() )
        selectRoot( panelY );
      panelY->update();
      return true;
    }
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
{ if ( ui->makeX->isChecked() )
    { panelX->clear();
      if ( ui->navMake->isChecked() )
        updateBB( nullptr );
      updateValueEditor();
      return true;
    }
  if ( ui->makeY->isChecked() )
    { panelY->clear();
      if ( ui->navMake->isChecked() )
        updateBB( nullptr );
      updateValueEditor();
      return true;
    }
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
   BaoSerial bs = file.readAll();
  if ( bs.size() > 0 )
    { KeyValueBase *kvb = makeKvb();
      kvb->setBao( bs );
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
  file.write( kvb->bao() );
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
  BlockValueRiceyCode *htc = new BlockValueRiceyCode( RCD_SHA3b512_c, this );
  hso->insert( RCD_type_c, htc );
  BlockValueInt64 *hti = new BlockValueInt64( 123, this );
  hso->insert( RCD_time_i, hti );
  BlockValueByteArray *hdp = new BlockValueByteArray( "SampleHash", this );
  hso->insert( RCD_data_b, hdp );

  BlockValueRiceyCode *pcc = new BlockValueRiceyCode( RCD_ProtocolDAO0_c, this );
  hdo->insert( RCD_type_c, pcc );
  BlockValueString *stp = new BlockValueString( "DⒶ0", this );
  hdo->insert( RCD_text_s, stp );
  BlockValueObjectArray *idl = new BlockValueObjectArray( this );
  hdo->insert( RCD_ItemDefList_O, idl );

  BlockValueObject *ido = new BlockValueObject( this );
  idl->append( ido );
  BlockValueRiceyCode *idc = new BlockValueRiceyCode( RCD_requestRecordStorage_o, this );
  ido->insert( RCD_type_c, idc );
  BlockValueRiceyCodeArray *ril = new BlockValueRiceyCodeArray( this );
  ido->insert( RCD_DefinedSubItems_C, ril );
    BlockValueRiceyCode *ri1 = new BlockValueRiceyCode( RCD_data_b, this );
    ril->append( ri1 );
  // BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_userId_b, this );
  // ril->append( ri2 );
  BlockValueObjectArray *orl = new BlockValueObjectArray( this );
  ido->insert( RCD_OperReqList_O, orl );

  BlockValueObject *iro = new BlockValueObject( this );
  idl->append( iro );
  BlockValueRiceyCode *irc = new BlockValueRiceyCode( RCD_recordStorageResult_o, this );
  iro->insert( RCD_type_c, irc );
  BlockValueRiceyCodeArray *rilr = new BlockValueRiceyCodeArray( this );
  iro->insert( RCD_DefinedSubItems_C, rilr );
    BlockValueRiceyCode *ri1r = new BlockValueRiceyCode( RCD_type_c, this );
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
  idgo->insert( RCD_type_c, idg );
  BlockValueRiceyCodeArray *rigl = new BlockValueRiceyCodeArray( this );
  idgo->insert( RCD_DefinedSubItems_C, rigl );
    BlockValueRiceyCode *rig1 = new BlockValueRiceyCode( RCD_data_b, this );
    rigl->append( rig1 );
    // BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_userId_b, this );
    // ril->append( ri2 );
  BlockValueObjectArray *orgl = new BlockValueObjectArray( this );
  idgo->insert( RCD_OperReqList_O, orgl );

  BlockValueObject *rro = new BlockValueObject( this );
  idl->append( rro );
  BlockValueRiceyCode *rrc = new BlockValueRiceyCode( RCD_recordRetrievalResult_o, this );
  rro->insert( RCD_type_c, rrc );
  BlockValueRiceyCodeArray *rrlr = new BlockValueRiceyCodeArray( this );
  rro->insert( RCD_DefinedSubItems_C, rrlr );
    BlockValueRiceyCode *rr1r = new BlockValueRiceyCode( RCD_type_c, this );
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
  BlockValueRiceyCode *ad1 = new BlockValueRiceyCode( RCD_actorWriterClient_c, this );
  ado1->insert( RCD_type_c, ad1 );
  BlockValueObjectArray *ado1o = new BlockValueObjectArray( this );
  ado1->insert( RCD_OutgoingItemsList_O, ado1o );
    BlockValueObject *ado1o1 = new BlockValueObject( this );
    ado1o->append( ado1o1 );
    BlockValueRiceyCode *ado1o1i = new BlockValueRiceyCode( RCD_requestRecordStorage_o, this );
    ado1o1->insert( RCD_type_c, ado1o1i );
  BlockValueObjectArray *ado1i = new BlockValueObjectArray( this );
  ado1->insert( RCD_IncomingItemsList_O, ado1i );
    BlockValueObject *ado1i1 = new BlockValueObject( this );
    ado1i->append( ado1i1 );
    BlockValueRiceyCode *ado1i1i = new BlockValueRiceyCode( RCD_recordStorageResult_o, this );
    ado1i1->insert( RCD_type_c, ado1i1i );

  BlockValueObject *ado2 = new BlockValueObject( this );
  adl->append( ado2 );
  BlockValueRiceyCode *ad2 = new BlockValueRiceyCode( RCD_actorWriterServer_c, this );
  ado2->insert( RCD_type_c, ad2 );

  BlockValueObject *ado3 = new BlockValueObject( this );
  adl->append( ado3 );
  BlockValueRiceyCode *ad3 = new BlockValueRiceyCode( RCD_actorReaderClient_c, this );
  ado3->insert( RCD_type_c, ad3 );

  BlockValueObject *ado4 = new BlockValueObject( this );
  adl->append( ado4 );
  BlockValueRiceyCode *ad4 = new BlockValueRiceyCode( RCD_actorReaderServer_c, this );
  ado4->insert( RCD_type_c, ad4 );

  if ( ui->showJson->isChecked() ) ui->report->append( jsonReformat( kvp->json() ) );
  if ( ui->showHex ->isChecked() ) ui->report->append( kvp->bao().toHex() );
  if ( ui->showDot ->isChecked() ) ui->report->append( kvp->dot(ValueBase::Mode::build) );
  setBuild( kvp ); updateMake();
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
  BlockValueRiceyCode *htc = new BlockValueRiceyCode( RCD_SHA3b512_c, this );
  hso->insert( RCD_type_c, htc );
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
  BlockValueRiceyCode *htcp = new BlockValueRiceyCode( RCD_SHA3b512_c, this );
  hsop->insert( RCD_type_c, htcp );
  BlockValueInt64 *htip = new BlockValueInt64( 1122, this );
  hsop->insert( RCD_time_i, htip );
  BlockValueByteArray *hdpp = new BlockValueByteArray( "HashTrample", this );
  hsop->insert( RCD_data_b, hdpp );

  BlockValueObjectArray *sil = new BlockValueObjectArray( this );
  cbo->insert( RCD_separableItems_O, sil );
  BlockValueObjectArray *sih = new BlockValueObjectArray( this );
  hdo->insert( RCD_separableItemsHashes_O, sih );

  if ( ui->showJson->isChecked() ) ui->report->append( jsonReformat( kvp->json() ) );
  if ( ui->showHex ->isChecked() ) ui->report->append( kvp->bao().toHex() );
  if ( ui->showDot ->isChecked() ) ui->report->append( kvp->dot(ValueBase::Mode::build) );
  setBuild( kvp ); updateMake();
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
  BlockValueRiceyCode *htc = new BlockValueRiceyCode( RCD_SHA3b512_c, this );
  hso->insert( RCD_type_c, htc );
  BlockValueInt64 *hti = new BlockValueInt64( 123, this );
  hso->insert( RCD_time_i, hti );
  BlockValueByteArray *hdp = new BlockValueByteArray( "SampleHash", this );
  hso->insert( RCD_data_b, hdp );

  BlockValueRiceyCode *pcc = new BlockValueRiceyCode( RCD_ProtocolDAO2_c, this );
  hdo->insert( RCD_type_c, pcc );
  BlockValueString *stp = new BlockValueString( "DⒶ2", this );
  hdo->insert( RCD_text_s, stp );
  BlockValueObjectArray *idl = new BlockValueObjectArray( this );
  hdo->insert( RCD_ItemDefList_O, idl );

  BlockValueObject *ido = new BlockValueObject( this );
  idl->append( ido );
    BlockValueRiceyCode *idc = new BlockValueRiceyCode( RCD_hash_o, this );
    ido->insert( RCD_type_c, idc );
    BlockValueRiceyCodeArray *ril = new BlockValueRiceyCodeArray( this );
    ido->insert( RCD_DefinedSubItems_C, ril );
      BlockValueRiceyCode *ri1 = new BlockValueRiceyCode( RCD_type_c, this );
      ril->append( ri1 );
      BlockValueRiceyCode *ri2 = new BlockValueRiceyCode( RCD_data_b, this );
      ril->append( ri2 );
      BlockValueRiceyCode *ri3 = new BlockValueRiceyCode( RCD_time_i, this );
      ril->append( ri3 );
    BlockValueObjectArray *orl = new BlockValueObjectArray( this );
    ido->insert( RCD_OperReqList_O, orl );

      BlockValueObject *or1 = new BlockValueObject( this );
      orl->append( or1 );
        BlockValueRiceyCode *or1t = new BlockValueRiceyCode( RCD_type_c, this );
        or1->insert( RCD_type_c, or1t );
        BlockValueRiceyCodeArray *or1o = new BlockValueRiceyCodeArray( this );
        or1->insert( RCD_OpMemberOf_C, or1o );
          BlockValueRiceyCode *or1m1 = new BlockValueRiceyCode( RCD_SHA256_c, this );
          or1o->append( or1m1 );
          BlockValueRiceyCode *or1m2 = new BlockValueRiceyCode( RCD_SHA3b512_c, this );
          or1o->append( or1m2 );
      BlockValueObject *or2 = new BlockValueObject( this );
      orl->append( or2 );
        BlockValueRiceyCode *or2t = new BlockValueRiceyCode( RCD_time_i, this );
        or2->insert( RCD_type_c, or2t );
        BlockValueRiceyCode *or2to = new BlockValueRiceyCode( RCD_time_i, this );
        or2->insert( RCD_OpTimeValue_c, or2to );

      BlockValueObject *or3 = new BlockValueObject( this );
      orl->append( or3 );
        BlockValueRiceyCode *or3t = new BlockValueRiceyCode( RCD_time_i, this );
        or3->insert( RCD_type_c, or3t );
        BlockValueRiceyCodeArray *or3o = new BlockValueRiceyCodeArray( this );
        or3->insert( RCD_OpGreaterThan_C, or3o );
          BlockValueRiceyCode *or3s1 = new BlockValueRiceyCode( RCD_navUpOne_c, this );
          or3o->append( or3s1 );
          BlockValueRiceyCode *or3s2 = new BlockValueRiceyCode( RCD_hashedOb_o, this );
          or3o->append( or3s2 );
          BlockValueRiceyCode *or3s3 = new BlockValueRiceyCode( RCD_navIfPresent_c, this );
          or3o->append( or3s3 );
          BlockValueRiceyCode *or3s4 = new BlockValueRiceyCode( RCD_parentHash_O, this );
          or3o->append( or3s4 );
          BlockValueRiceyCode *or3s5 = new BlockValueRiceyCode( RCD_time_i, this );
          or3o->append( or3s5 );

      BlockValueObject *or4 = new BlockValueObject( this );
      orl->append( or4 );
        BlockValueRiceyCode *or4t = new BlockValueRiceyCode( RCD_data_b, this );
        or4->insert( RCD_type_c, or4t );
        BlockValueObjectArray *or4o = new BlockValueObjectArray( this );
        or4->insert( RCD_OpHash_O, or4o );
          BlockValueObject *or41 = new BlockValueObject( this );
          or4o->append( or41 );
            BlockValueRiceyCodeArray *or411 = new BlockValueRiceyCodeArray( this );
            or41->insert( RCD_riceyArray_C, or411 );
              BlockValueRiceyCode *or4111 = new BlockValueRiceyCode( RCD_type_c, this );
              or411->append( or4111 );
          BlockValueObject *or42 = new BlockValueObject( this );
          or4o->append( or42 );
            BlockValueRiceyCodeArray *or421 = new BlockValueRiceyCodeArray( this );
            or42->insert( RCD_riceyArray_C, or421 );
              BlockValueRiceyCode *or4211 = new BlockValueRiceyCode( RCD_time_i, this );
              or421->append( or4211 );
         BlockValueObject *or43 = new BlockValueObject( this );
         or4o->append( or43 );
           BlockValueRiceyCodeArray *or431 = new BlockValueRiceyCodeArray( this );
           or43->insert( RCD_riceyArray_C, or431 );
             BlockValueRiceyCode *or4311 = new BlockValueRiceyCode( RCD_navUpOne_c, this );
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
  BlockArrayRicey *bar = new BlockArrayRicey( RCD_riceyArray_C, ta, this );
  hdo->insert( RCD_riceyArray_C, bar );
*/
  if ( ui->showJson->isChecked() ) ui->report->append( jsonReformat( kvp->json() ) );
  if ( ui->showHex ->isChecked() ) ui->report->append( kvp->bao().toHex() );
  if ( ui->showDot ->isChecked() ) ui->report->append( kvp->dot(ValueBase::Mode::build) );
  setBuild( kvp ); updateMake();
  kvp->deleteLater();
}

