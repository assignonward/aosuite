#include "dictionary.h"
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

Dictionary::Dictionary()
{ QFile file( ":/data/dictionary.json" );
  if ( !file.open( QIODevice::ReadOnly ) )
    qDebug( "Cannot open dictionary.json resource for reading." );
   else
    { QJsonDocument jd = QJsonDocument::fromJson( file.readAll() );
      if ( !jd.isObject() )
        qDebug( "dictionary.json does not contain a json object." );
       else
        { QJsonObject jo = jd.object();
          if ( !jo.contains( "riceyCodes_O" ) )
            qDebug( "dictionary.json does not contain a riceyCodes_O array." );
           else if ( !jo["riceyCodes_O"].isArray() )
            qDebug( "riceyCodes_O is not an array." );
           else
            { codes = jo["riceyCodes_O"].toArray();
              qint32 sz = codes.size();
              for ( int i = 0; i < sz; i++ )
                { if ( !codes.at(i).isObject() )
                    qDebug( "riceyCodes_O at %d is not an object.", i );
                   else
                    { QJsonObject co = codes.at(i).toObject();
                      if ( !co.contains("name_s") )
                        qDebug( "riceyCodes_O at %d has no name_s element.", i );
                       else
                        ciByName.insert( co["name_s"].toString(), i );
                      if ( !co.contains("type_y") )
                        qDebug( "riceyCodes_O at %d has no type_y element.", i );
                       else
                        { QByteArray ricey = QByteArray::fromHex( co["type_s"].toString().toUtf8() );
                          ciByRicey.insert( ricey, i );
                          ciByN.insert( riceToInt(ricey), i );
                          // TODO: type checking vs the riceyTypes_O table info
                        }
                    }
                } // for ( int i = 0; i < sz; i++ )
            }
        }
    }
}
