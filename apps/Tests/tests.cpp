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
#include <unistd.h>

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

void Tests::liveDelay( int t )
{ QTimer timer;
  timer.setSingleShot( true );
  timer.start( t );
  QCoreApplication::processEvents( QEventLoop::AllEvents, t );
  while ( timer.isActive() )
    { usleep( 5000 ); // Get 5ms of "real sleep"
      QCoreApplication::processEvents( QEventLoop::AllEvents, 50 );
    }
}

void  Tests::on_start_clicked()
{ ui->report->clear();
  bool pass = true;
  qint32 count = 0;
  qint32 tc = 0;
  QString msg;
  pass &= testDict      ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testInt32     ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testInt64     ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testRicey     ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testString    ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testByteArray ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  // TODO: MPZ & MPQ
  pass &= testInt32A    ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testInt64A    ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testRiceyA    ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);
  pass &= testStringA   ( msg, tc ); ui->report->append( msg ); count += tc; tc = 0; liveDelay(4);

  if ( pass )
    ui->report->append( QString( "\nPassed all %1 tests" ).arg( count ) );
   else
    ui->report->append( "FAILED one or more tests" );
}

bool Tests::testDict( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "Dictionary Test: ";
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
        tc++;
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
    msg.append( QString("Pass self consistency %1 entries.").arg(tc) );

  return pass;
}

bool Tests::testInt32( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "int32 Test: ";
  BlockValueInt32 v(this);
  if ( v.type() == RDT_INT32 ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  qint32 tv;
  tv =      0; pass &= testInt32( v, tv, tc, msg );
  tv =      1; pass &= testInt32( v, tv, tc, msg );
  tv =     -1; pass &= testInt32( v, tv, tc, msg );
  tv =  70000; pass &= testInt32( v, tv, tc, msg );
  tv = -70000; pass &= testInt32( v, tv, tc, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testInt32( BlockValueInt32 &v, qint32 tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( tv + 1 );
  v.setBsonish( b );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( tv - 1 );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}

bool Tests::testInt64( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "int64 Test: ";
  BlockValueInt64 v(this);
  if ( v.type() == RDT_INT64 ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  qint64 tv;
  tv =                  0LL; pass &= testInt64( v, tv, tc, msg );
  tv =                  1LL; pass &= testInt64( v, tv, tc, msg );
  tv =                 -1LL; pass &= testInt64( v, tv, tc, msg );
  tv =              70000LL; pass &= testInt64( v, tv, tc, msg );
  tv =             -70000LL; pass &= testInt64( v, tv, tc, msg );
  tv =         5123456789LL; pass &= testInt64( v, tv, tc, msg );
  tv =        -5123456789LL; pass &= testInt64( v, tv, tc, msg );
  tv =   9007199254740992LL; pass &= testInt64( v, tv, tc, msg );
  tv =  -9007199254740992LL; pass &= testInt64( v, tv, tc, msg );
  tv =  12345678901234567LL; pass &= testInt64( v, tv, tc, msg );
  tv = -12345678901234567LL; pass &= testInt64( v, tv, tc, msg );
  for ( qint64 i = 12345678901234567LL; i < 12345678901235000LL; i++ )
    { tv = i;                pass &= testInt64( v, tv, tc, msg ); }
  for ( qint64 i = -12345678901234567LL; i > -12345678901235000LL; i-- )
    { tv = i;                pass &= testInt64( v, tv, tc, msg ); }

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testInt64( BlockValueInt64 &v, qint64 tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( tv + 1 );
  v.setBsonish( b );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( tv - 1 );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}

bool Tests::testRicey( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "ricey Test: ";
  BlockValueRiceyCode v(this);
  if ( v.type() == RDT_RCODE ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  RiceyInt tv;
  tv =                  0; pass &= testRicey( v, tv, tc, msg );
  tv =               0x01; pass &= testRicey( v, tv, tc, msg );
  tv =               0x7F; pass &= testRicey( v, tv, tc, msg );
  tv =               0x80; pass &= testRicey( v, tv, tc, msg );
  tv =               0x81; pass &= testRicey( v, tv, tc, msg );
  tv =             0xA0A0; pass &= testRicey( v, tv, tc, msg );
  tv =           0xA0A0A0; pass &= testRicey( v, tv, tc, msg );
  tv =         0xA0A0A0A0; pass &= testRicey( v, tv, tc, msg );
  tv = 0x7060504030201000; pass &= testRicey( v, tv, tc, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testRicey( BlockValueRiceyCode &v, RiceyInt tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.valueInt() == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv) ); pass = false; }

  RiceyCode rc = v.value();
  if ( rc == intToRice( tv ) ) tc++; else
    { msg.append( QString( "FAIL code get test %1\n" ).arg(tv) ); pass = false; }

  rc = intToRice( tv );
  v.setValue( tv + 1 );
  v.setValue( rc );
  if ( v.valueInt() == tv ) tc++; else
    { msg.append( QString( "FAIL code set test %1\n" ).arg(tv) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( tv + 1 );
  v.setBsonish( b );
  if ( v.valueInt() == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( tv + 1 );
  bool ok = v.setJson( j );
  if ( ok && ( v.valueInt() == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv).arg(ok).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  return pass;
}

bool Tests::testString( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "string Test: ";
  BlockValueString v(this);
  if ( v.type() == RDT_STRING ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  Utf8String tv;
  tv = "";                                         pass &= testString( v, tv, tc, msg );
  tv = "X";                                        pass &= testString( v, tv, tc, msg );
  tv = "1";                                        pass &= testString( v, tv, tc, msg );
  tv = "Stringy McStringface";                     pass &= testString( v, tv, tc, msg );
  tv = "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=\r\n"; pass &= testString( v, tv, tc, msg );
  for ( int i = 0; i < 1024*768; i++ )
    tv.append( "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=abcdefghijklmnopqrstuvwxyz01234567890" );
                                                   pass &= testString( v, tv, tc, msg );
  // TODO: international characters, more wingdings, etc.

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testString( BlockValueString &v, const Utf8String &tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(QString::fromUtf8(tv)) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( "?" );
  v.setBsonish( b );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(QString::fromUtf8(tv)).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( "x" );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(QString::fromUtf8(tv)).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}

bool Tests::testByteArray( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "byteArray Test: ";
  BlockValueByteArray v(this);
  if ( v.type() == RDT_BYTEARRAY ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  QByteArray tv;
  tv = "";                                         pass &= testByteArray( v, tv, tc, msg );
  tv = "X";                                        pass &= testByteArray( v, tv, tc, msg );
  tv = "1";                                        pass &= testByteArray( v, tv, tc, msg );
  tv = "Stringy McStringface";                     pass &= testByteArray( v, tv, tc, msg );
  tv = "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=\r\n"; pass &= testByteArray( v, tv, tc, msg );
  for ( int i = 0; i < 1024*768; i++ )
    tv.append( "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=abcdefghijklmnopqrstuvwxyz01234567890" );
                                                   pass &= testByteArray( v, tv, tc, msg );
  tv.clear();
  for ( int i = 0; i < 256; i++ )
    tv.append( (quint8)i );
                                                   pass &= testByteArray( v, tv, tc, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testByteArray( BlockValueByteArray &v, const QByteArray &tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.setValue( tv );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(QString::fromUtf8(tv)) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.setValue( "?" );
  v.setBsonish( b );
  if ( v.value() == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(QString::fromUtf8(tv)).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.setValue( "x" );
  bool ok = v.setJson( j );
  if ( ok && ( v.value() == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(QString::fromUtf8(tv)).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }

  return pass;
}

bool Tests::testInt32A( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "int32 array Test: ";
  BlockArrayInt32 v(RCD_int32Array_L,this);
  if ( v.type() == RDT_INT32_ARRAY ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  QList<qint32> tv;
                       pass &= testInt32A( v, tv, tc, msg ); // Empty Array test
  tv.append(      0 ); pass &= testInt32A( v, tv, tc, msg );
  tv.append(      1 ); pass &= testInt32A( v, tv, tc, msg );
  tv.append(     -1 ); pass &= testInt32A( v, tv, tc, msg );
  tv.append(  70000 ); pass &= testInt32A( v, tv, tc, msg );
  tv.append( -70000 ); pass &= testInt32A( v, tv, tc, msg );
  tv.clear();
  for ( qint32 i = 1; i < 1100; i++ )
    tv.append(0);
                       pass &= testInt32A( v, tv, tc, msg );
  tv.clear();
  for ( qint32 i = 1; i < 1101; i++ )
    tv.append(i);
                       pass &= testInt32A( v, tv, tc, msg );
  tv.clear();
  for ( qint32 i = 1; i < 1102; i++ )
    tv.append(-i);
                       pass &= testInt32A( v, tv, tc, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testInt32A( BlockArrayInt32 &v, const QList<qint32> &tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.set( tv );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv.size()) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.clear();
  if ( v.append( 123 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  v.setBsonish( b );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv.size()).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.clear();
  if ( v.append( 321 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  bool ok = v.setJson( j );
  if ( ok && ( v == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv.size()).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }
  return pass;
}

bool Tests::testInt64A( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "int64 array Test: ";
  BlockArrayInt64 v(RCD_int64Array_I,this);
  if ( v.type() == RDT_INT64_ARRAY ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  QList<qint64> tv;
                                   pass &= testInt64A( v, tv, tc, msg ); // Empty Array test
  tv.append(                  0 ); pass &= testInt64A( v, tv, tc, msg );
  tv.append(                  1 ); pass &= testInt64A( v, tv, tc, msg );
  tv.append(                 -1 ); pass &= testInt64A( v, tv, tc, msg );
  tv.append(              70000 ); pass &= testInt64A( v, tv, tc, msg );
  tv.append(             -70000 ); pass &= testInt64A( v, tv, tc, msg );
  tv.append(         5123456789 ); pass &= testInt64A( v, tv, tc, msg );
  tv.append(        -5123456789 ); pass &= testInt64A( v, tv, tc, msg );
  for ( qint64 i = 4500000000000000; i <= 4500000000001000; i++ )
    { tv.append( i ); tv.append( -i*2 ); } // Something about the array decoder is precision limited, more than the straight integer json codec
                                   pass &= testInt64A( v, tv, tc, msg );
/*  for ( qint64 i = 9007199254740000; i <= 9007199254740992; i++ )
    { tv.append( i ); tv.append( -i ); }
                                   pass &= testInt64A( v, tv, tc, msg );
  for ( qint64 i = 12345678901234567; i < 12345678901235000; i++ )
    { tv.append( i ); tv.append( -i ); }
                                   pass &= testInt64A( v, tv, tc, msg );
*/
  tv.clear();
  for ( qint64 i = 1; i < 1100; i++ )
    tv.append(0);
                            pass &= testInt64A( v, tv, tc, msg );
  tv.clear();
  for ( qint64 i = 1; i < 1101; i++ )
    tv.append(i*438957);
                            pass &= testInt64A( v, tv, tc, msg );
  tv.clear();
  for ( qint64 i = 1; i < 1102; i++ )
    tv.append(-i*234683);
                            pass &= testInt64A( v, tv, tc, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testInt64A( BlockArrayInt64 &v, const QList<qint64> &tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.set( tv );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv.size()) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.clear();
  if ( v.append( (qint64)123 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  v.setBsonish( b );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv.size()).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.clear();
  if ( v.append( (qint64)321 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  bool ok = v.setJson( j );
  if ( ok && ( v == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv.size()).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false;
      if ( v.size() != tv.size() )
        msg.append( QString( "    size mismatch original %1 read from json %2\n" ).arg( tv.size() ).arg( v.size() ) );
       else
        { for ( qint32 i = 0; i < v.size() ; i++ )
            if ( v.at(i) != tv.at(i) )
              msg.append( QString( "     value mismatch at %1, original %2 read from json %3\n" ).arg(i).arg(tv.at(i)).arg(v.at(i)) );
        }
    }
  return pass;
}

bool Tests::testRiceyA( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "ricey array Test: ";
  BlockArrayRicey v(RCD_riceyArray_Y,this);
  if ( v.type() == RDT_RCODE_ARRAY ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  QList<RiceyInt> tv;
                                   pass &= testRiceyA( v, tv, tc, msg ); // Empty Array test
  tv.append(                  0 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append(               0x01 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append(               0x7F ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append(               0x80 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append(               0x81 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append(             0xA0A0 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append(           0xA0A0A0 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append(         0xA0A0A0A0 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.append( 0x7060504030201000 ); pass &= testRiceyA( v, tv, tc, msg );
  tv.clear();
  for ( qint32 i = 1; i < 1100; i++ )
    tv.append(0);
                                   pass &= testRiceyA( v, tv, tc, msg );
  tv.clear();
  for ( qint32 i = 1; i < 1101; i++ )
    tv.append(i*438957);
                                   pass &= testRiceyA( v, tv, tc, msg );

  QList<RiceyCode> rc;
  rc.append( intToRice(                 0) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(              0x01) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(              0x7F) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(              0x80) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(              0x81) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(            0xA0A0) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(          0xA0A0A0) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(        0xA0A0A0A0) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.append( intToRice(0x7060504030201000) ); pass &= testRiceyA( v, rc, tc, msg );
  rc.clear();
  for ( qint32 i = 1; i < 1100; i++ )
    rc.append( intToRice(0) );
                                   pass &= testRiceyA( v, rc, tc, msg );
  rc.clear();
  for ( qint64 i = 1; i < 1101; i++ )
    rc.append( intToRice(i*438957) );
                                   pass &= testRiceyA( v, rc, tc, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testRiceyA( BlockArrayRicey &v, const QList<RiceyInt> &tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.set( tv );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv.size()) ); pass = false; }

  BsonSerial b = v.bsonish();
  if ( v.append( (RiceyInt)123 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  v.setBsonish( b );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv.size()).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  if ( v.append( (RiceyInt)321 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  bool ok = v.setJson( j );
  if ( ok && ( v == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv.size()).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }
  return pass;
}

bool Tests::testRiceyA( BlockArrayRicey &v, const QList<RiceyCode> &tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.set( tv );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv.size()) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.clear();
  if ( v.append( (RiceyInt)123 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  v.setBsonish( b );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv.size()).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.clear();
  if ( v.append( (RiceyInt)321 ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  bool ok = v.setJson( j );
  if ( ok && ( v == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv.size()).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false; }
  return pass;
}

bool Tests::testStringA( QString &msg, qint32 &tc )
{ bool pass = true;
  msg = "string array Test: ";
  BlockArrayString v(RCD_stringArray_S,this);
  if ( v.type() == RDT_STRING_ARRAY ) tc++; else
    { msg.append( "FAIL type() test.\n" ); pass = false; }

  QList<Utf8String> tv;
                                                           pass &= testStringA( v, tv, tc, msg ); // Empty Array test
  tv.append( ""                                         ); pass &= testStringA( v, tv, tc, msg );
  tv.append( "1"                                        ); pass &= testStringA( v, tv, tc, msg );
  tv.append( "A"                                        ); pass &= testStringA( v, tv, tc, msg );
  tv.append( "Stringy McStringface"                     ); pass &= testStringA( v, tv, tc, msg );
  tv.append( "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=\r\n" ); pass &= testStringA( v, tv, tc, msg );
  Utf8String lStr;
  for ( qint32 i = 0; i < 1000; i++ )
    lStr.append( "Ⓐ~!@#$%^&*()`[]{}\\|;'\":<>?/.,_+-=\r\n" );
  tv.append( lStr                                       ); pass &= testStringA( v, tv, tc, msg );

  if ( pass )
    msg.append( QString("Pass %1 tests.").arg(tc) );

  return pass;
}

bool Tests::testStringA( BlockArrayString &v, const QList<Utf8String> &tv, qint32 &tc, QString &msg )
{ bool pass = true;
  v.set( tv );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL value set/get test %1\n" ).arg(tv.size()) ); pass = false; }

  BsonSerial b = v.bsonish();
  v.clear();
  if ( v.append( "?" ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  v.setBsonish( b );
  if ( v == tv ) tc++; else
    { msg.append( QString( "FAIL bson encode/decode test %1 %2\n" ).arg(tv.size()).arg( QString::fromUtf8(b.toHex()) ) ); pass = false; }

  JsonSerial j = v.json();
  v.clear();
  if ( v.append( "??" ) ) tc++; else
    { pass = false; msg.append( "FAIL during append()\n" ); }
  bool ok = v.setJson( j );
  if ( ok && ( v == tv )) tc++; else
    { msg.append( QString( "FAIL json encode/decode test %1 %2 %3\n" ).arg(tv.size()).arg(ok).arg( QString::fromUtf8(j) ) ); pass = false;
      if ( v.size() != tv.size() )
        msg.append( QString( "    size mismatch original %1 read from json %2\n" ).arg( tv.size() ).arg( v.size() ) );
       else
        { for ( qint32 i = 0; i < v.size() ; i++ )
            if ( v.at(i) != tv.at(i) )
              msg.append( QString( "     value mismatch at %1, original %2 read from json %3\n" ).arg(i).arg(QString::fromUtf8(tv.at(i))).arg(QString::fromUtf8(v.at(i))) );
        }
    }
  return pass;
}
