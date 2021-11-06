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
#include "protocolParser.h"

/**
 * @brief ProtocolParser::isValid
 * @return true if the passed protocol is valid
 */
bool  ProtocolParser::isValid()
{ if ( name().size() < 1 )
    { qWarning( "protocol name not found" );
      return false;
    }
  // TODO: more checks as needed
  return true;
}

/**
 * @brief ProtocolParser::name
 * @return name of the protocol, or empty string if name is not found.
 */
Utf8String  ProtocolParser::name()
{ if ( pr->key() == RCD_ProtocolDef_o )
    { BlockValueObject *bvo = (BlockValueObject *)pr->value();
      if ( !bvo->contains( RCD_hash_o ) )     { qWarning(     "hash_o not found" ); return ""; }
      bvo = (BlockValueObject *)bvo->value( RCD_hash_o );
      if ( !bvo->contains( RCD_hashInfo_o ) ) { qWarning( "hashInfo_o not found" ); return ""; }
      bvo = (BlockValueObject *)bvo->value( RCD_hashInfo_o );
      if ( !bvo->contains( RCD_text_s ) )     { qWarning(     "text_s not found" ); return ""; }
      BlockValueString *bvs = (BlockValueString *)bvo->value( RCD_text_s );
      return bvs->value();
    }
  qWarning( "Expected ProtocolDef_o as the protocol key" );
  return "";
}

/**
 * @brief ProtocolUser::setProtocol - in the single protocol ecosystem model, the incoming
 *   protocol is the only one that the ProtocolUser will be working with.
 * @param p - protocol to set
 */
void ProtocolUser::setProtocol( BaoSerial p )
{ if ( pp )
    pp->deleteLater();
  pp = new ProtocolParser( p );
  if ( !pp->isValid() ) qWarning( "protocol not valid, not set." ); else
    { protocolSet();
      emit newName( QString::fromUtf8( pp->name() ) );
    }
}

// TODO: instead of coding navigation into the protocol, make it data driven - perhaps by a protocol definition
/**
 * @brief ProtocolActor::protocolSet - a new protocol has been set in the protocol parser
 *   get ths actor's sendable and receivable items.
 */
void  ProtocolActor::protocolSet()
{ if ( pp     == nullptr ) { qWarning( "ProtocolActor::protocolSet() error 1" ); return; }
  if ( pp->pr == nullptr ) { qWarning( "ProtocolActor::protocolSet() error 2" ); return; }
  Utf8String actor = dict.nameFromCode( actTyp );
  QList<RiceyCode> items;
  if ( pp->pr->key() == RCD_ProtocolDef_o )
    { BlockValueObject *bvi = nullptr;
      BlockValueObject *bvo = (BlockValueObject *)pp->pr->value();
      if ( bvo == nullptr )                          { qWarning( "ProtocolActor::protocolSet() Error 3"  ); return; }
      if ( !bvo->contains( RCD_hashedOb_o ) )        { qWarning( "ProtocolActor::protocolSet() Error 4"  ); return; }
      bvo = (BlockValueObject *)bvo->value( RCD_hashedOb_o );
      if ( !bvo->contains( RCD_ItemsDef_o ) )        { qWarning( "ProtocolActor::protocolSet() Error 5"  ); return; }
      bvi = (BlockValueObject *)bvo->value( RCD_ItemsDef_o );
      if ( !bvo->contains( RCD_ActorsDef_o ) )       { qWarning( "ProtocolActor::protocolSet() Error 6"  ); return; }

      // Populate the actor definition with its sendable and receivable item structure definitions
      bvo = (BlockValueObject *)bvo->value( RCD_ActorsDef_o );
      if ( !bvo->contains( actTyp ) )                { qWarning( "ProtocolActor::protocolSet() actor %s not defined in protocol", actor.data() ); return; }
      bvo = (BlockValueObject *)bvo->value( actTyp );

      if ( !bvo->contains( RCD_sendableItems_C ) )   { qWarning( "ProtocolActor::protocolSet() Error 8" ); return; }
      BlockValueRiceyCodeArray *bvc = (BlockValueRiceyCodeArray *)bvo->value().value( RCD_sendableItems_C );
      if ( bvc->size() < 1 )                         { qWarning( "ProtocolActor::protocolSet() Warning 9 - no sendable items" ); }
       else
        { if ( sendableItemDefs )
            sendableItemDefs->deleteLater();
          sendableObTypes.clear();
          sendableContents.clear();
          sendableItemDefs = new BlockValueObject( this );
          items = bvc->value(); // populate sendableItemDefs for actTyp
          foreach( RiceyCode item, items )
            { Utf8String itNam = dict.nameFromCode( item );
              if ( !bvi->contains( item ) )
                { qWarning( "%s sendable item %s not found", actor.data(), itNam.data() );
                }
               else
                { if ( !sendableItemDefs->insert( riceToInt(item), ((BlockValueObject *)(bvi->value( item )))->value() ) )
                    qWarning( "%s sendable item %s failed to insert", actor.data(), itNam.data() );
                }
            }
        }
      connectSendableItems();

      if ( !bvo->contains( RCD_receivableItems_C ) ) { qWarning( "ProtocolActor::protocolSet() Error 10" ); return; }
      bvc = (BlockValueRiceyCodeArray *)bvo->value( RCD_receivableItems_C );
      if ( bvc->size() < 1 )                         { qWarning( "ProtocolActor::protocolSet() Warning 11 - no receivable items" ); }
       else
        { if ( receivableItemDefs )
            receivableItemDefs->deleteLater();
          receivableObTypes.clear();
          receivableContents.clear();
          receivableItemDefs = new BlockValueObject( this );
          items = bvc->value();
          foreach( RiceyCode item, items )
            { Utf8String itNam = dict.nameFromCode( item );
              if ( !bvi->contains( item ) )
                { qWarning( "%s receivable item %s not found", actor.data(), itNam.data() ); }
               else
                { if ( !receivableItemDefs->insert( riceToInt(item), ((BlockValueObject *)(bvi->value( item )))->value() ) )
                    qWarning( "%s receivable item %s failed to insert", actor.data(), itNam.data() );
                }
            }
        }
    }
  connectReceivableItems();
  emit newProtocolSet(); // Enables ui to react to the new protocol items, lack of items.
  emit transactionRecord( QString( "Protocol set: %1" ).arg( QString::fromUtf8( pp->name() ) ) ); // TODO: maybe add a few descriptives...
}

/**
 * @brief ProtocolActor::connectSendableItems - navigate through each of the sendableItemDefs
 *   populate the lists of ObTypes and Contents
 *
 */
void  ProtocolActor::connectSendableItems()
{ Utf8String actor = dict.nameFromCode( actTyp );
  QList<RiceyInt> keys = sendableItemDefs->keys();
  foreach ( RiceyInt key, keys )
    { BlockValueObject *si = (BlockValueObject *)sendableItemDefs->value( key );
      if ( si == nullptr )                              qWarning( "%s sendable item nullptr", actor.data() ); else
        { Utf8String siName = dict.nameFromCode( si->vKey() );
          if ( !si->contains( RCD_ItemStructure_o ) )   qWarning( "%s sendable %s lacks structure", actor.data(), siName.data() ); else
            { BlockValueObject *itSt = (BlockValueObject *)si->value( RCD_ItemStructure_o );
              if ( itSt == nullptr )                    qWarning( "%s sendable ItemStructure %s nullptr", actor.data(), siName.data() ); else
                { QList<RiceyInt> itStKeys = itSt->keys();
                  if ( itStKeys.size() != 1 )           qWarning( "%s sendable ItemStructure %s %lld keys, should be 1"   , actor.data(), siName.data(), itStKeys.size() ); else
                    { if ( key != itStKeys.at(0) )      qWarning( "%s sendable ItemStructure %s key mismatch %llx vs %llx", actor.data(), siName.data(), key, itStKeys.at(0) ); else
                        sendableObTypes.append( key );
                    }
                }
            } // si contains ItemStructure_o
          if ( !si->contains( RCD_ItemContents_O ) )    qWarning( "%s sendable ItemContents %s lacks contents", actor.data(), siName.data() ); else
            { BlockValueObjectArray *icA = (BlockValueObjectArray *)si->value( RCD_ItemContents_O );
              if ( icA == nullptr )                     qWarning( "%s sendable ItemContents %s nullptr", actor.data(), siName.data() ); else
                { QList<BlockObjectMap> icObs = icA->value();
                  foreach ( BlockObjectMap icOm, icObs )
                    { if ( icOm.contains( RCD_OpDataLink_C ) )
                        { BlockValueRiceyCodeArray *opDataLink = (BlockValueRiceyCodeArray *)icOm[RCD_OpDataLink_C];
                          if ( opDataLink->size() < 2 ) qWarning( "%s sendable ItemContents %s not enough items %d", actor.data(), siName.data(), opDataLink->size() ); else
                            sendableContents.append( riceToInt( opDataLink->at(0) ) );
                        }
                    }
                }
            } // si contains ItemContents_O
        }    // si not nullptr
    }       // foreach key in keys
}

/**
 * @brief ProtocolActor::connectReceivableItems - navigate through each of the receivableItemDefs
 *   populate the lists of ObTypes and Contents
 */
void  ProtocolActor::connectReceivableItems()
{ Utf8String actor = dict.nameFromCode( actTyp );
  QList<RiceyInt> keys = receivableItemDefs->keys();
  foreach ( RiceyInt key, keys )
    { BlockValueObject *ri = (BlockValueObject *)receivableItemDefs->value( key );
      if ( ri == nullptr )                              qWarning( "%s receivable item nullptr", actor.data() ); else
        { Utf8String siName = dict.nameFromCode( ri->vKey() );
          if ( !ri->contains( RCD_ItemStructure_o ) )   qWarning( "%s receivable %s lacks structure", actor.data(), siName.data() ); else
           { BlockValueObject *itSt = (BlockValueObject *)ri->value( RCD_ItemStructure_o );
             if ( itSt == nullptr )                     qWarning( "%s receivable ItemStructure %s nullptr", actor.data(), siName.data() ); else
               { QList<RiceyInt> itStKeys = itSt->keys();
                 if ( itStKeys.size() != 1 )            qWarning( "%s receivable ItemStructure %s %lld keys, should be 1", actor.data(), siName.data(), itStKeys.size() ); else
                   { if ( key != itStKeys.at(0) )       qWarning( "%s receivable ItemStructure %s key mismatch %llx vs %llx", actor.data(), siName.data(), key, itStKeys.at(0) ); else
                       receivableObTypes.append( key );
                   }
               }
           } // ri contains ItemStructure_o
          if ( !ri->contains( RCD_ItemContents_O ) )    qWarning( "%s receivable %s lacks contents", actor.data(), siName.data() ); else
            { BlockValueObjectArray *icA = (BlockValueObjectArray *)ri->value( RCD_ItemContents_O );
              if ( icA == nullptr )                     qWarning( "%s receivable ItemContents %s nullptr", actor.data(), siName.data() ); else
                { QList<BlockObjectMap> icObs = icA->value();
                  foreach ( BlockObjectMap icOm, icObs )
                    { if ( icOm.contains( RCD_OpDataLink_C ) )
                        { BlockValueRiceyCodeArray *opDataLink = (BlockValueRiceyCodeArray *)icOm[RCD_OpDataLink_C];
                          if ( opDataLink->size() < 2 ) qWarning( "%s receivable ItemContents %s not enough items %d", actor.data(), siName.data(), opDataLink->size() ); else
                            { receivableContents.append( riceToInt( opDataLink->at(0) ) );
                              // qWarning( "%s receivable ItemContents %s added %llx", actor.data(), siName.data(), riceToInt( opDataLink->at(0) ) );
                            }
                        }
                    }
                }
            } // ri contains ItemContents_O
        }    // ri not nullptr
    }       // foreach key in keys
}

/**
 * @brief ProtocolActor::compose - organize inputs into an obType packet according to the currently set protocol.
 *   Composed items are for sending, so they only consider sendable items.
 *   TODO: a logical nesting structure that works with the inputs map
 * @param obType - type of packet to make
 * @param inputs - data to fill the packet with
 * @return serialized packet - or empty array if there was a problem.
 */
BaoSerial  ProtocolActor::compose( RiceyInt obType, const BlockObjectMap &inputs )
{ Utf8String obName = dict.nameFromCode( obType );
  BaoSerial err; // empty bao indicates error
  if ( !sendableItemDefs->contains( obType ) )
    { qWarning( "ProtocolActor::compose( %llx ) not found in sendableIdemDefs.", obType );
      return err;
    }
  BlockValueObject    itemDef( sendableItemDefs->value( obType )             ->bao() );
  BlockValueObject itemStruct( itemDef          .value( RCD_ItemStructure_o )->bao() );
  BlockValueObject         ob( itemStruct       .value( obType )             ->bao() );
  BlockValueObjectArray itemContents; itemContents.setBao( itemDef.value( RCD_ItemContents_O )->bao() );
  for ( qint32 i = 0; i < itemContents.size(); i++ )
    { BlockObjectMap contents = itemContents.at(i);
      if ( !contents.contains( RCD_OpDataLink_C ) )                  qWarning( "unexpected: %s contents[%d] doesn't have an OpDataLink_C", obName.data(), i ); else
        { BlockValueRiceyCodeArray dataLink; dataLink.setBao( contents[RCD_OpDataLink_C]->bao() );
          if ( dataLink.size() < 2 )                                 qWarning( "%s dataLink[%d] size %d", obName.data(), i, dataLink.size() ); else
            { if ( !inputs.contains( riceToInt( dataLink.at(0) ) ) ) qWarning( "%s dataLink[%d] defined item %llx not found in inputs", obName.data(), i, riceToInt( dataLink.at(0) ) ); else
                { if ( !populate( ob, dataLink, inputs[riceToInt(dataLink.at(0))] ) )
                    { qWarning( "problem during populate" );
                      return err;
                    }
                }
            } // dataLink size 2 or higher
        }    // contents contains OpDataLink
    }       // for i = 0 to itemContents.size()-1
  KeyValuePair kvp( obType, &ob );
  return kvp.bao();
}

/**
 * @brief ProtocolActor::populate
 * @param ob       - object to populate value in
 * @param dataLink - description of where to put the value
 * @param v        - value to be populated
 * @return true if successful
 */
bool  ProtocolActor::populate( BlockValueObject &ob, const BlockValueRiceyCodeArray &dataLink, ValueBase *v )
{ if ( dataLink.size() < 2 )
    { qWarning( "dataLink undersized %d", dataLink.size() );
      return false;
    }
  qint32 i = 1;
  ValueBase *ptr = &ob;
  while ( i < dataLink.size() )
    { // TODO: enable navigation to parents as well as children
      // TODO2: navigation into array elements
      if ( ptr != nullptr )
        { if ( !((BlockValueObject *)ptr)->contains( dataLink.at(i) ) )
           qWarning( "navigation could not find item %d %llx", i, riceToInt( dataLink.at(i) ) );
          else
           ptr = ((BlockValueObject *)ptr)->value( dataLink.at(i) );
        }
      i++;
    }
  if ( ptr == nullptr )
    { qWarning( "populate ptr is nullptr" );
      return false;
    }
  if ( ptr->type() != v->type() )
    { qWarning( "mismatch ptr type %d v type %d", ptr->type(), v->type() );
      return false;
    }
  ptr->setBao( v->bao() );
  return true;
}


/**
 * @brief ProtocolActor::extract
 * @param bao - packet to extract receivable values from
 * @return map of receivable values, empty if there is a problem
 */
BlockObjectMap ProtocolActor::extract( BaoSerial bao )
{ KeyValuePair kvp( bao );
  RiceyInt obk = kvp.key();
  BlockValueObject *bob = (BlockValueObject *)kvp.value();
  BlockObjectMap bom;
  if ( bob == nullptr )                      { qWarning( "bob nullptr" );                                  return bom; }
  if ( receivableItemDefs == nullptr )       { qWarning( "receivableItemDefs nullptr" );                   return bom; }
  if ( !receivableObTypes.contains(obk) )    { qWarning( "key %llx not found in receivableObTypes", obk ); return bom; }
  if ( !receivableItemDefs->contains(obk) )  { qWarning( "rid does not contain %llx", obk );               return bom; }
  BlockValueObject *ob = (BlockValueObject *)receivableItemDefs->value(obk);
  if ( ob == nullptr )                       { qWarning( "%llx ob nullptr", obk );                         return bom; }
  if ( !ob->contains( RCD_ItemContents_O ) ) { qWarning( "ob does not contain ItemContents" );             return bom; }
  BlockValueObjectArray *ic = (BlockValueObjectArray *)ob->value( RCD_ItemContents_O );
  if ( ic == nullptr )                       { qWarning( "%llx ic nullptr", obk );                         return bom; }
  if ( ic->size() < 1 )                        qWarning( "ic is empty?" );
  QList<BlockObjectMap> icm = ic->value();  // Work through ItemContents_O
  foreach( BlockObjectMap ici, icm )
    if ( ici.contains( RCD_OpDataLink_C ) ) // one of the OpDataLink_Cs in ItemContents_O
      { BlockValueRiceyCodeArray *odl = (BlockValueRiceyCodeArray *)ici.value( RCD_OpDataLink_C );
        if ( odl == nullptr )                  qWarning( "odl nullptr" ); else
          { if ( odl->size() < 2 )             qWarning( "odl size %d?", odl->size() ); else
              { RiceyInt it = riceToInt( odl->at(0) ); // id of the value being extracted
                ValueBase *vb = extractOne( bob, odl );
                if ( vb == nullptr )           qWarning( "extractOne failed on %llx", it ); else
                  bom.insert( it, vb );
              }
          }
      }
  return bom;
}

/**
 * @brief ProtocolActor::extractOne - pull one specified value out of the object
 * @param bob - object to read into
 * @param odl - describes a route to the value
 * @return value from bob at the end of the route odl, nullptr if there is a problem
 */
ValueBase *ProtocolActor::extractOne( BlockValueObject *bob, BlockValueRiceyCodeArray *odl )
{ if ( bob == nullptr )  { qWarning( "bob nullptr?!?" ); return nullptr; }
  if ( odl == nullptr )  { qWarning( "odl nullptr?!?" ); return nullptr; }
  if ( odl->size() < 2 ) { qWarning( "odl size %d, no path", odl->size() ); return nullptr; }
  qint32 i = 1; // index working down the route
  while ( i < odl->size() )
    { RiceyInt cid = riceToInt( odl->at(i) );
      // TODO: enable navigation to parents as well as children
      // TODO2: navigation into array elements
      if ( !bob->contains( cid ) ) { qWarning( "bob doesn't contain %llx", cid ); return nullptr; }
      if ( i == ( odl->size() - 1 ) )
        return bob->value( cid );
      if ( bob->value( cid )->type() != RDT_OBJECT ) { qWarning( "extractOne only navigates into object trees" ); return nullptr; }
      i++;
    }
  return nullptr;
}
