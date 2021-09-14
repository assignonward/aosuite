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
#include "tests.h"
#include "ui_tests.h"
#include "blockOb.h"

// TODO: test intentional failures, error checking, length reporting, trailing input data, etc.

Tests::Tests( QWidget *cw ) :
    QScrollArea(cw),
    ui(new Ui::Tests)
{ ui->setupUi(this);
  if ( cw )
    { new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
    }
}

Tests::~Tests()
{ delete ui; }

void  Tests::on_start_clicked()
{ ui->report->clear();
  bool pass = true;
  QString msg;
  pass &= testDict ( msg ); ui->report->append( msg );
  pass &= testInt32( msg ); ui->report->append( msg );
  pass &= testInt64( msg ); ui->report->append( msg );
  pass &= testRicey( msg ); ui->report->append( msg );
  pass &= testString( msg ); ui->report->append( msg );
  pass &= testByteArray( msg ); ui->report->append( msg );

  if ( pass )
    ui->report->append( "Passed all tests" );
   else
    ui->report->append( "FAILED one or more tests" );
}

bool Tests::testDict( QString &msg )
{ bool pass = true;
  msg = "Dictionary Test: ";
  qint32 count = 0;
  QList<RiceyInt> rList = dict.ciByNum.keys();
  foreach ( RiceyInt i, rList )
    { Utf8String name = dict.nameFromCode(i);
        RiceyInt j    = dict.codeFromCodeName( name );
        RiceyInt k    = dict.codeFromCodeName( name );
       RiceyCode code = dict.riceyFromCodeName( name );
      Utf8String cnme = dict.nameFromCode( code );
            bool cc1  = dict.codesContainName( name );
            bool cc2  = dict.codesContainCode( i );
            bool cc3  = dict.codesContainCode( code );
            bool tp   = true;
      if (( j != i ) || ( k != i ) || !cc1 || !cc2 || !cc3 ||
          ( riceToInt(code) != i ) || ( cnme != name ) ||
          ( intToRice(i) != code ))
        pass = tp = false;
      if ( tp )
        count++;
       else
        msg.append( QString( "FAIL i %1 j %2 k %3 cc1 %4 cc2 %5 cc3 %6 code %7 name %8 cnme %9 rti %10 itr %11\n" )
                               .arg(i).arg(j).arg(k).arg(cc1).arg(cc2).arg(cc3)
                               .arg(QString::fromUtf8(code.toHex()))
                               .arg(QString::fromUtf8(name))
                               .arg(QString::fromUtf8(cnme))
                               .arg(riceToInt(code))
                               .arg(QString::fromUtf8(intToRice(i).toHex()))
                            );
    }
  if ( pass )
    msg.append( QString("Pass self consistency %1 entries.").arg(count) );

  return pass;
}

bool Tests::testInt32( QString &msg )
{ bool pass = true;
  msg = "int32 Test: ";
  qint32 count = 0;
  BlockValueInt32 v(this);
  if ( v.type() == RDT_INT32 ) count++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  qint32 tv;
  tv =      0; pass &= testInt32( v, tv, count, msg );
  tv =      1; pass &= testInt32( v, tv, count, msg );
  tv =     -1; pass &= testInt32( v, tv, count, msg );
  tv =  70000; pass &= testInt32( v, tv, count, msg );
  tv = -70000; pass &= testInt32( v, tv, count, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(count) );

  return pass;
}

bool Tests::testInt32( BlockValueInt32 &v, qint32 tv, qint32 &count, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( tv + 1 );
  v.setBsonish( b );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( tv - 1 );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) count++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}

bool Tests::testInt64( QString &msg )
{ bool pass = true;
  msg = "int64 Test: ";
  qint32 count = 0;
  BlockValueInt64 v(this);
  if ( v.type() == RDT_INT64 ) count++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  qint64 tv;
  tv =           0; pass &= testInt64( v, tv, count, msg );
  tv =           1; pass &= testInt64( v, tv, count, msg );
  tv =          -1; pass &= testInt64( v, tv, count, msg );
  tv =       70000; pass &= testInt64( v, tv, count, msg );
  tv =      -70000; pass &= testInt64( v, tv, count, msg );
  tv =  5123456789; pass &= testInt64( v, tv, count, msg );
  tv = -5123456789; pass &= testInt64( v, tv, count, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(count) );

  return pass;
}

bool Tests::testInt64( BlockValueInt64 &v, qint64 tv, qint32 &count, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( tv + 1 );
  v.setBsonish( b );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( tv - 1 );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) count++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}

bool Tests::testRicey( QString &msg )
{ bool pass = true;
  msg = "ricey Test: ";
  qint32 count = 0;
  BlockValueRiceyCode v(this);
  if ( v.type() == RDT_RCODE ) count++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  RiceyInt tv;
  tv =                  0; pass &= testRicey( v, tv, count, msg );
  tv =               0x01; pass &= testRicey( v, tv, count, msg );
  tv =               0x7F; pass &= testRicey( v, tv, count, msg );
  tv =               0x80; pass &= testRicey( v, tv, count, msg );
  tv =               0x81; pass &= testRicey( v, tv, count, msg );
  tv =             0xA0A0; pass &= testRicey( v, tv, count, msg );
  tv =           0xA0A0A0; pass &= testRicey( v, tv, count, msg );
  tv =         0xA0A0A0A0; pass &= testRicey( v, tv, count, msg );
  tv = 0x7060504030201000; pass &= testRicey( v, tv, count, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(count) );

  return pass;
}

bool Tests::testRicey( BlockValueRiceyCode &v, RiceyInt tv, qint32 &count, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.valueInt() == tv ) count++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv) ); pass = false; }

  RiceyCode rc = v.value();
  if ( rc == intToRice( tv ) ) count++; else
    { msg.append( QString( "FAIL code get test %1\n" ).arg(tv) ); pass = false; }

  rc = intToRice( tv );
  v.setValue( tv + 1 );
  v.setValue( rc );
  if ( v.valueInt() == tv ) count++; else
    { msg.append( QString( "FAIL code set test %1\n" ).arg(tv) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( tv + 1 );
  v.setBsonish( b );
  if ( v.valueInt() == tv ) count++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( tv + 1 );
  bool ok = v.setJson( j );
  if ( ok && ( v.valueInt() == tv )) count++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv).arg(ok).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  return pass;
}

bool Tests::testString( QString &msg )
{ bool pass = true;
  msg = "string Test: ";
  qint32 count = 0;
  BlockValueString v(this);
  if ( v.type() == RDT_STRING ) count++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  Utf8String tv;
  tv = "";                                         pass &= testString( v, tv, count, msg );
  tv = "X";                                        pass &= testString( v, tv, count, msg );
  tv = "1";                                        pass &= testString( v, tv, count, msg );
  tv = "Stringy McStringface";                     pass &= testString( v, tv, count, msg );
  tv = "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=\r\n"; pass &= testString( v, tv, count, msg );
  for ( int i = 0; i < 1024*768; i++ )
    tv.append( "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=abcdefghijklmnopqrstuvwxyz01234567890" );
                                                   pass &= testString( v, tv, count, msg );
  // TODO: international characters, more wingdings, etc.

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(count) );

  return pass;
}

bool Tests::testString( BlockValueString &v, const Utf8String &tv, qint32 &count, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(QString::fromUtf8(tv)) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( "?" );
  v.setBsonish( b );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(QString::fromUtf8(tv)).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( "x" );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) count++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(QString::fromUtf8(tv)).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}

bool Tests::testByteArray( QString &msg )
{ bool pass = true;
  msg = "byteArray Test: ";
  qint32 count = 0;
  BlockValueByteArray v(this);
  if ( v.type() == RDT_BYTEARRAY ) count++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  QByteArray tv;
  tv = "";                                         pass &= testByteArray( v, tv, count, msg );
  tv = "X";                                        pass &= testByteArray( v, tv, count, msg );
  tv = "1";                                        pass &= testByteArray( v, tv, count, msg );
  tv = "Stringy McStringface";                     pass &= testByteArray( v, tv, count, msg );
  tv = "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=\r\n"; pass &= testByteArray( v, tv, count, msg );
  for ( int i = 0; i < 1024*768; i++ )
    tv.append( "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=abcdefghijklmnopqrstuvwxyz01234567890" );
                                                   pass &= testByteArray( v, tv, count, msg );
  tv.clear();
  for ( int i = 0; i < 256; i++ )
    tv.append( (quint8)i );
                                                   pass &= testByteArray( v, tv, count, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(count) );

  return pass;
}

bool Tests::testByteArray( BlockValueByteArray &v, const QByteArray &tv, qint32 &count, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(QString::fromUtf8(tv)) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( "?" );
  v.setBsonish( b );
  if ( v.value() == tv ) count++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(QString::fromUtf8(tv)).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( "x" );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) count++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(QString::fromUtf8(tv)).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}
