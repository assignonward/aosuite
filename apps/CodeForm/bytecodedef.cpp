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
#include "bytecodedef.h"
#include "varsizecode.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTextStream>

void  ByteCodeDef::fromJsonObject( const QJsonObject &jo )
{ gbcr = "";
  sepr = false;
  if ( jo.contains( "code" ) ) code = jo.value( "code" ).toInt();
  if ( jo.contains( "desc" ) ) desc = jo.value( "desc" ).toString();
  if ( jo.contains( "gbcr" ) ) gbcr = jo.value( "gbcr" ).toString();
  if ( jo.contains( "sepr" ) ) sepr = jo.value( "sepr" ).toBool();
  if ( jo.contains( "size" ) ) sz   = jo.value( "size" ).toInt();
  if ( jo.contains( "type" ) ) tn   = jo.value( "type" ).toString();
  if ( jo.contains( "pdef" ) ) pdef = jo.value( "pdef" ).toString();
}

QJsonObject  ByteCodeDef::toJsonObject() const
{ QJsonObject jo;
  jo.insert( "code", QJsonValue( (qint64)code ) );
  jo.insert( "desc", QJsonValue(         desc ) );
  if ( gbcr.size() )
  jo.insert( "gbcr", QJsonValue(         gbcr ) );
  if ( sepr )
  jo.insert( "sepr", QJsonValue(         sepr ) );
  jo.insert( "size", QJsonValue(    (int)sz   ) );
  jo.insert( "type", QJsonValue(         tn   ) );
  jo.insert( "pdef", QJsonValue(         pdef ) );
  return jo;
}

QString ByteCodeDef::toDefine( qint32 maxLenPdef )
{ return QString( "#define %1%2 0x%3 // (%4+%5) %6:%7%8" )
           .arg( pdef )
           .arg( QString( maxLenPdef - pdef.size(), QChar(' ') ) )
           .arg( code, 2, 16, QChar('0') )
           .arg( QString::fromUtf8( VarSizeCode::codeToBytes(code).toHex() ) )
           .arg( (sz<0) ? "var" : QString::number(sz) )
           .arg( tn )
           .arg( sepr ? "separable " : " " )
           .arg( desc );
}

QString ByteCodeDef::toCase( qint32 maxLenPdef )
{ return QString( "      case %1:%3 return new %2( di, p );" )
           .arg( pdef ).arg( tn )
           .arg( QString( maxLenPdef - pdef.size(), QChar(' ') ) );
}

QString ByteCodeDef::toCaseDataItem( qint32 maxLenPdef )
{ return QString( "      case %1:%3 return new %2( *((%2 *)ditm), p );" )
           .arg( pdef ).arg( tn )
           .arg( QString( maxLenPdef - pdef.size(), QChar(' ') ) );
}

QString ByteCodeDef::toSizeCase( qint32 maxLenPdef )
{ return QString( "      case %1:%3 return %2;" )
           .arg( pdef ).arg( sz )
           .arg( QString( maxLenPdef - pdef.size(), QChar(' ') ) );
}

///////////////////////////////////////////////////////////////////////////////

void ByteCodeDefinitions::fromFile( const QString &filename )
{ bcdList.clear();
  QFile file( filename );
  if ( !file.open( QIODevice::ReadOnly ) )
    { qDebug( "Could not open %s %s", qPrintable( file.fileName() ), qPrintable( file.errorString() ) );
      return;
    }
  QJsonDocument doc = QJsonDocument::fromJson( file.readAll() );
  if ( !doc.isArray() )
    { // TODO: log error
      return;
    }
  QJsonArray ja = doc.array();
  foreach( const QJsonValue &jv, ja )
    bcdList.append( ByteCodeDef( jv.toObject() ) );
}

QString ByteCodeDefinitions::toString() const
{ QJsonDocument doc;
  QJsonArray ja;
  foreach( ByteCodeDef bcd, bcdList )
    ja.append( bcd.toJsonObject() );
  doc.setArray( ja );
  return QString::fromUtf8( doc.toJson() );
}
