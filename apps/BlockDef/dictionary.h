#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "riceyCodes.h"
#include <QJsonArray>
#include <QHash>

class Dictionary
{
public:
                           Dictionary();
                     void  read();
                     void  clear();
                     void  reread();
                     void  interpret( const QByteArray & );
                     bool  codesContainName( QByteArray n ) { return ciByName .contains(n); }
                     bool  codesContainCode( QByteArray c ) { return ciByRicey.contains(c); }
                     bool  codesContainCode( qint64 c )     { return ciByNum    .contains(c); }
               QByteArray  nameFromCode( QByteArray );
               QByteArray  nameFromCode( qint64 );
               QByteArray  riceyFromCodeName( QByteArray );
                   qint64  codeFromCodeName( QByteArray );

               QJsonArray  codes;
  QHash<QByteArray,qint32> ciByName;
  QHash<QByteArray,qint32> ciByRicey;
  QHash<qint64    ,qint32> ciByNum;
               QJsonArray  types;
};

#endif // DICTIONARY_H
