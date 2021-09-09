#include "dictionary.h"
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

Dictionary::Dictionary()
{ Q_INIT_RESOURCE(blockDef);
  read();
}

void Dictionary::reread()
{ clear();
  read();
}

void Dictionary::clear()
{ while ( !codes.isEmpty() )
    codes.removeLast();
  while ( !types.isEmpty() )
    types.removeLast();
  ciByNum.clear();
  ciByName.clear();
  ciByRicey.clear();
}

void Dictionary::read()
{ QFile file( ":/data/dictionary.json" );
  if ( !file.open( QIODevice::ReadOnly ) )
    qWarning( "Cannot open dictionary.json resource for reading." );
   else
    interpret( file.readAll() );
}

void  Dictionary::interpret( const QByteArray &ba )
{ qint32 sz;
  QJsonDocument jd = QJsonDocument::fromJson( ba );
  if ( !jd.isObject() )
    qWarning( "dictionary.json does not contain a json object." );
   else
    { QJsonObject jo = jd.object();
      if ( !jo.contains( "riceyTypes_O" ) )
        qWarning( "dictionary.json does not contain a riceyTypes_O array." );
       else if ( !jo["riceyTypes_O"].isArray() )
        qWarning( "riceyTypes_O is not an array." );
       else
        { types = jo["riceyTypes_O"].toArray();
          sz = types.size();
          for ( int i = 0; i < sz; i++ )
            { if ( !types.at(i).isObject() )
                qWarning( "riceyTypes_O at %d is not an object.", i );
               else
                {} // TODO: index the types for referencing
            } // for ( int i = 0; i < sz; i++ )
          qDebug( "interpreted %d elements in riceyTypes_O", sz );
        }

      if ( !jo.contains( "riceyCodes_O" ) )
        qWarning( "dictionary.json does not contain a riceyCodes_O array." );
       else if ( !jo["riceyCodes_O"].isArray() )
        qWarning( "riceyCodes_O is not an array." );
       else
        { codes = jo["riceyCodes_O"].toArray();
          sz = codes.size();
          for ( int i = 0; i < sz; i++ )
            { if ( !codes.at(i).isObject() )
                qWarning( "riceyCodes_O at %d is not an object.", i );
               else
                { QJsonObject co = codes.at(i).toObject();
                  if ( !co.contains("name_s") )
                    qWarning( "riceyCodes_O at %d has no name_s element.", i );
                   else
                    ciByName.insert( co["name_s"].toString().toUtf8(), i );
                  if ( !co.contains("type_y") )
                    qWarning( "riceyCodes_O at %d has no type_y element.", i );
                   else
                    { QByteArray ricey = QByteArray::fromHex( co["type_y"].toString().toUtf8() );
                      ciByRicey.insert( ricey, i );
                      ciByNum.insert( riceToInt(ricey), i );
                      // TODO: type checking vs the riceyTypes_O table info
                    }
                }
            } // for ( int i = 0; i < sz; i++ )
          qDebug( "interpreted %d elements in riceyCodes_O", sz );
        }
    }
}

QByteArray  Dictionary::nameFromCode( QByteArray c )
{ if ( !codesContainCode( c ) )
    return QByteArray();
  QJsonValue jv = codes.at(ciByRicey[c]);
  if ( !jv.isObject() )
    return QByteArray();
  QJsonObject jo = jv.toObject();
  if ( !jo.contains("name_s" ) )
    return QByteArray();
  return jo["name_s"].toString().toUtf8();
}

QByteArray  Dictionary::nameFromCode( qint64 c )
{ if ( !codesContainCode( c ) )
    return QByteArray();
  QJsonValue jv = codes.at(ciByNum[c]);
  if ( !jv.isObject() )
    return QByteArray();
  QJsonObject jo = jv.toObject();
  if ( !jo.contains("name_s") )
    return QByteArray();
  return jo["name_s"].toString().toUtf8();
}

QByteArray  Dictionary::riceyFromCodeName( QByteArray n )
{ if ( !codesContainName(n) )
    return QByteArray();
  QJsonValue jv = codes.at(ciByName[n]);
  if ( !jv.isObject() )
    return QByteArray();
  QJsonObject jo = jv.toObject();
  if ( !jo.contains("type_s") )
    return QByteArray();
  return QByteArray::fromHex( jo["type_s"].toString().toUtf8() );
}

qint64  Dictionary::codeFromCodeName( QByteArray n )
{ if ( !codesContainName(n) )
    return -1;
  QJsonValue jv = codes.at(ciByName[n]);
  if ( !jv.isObject() )
    return -1;
  QJsonObject jo = jv.toObject();
  if ( !jo.contains("type_s") )
    return -1;
  return riceToInt( QByteArray::fromHex( jo["type_s"].toString().toUtf8() ) );
}

