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
{ if ( pr->key() != RCD_ProtocolDef_o )
    { qWarning( "protocol invalid" );
      return false;
    }
  if ( name().size() < 1 )
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
      if ( !bvo->contains( RCD_hash_o ) )     return "1";
      bvo = (BlockValueObject *)bvo->value( RCD_hash_o );
      if ( !bvo->contains( RCD_hashInfo_o ) ) return "2";
      bvo = (BlockValueObject *)bvo->value( RCD_hashInfo_o );
      if ( !bvo->contains( RCD_text_s ) )     return "3";
      BlockValueString *bvs = (BlockValueString *)bvo->value( RCD_text_s );
      return bvs->value();
    }
  return "0";
}

void ProtocolUser::setProtocol( BaoSerial p )
{ if ( pp )
    pp->deleteLater();
  pp = new ProtocolParser( p );
  if ( pp->isValid() )
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
{ if ( pp == nullptr )
    { qWarning( "ProtocolActor::protocolSet() error 1" );
      return;
    }
  if ( pp->pr == nullptr )
      { qWarning( "ProtocolActor::protocolSet() error 2" );
        return;
      }
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
      if ( bvc->size() < 1 )                         { qWarning( "ProtocolActor::protocolSet() Error 9 - no sendable items" ); }
       else
        { // qWarning( "ProtocolActor::protocolSet() %s %d sendable items", actor.data(), bvc->size() );
          if ( sendableItemDefs )
            sendableItemDefs->deleteLater();
          sendableObTypes.clear();
          sendableContents.clear();
          sendableItemDefs = new BlockValueObject( this );
          items = bvc->value();
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
      if ( bvc->size() < 1 )                         { qWarning( "ProtocolActor::protocolSet() Error 11 - no receivable items" ); }
       else
        { // qWarning( "ProtocolActor::protocolSet() %s %d receivable items", actor.data(), bvc->size() );
          if ( receivableItemDefs )
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

void  ProtocolActor::connectSendableItems()
{ Utf8String actor = dict.nameFromCode( actTyp );
  QList<RiceyInt> keys = sendableItemDefs->keys();
  foreach ( RiceyInt key, keys )
    { BlockValueObject *si = (BlockValueObject *)sendableItemDefs->value( key );
      if ( si == nullptr )
        qWarning( "%s sendable item nullptr", actor.data() );
       else
        { Utf8String siName = dict.nameFromCode( si->vKey() );
          if ( !si->contains( RCD_ItemStructure_o ) )
            qWarning( "%s sendable %s lacks structure", actor.data(), siName.data() );
           else
            { BlockValueObject *itSt = (BlockValueObject *)si->value( RCD_ItemStructure_o );
              if ( itSt == nullptr )
                qWarning( "%s sendable ItemStructure %s nullptr", actor.data(), siName.data() );
               else
                { QList<RiceyInt> itStKeys = itSt->keys();
                  if ( itStKeys.size() != 1 )
                    qWarning( "%s sendable ItemStructure %s %lld keys, should be 1", actor.data(), siName.data(), itStKeys.size() );
                   else
                    { if ( key != itStKeys.at(0) )
                        qWarning( "%s sendable ItemStructure %s key mismatch %llx vs %llx", actor.data(), siName.data(), key, itStKeys.at(0) );
                       else
                        sendableObTypes.append( key );
                    }
                }
            } // si contains ItemStructure_o
          if ( !si->contains( RCD_ItemContents_O ) )
            qWarning( "%s sendable ItemContents %s lacks contents", actor.data(), siName.data() );
           else
            { BlockValueObjectArray *icA = (BlockValueObjectArray *)si->value( RCD_ItemContents_O );
              if ( icA == nullptr )
                qWarning( "%s sendable ItemContents %s nullptr", actor.data(), siName.data() );
               else
                { QList<BlockObjectMap> icObs = icA->value();
                  foreach ( BlockObjectMap icOm, icObs )
                    { if ( icOm.contains( RCD_OpDataLink_C ) )
                        { BlockValueRiceyCodeArray *opDataLink = (BlockValueRiceyCodeArray *)icOm[RCD_OpDataLink_C];
                          if ( opDataLink->size() < 2 )
                            qWarning( "%s sendable ItemContents %s not enough items %d", actor.data(), siName.data(), opDataLink->size() );
                           else
                            sendableContents.append( riceToInt( opDataLink->at(0) ) );
                        }
                    }
                }
            } // si contains ItemContents_O
          // qWarning( "%s sendable %s checked.", actor.data(), siName.data() );
        } // si not nullptr
    } // foreach key in keys
}

void  ProtocolActor::connectReceivableItems()
{ Utf8String actor = dict.nameFromCode( actTyp );
  QList<RiceyInt> keys = receivableItemDefs->keys();
  foreach ( RiceyInt key, keys )
    { BlockValueObject *ri = (BlockValueObject *)receivableItemDefs->value( key );
      if ( ri == nullptr )
        qWarning( "%s receivable item nullptr", actor.data() );
       else
        { Utf8String siName = dict.nameFromCode( ri->vKey() );
          if ( !ri->contains( RCD_ItemStructure_o ) )
            qWarning( "%s receivable %s lacks structure", actor.data(), siName.data() );
          else
           { BlockValueObject *itSt = (BlockValueObject *)ri->value( RCD_ItemStructure_o );
             if ( itSt == nullptr )
               qWarning( "%s receivable ItemStructure %s nullptr", actor.data(), siName.data() );
              else
               { QList<RiceyInt> itStKeys = itSt->keys();
                 if ( itStKeys.size() != 1 )
                   qWarning( "%s receivable ItemStructure %s %lld keys, should be 1", actor.data(), siName.data(), itStKeys.size() );
                  else
                   { if ( key != itStKeys.at(0) )
                       qWarning( "%s receivable ItemStructure %s key mismatch %llx vs %llx", actor.data(), siName.data(), key, itStKeys.at(0) );
                      else
                       receivableObTypes.append( key );
                   }
               }
           } // ri contains ItemStructure_o
          if ( !ri->contains( RCD_ItemContents_O ) )
            qWarning( "%s receivable %s lacks contents", actor.data(), siName.data() );
           else
            { BlockValueObjectArray *icA = (BlockValueObjectArray *)ri->value( RCD_ItemContents_O );
              if ( icA == nullptr )
                qWarning( "%s receivable ItemContents %s nullptr", actor.data(), siName.data() );
               else
                { QList<BlockObjectMap> icObs = icA->value();
                  foreach ( BlockObjectMap icOm, icObs )
                    { if ( icOm.contains( RCD_OpDataLink_C ) )
                        { BlockValueRiceyCodeArray *opDataLink = (BlockValueRiceyCodeArray *)icOm[RCD_OpDataLink_C];
                          if ( opDataLink->size() < 2 )
                            qWarning( "%s receivable ItemContents %s not enough items %d", actor.data(), siName.data(), opDataLink->size() );
                           else
                            { receivableContents.append( riceToInt( opDataLink->at(0) ) );
                              // qWarning( "%s receivable ItemContents %s added %llx", actor.data(), siName.data(), riceToInt( opDataLink->at(0) ) );
                            }
                        }
                    }
                }
            } // ri contains ItemContents_O
          // qWarning( "%s receivable %s checked.", actor.data(), siName.data() );
        }
    }
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
{ BaoSerial bao;
  if ( !sendableItemDefs->contains( obType ) )
    { qWarning( "ProtocolActor::compose( %llx ) not found in sendableIdemDefs.", obType );
      return bao;
    }
  BlockValueObject    itemDef( sendableItemDefs->value( obType )             ->bao() );
  BlockValueObject itemStruct( itemDef          .value( RCD_ItemStructure_o )->bao() );
  BlockValueObject         ob( itemStruct       .value( obType )             ->bao() );
  BlockValueObjectArray itemContents; itemContents.setBao( itemDef.value( RCD_ItemContents_O )->bao() );
  for ( qint32 i = 0; i < itemContents.size(); i++ )
    { BlockObjectMap contents = itemContents.at(i);
      if ( !contents.contains( RCD_OpDataLink_C ) )
        { qWarning( "unexpected: contents[%d] doesn't have an OpDataLink_C", i );
        }
       else
        { BlockValueRiceyCodeArray dataLink; dataLink.setBao( contents[RCD_OpDataLink_C]->bao() );
          qWarning( "%s", dataLink.json().data() );
        }
    }
  KeyValuePair kvp( obType, &ob );
  return kvp.bao();
}
