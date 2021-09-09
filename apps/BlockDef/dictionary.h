#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "riceyCodes.h"
#include <QJsonArray>
#include <QHash>

class Dictionary
{
public:
                        Dictionary();

               QJsonArray  codes;
     QHash<QString,qint32> ciByName;
  QHash<QByteArray,qint32> ciByRicey;
      QHash<qint64,qint32> ciByN;
               QJsonArray  types;
};

#endif // DICTIONARY_H
