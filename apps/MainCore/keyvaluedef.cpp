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
#include "keyvaluedef.h"
#include <QFile>

KeyValueDef::KeyValueDef( const QJsonObject &jo, QObject *parent) : QObject(parent)
{ fromJsonObject( jo ); }

QJsonObject  KeyValueDef::toJsonObject() const
{ QJsonObject jo;
  jo.insert( "key" , key  );
  jo.insert( "desc", desc );
  jo.insert( "type", tc   );
  return jo;
}


void  KeyValueDef::fromJsonObject( const QJsonObject &jo )
{ if ( jo.contains( "key"  ) ) key  = jo.value( "key"  ).toInt();
  if ( jo.contains( "desc" ) ) desc = jo.value( "desc" ).toString();
  if ( jo.contains( "type" ) ) tc   = jo.value( "type" ).toInt();
}


KeyValueDefinitions::KeyValueDefinitions( const QString &filename, QObject *parent )
  : QObject( parent )
{ fromFile( filename ); }

void KeyValueDefinitions::fromFile( const QString &filename )
{ kvds.clear();
  QFile file( filename );
  if ( !file.exists() )
    return;
  // TODO: read defined kvds from file
}
