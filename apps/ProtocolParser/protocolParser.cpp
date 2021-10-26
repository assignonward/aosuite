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

// TODO: instead of coding navigation into the protocol, make it data driven
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

      // TODO: read the item definitions into a more convenient structure


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
          sendableItemDefs = new BlockValueObject( this );
          items = bvc->value();
          foreach( RiceyCode item, items )
            { Utf8String itNam = dict.nameFromCode( item );
              if ( !bvi->contains( item ) )
                { qWarning( "%s sendable item %s not found", actor.data(), itNam.data() );
                }
               else
                { if ( sendableItemDefs->insert( riceToInt(item), ((BlockValueObject *)(bvi->value( item )))->value() ) )
                    qWarning( "%s sendable item %s successfully inserted", actor.data(), itNam.data() );
                   else
                    qWarning( "%s sendable item %s failed to insert", actor.data(), itNam.data() );
                }
            }
        }

      if ( !bvo->contains( RCD_receivableItems_C ) ) { qWarning( "ProtocolActor::protocolSet() Error 10" ); return; }
      bvc = (BlockValueRiceyCodeArray *)bvo->value( RCD_receivableItems_C );
      if ( bvc->size() < 1 )                         { qWarning( "ProtocolActor::protocolSet() Error 11 - no receivable items" ); }
       else
        { // qWarning( "ProtocolActor::protocolSet() %s %d receivable items", actor.data(), bvc->size() );
          if ( receivableItemDefs )
            receivableItemDefs->deleteLater();
          receivableItemDefs = new BlockValueObject( this );
          items = bvc->value();
          foreach( RiceyCode item, items )
            { Utf8String itNam = dict.nameFromCode( item );
              if ( !bvi->contains( item ) )
                { qWarning( "%s receivable item %s not found", actor.data(), itNam.data() ); }
               else
                { if ( receivableItemDefs->insert( riceToInt(item), ((BlockValueObject *)(bvi->value( item )))->value() ) )
                    qWarning( "%s receivable item %s successfully inserted", actor.data(), itNam.data() );
                   else
                    qWarning( "%s receivable item %s failed to insert", actor.data(), itNam.data() );
                }
            }
        }
    }

  emit transactionRecord( QString( "Protocol set: %1" ).arg( QString::fromUtf8( pp->name() ) ) ); // TODO: maybe add a few descriptives...
}
