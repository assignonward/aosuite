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

// Design Intent:
//
// The dot() functions of block object family members are intended to create
//   a read-only graphviz presentation of the block structure, showing object
//   nesting, key labels and data value summaries.  Larger data objects may be
//   abbreviated to convey the structure of larger blocks rather than the
//   absolute complete contents.
//
// When protocols start specifying operations, the intent is for the .dot graphs
//    to include arrows and symbols to illustrate the operands, operations and
//    where the results of the operations appear in the structure.
//

#include "blockOb.h"

/* Calculation edge drawing syntax:
 node_145 -> hashCalc [constraint=false];
 node_138 -> hashCalc [constraint=false ltail=cluster_76];
 node_141 -> hashCalc [constraint=false];
 hashCalc -> node_143;

 or drawn on the right side:

 node_145 -> hashCalc [constraint=false];
 node_138 -> hashCalc [constraint=false ltail=cluster_76];
 node_141 -> hashCalc [constraint=false];
 node_143 -> hashCalc [dir=back];
 */

qint32 dex = 0;

DotSerial ValueBase::dotEmptyNode()
{ return "    node_"+DotSerial::number( dex++ )+" [ label=\"\"; shape=plaintext; ];\n"; }

DotSerial ValueBase::dotName( RiceyInt k )
{ if ( dict.codesContainCode( k ) )
    return dict.nameFromCode(k);
   else
    return intToRice( k ).toHex();
}

DotSerial ValueBase::dotArrayName( RiceyInt k, qint32 sz )
{ return dotName(k) + "["+DotSerial::number( sz )+"]"; }

DotSerial ValueBase::clusterWrap( Mode m, qint32 d, const DotSerial &kn, const DotSerial &v )
{ DotSerial dot;
  dot.append( "subgraph cluster_"+DotSerial::number( dex++ )+" {\n" );
  dot.append( "  label     = "+ensureQuotes( /* DotSerial::number(d)+":"+ */ removeQuotes( kn ) )+";\n" );
  dot.append( "  labeljust = \"l\";\n" );
  dot.append( "  style     = rounded;\n" );
  dot.append( "  "+lineColor( m, d ) );
  dot.append( "  "+bgColor( m, d ) );
  dot.append( "  margin    = 4;\n" );
  dot.append( "  fontsize  = 10;\n\n" );
  if ( v.size() > 0 )
    { if ( !v.trimmed().startsWith( "subgraph" ) && !v.trimmed().startsWith( "node" ) )
        { dot.append( "node_"+DotSerial::number( dex++ )
                   +" [ label="+v+"; "+lineColor(m,d)+" shape=box; style=rounded; fontsize=10; ];\n" );
        }
       else
        { dot.append( v );
          if ( !v.trimmed().endsWith( ";" ) )
            dot.append( ";" );
          dot.append( "\n" );
        }
    }
   else
    dot.append( dotEmptyNode() );
  dot.append( "  }\n" );
  return dot;
}

DotSerial ValueBase::lineColor( Mode m, qint32 depth )
{ switch ( m )
    { case make:  return "color=\""+wheelColor( QColor( "darkgreen"  ), 0.05, 0.15, 0.1, depth)+"\";\n";
      case build: return "color=\""+wheelColor( QColor( "darkblue"   ), 0.05, 0.15, 0.1, depth)+"\";\n";
      case idle:  return "color=\""+wheelColor( QColor( "grey"       ), 0.05, 0.0 , 0.1, depth)+"\";\n";
    }
  return "";
}

DotSerial ValueBase::bgColor( Mode m, qint32 depth )
{ switch ( m )
    { case make:  return "bgcolor=\""+wheelColor( QColor( "mintcream" ), 0.05, 0.05, 0.1, depth)+"\";\n";
      case build: return "bgcolor=\""+wheelColor( QColor( "#F0F8FF"   ), 0.05, 0.05, 0.1, depth)+"\";\n";
      case idle:  return "bgcolor=\""+wheelColor( QColor( "gainsboro" ), 0.05, 0.05, 0.1, depth)+"\";\n";
    }
  return "";
}

DotSerial ValueBase::wheelColor( const QColor &c, qreal hDep, qreal sDep, qreal lDep, qint32 depth )
{ if ( hDep > 0.5 ) hDep = 0.5;
  if ( hDep < 0.0 ) hDep = 0.0;
  if ( sDep > 0.5 ) sDep = 0.5;
  if ( sDep < 0.0 ) sDep = 0.0;
  if ( lDep > 0.5 ) lDep = 0.5;
  if ( lDep < 0.0 ) lDep = 0.0;
  qreal nHue = c.hslHueF();
  qreal nSat = c.hslSaturationF(); if (nSat < sDep) nSat = sDep; if ( (nSat + sDep) > 1.0 ) nSat = 1.0 - sDep;
  qreal nLig = c.lightnessF();     if (nLig < lDep) nLig = lDep; if ( (nLig + lDep) > 1.0 ) nLig = 1.0 - lDep;
  nHue += hDep * sin( ((qreal)depth)* 70.0*6.28318530718/360.0 );
  nSat += sDep * sin( ((qreal)depth)* 25.0*6.28318530718/360.0 );
  nLig += lDep * sin( ((qreal)depth)*130.0*6.28318530718/360.0 );
  while ( nHue > 1.0 ) { nHue -= 1.0; } if ( nHue < 0.0 ) nHue = 0.0;
  if ( nSat > 1.0 ) nSat = 1.0;
  if ( nSat < 0.0 ) nSat = 0.0;
  if ( nLig > 1.0 ) nLig = 1.0;
  if ( nLig < 0.0 ) nLig = 0.0;
  QColor ct; ct.setHslF( nHue, nSat, nLig );
  DotSerial cs;
  cs.append( (quint8)ct.red()   );
  cs.append( (quint8)ct.green() );
  cs.append( (quint8)ct.blue()  );
  return "#"+cs.toHex();
}

DotSerial KeyValuePair::dot(Mode m) const
{ return clusterWrap( m, depth(),      dotName( key() )        , value() ? value()->dot(m) : "" ); }

DotSerial KeyValueArray::dot(Mode m) const
{ return clusterWrap( m, depth(), dotArrayName( key(), size() ), value() ? value()->dot(m) : "" ); }

DotSerial  ValueBaseArray::dot(Mode m) const
{ DotSerial d;
  if ( size() == 0 )
    d.append( dotEmptyNode() );
   else for ( qint32 i = 0; i < size(); i++ )
    d.append( clusterWrap( m, depth()+1, "["+DotSerial::number(i)+"]", at(i) ? at(i)->dot(m) : "" ) );
  return d;
}

DotSerial BlockValueObject::dot(Mode m) const
{ DotSerial d;
  QList<RiceyInt>keys = value().keys();
  if ( keys.size() == 0 )
    d.append( dotEmptyNode() );
   else foreach ( RiceyInt i, keys )
    { if (( value(i)->type() & RDT_ARRAY ) == 0 )
        d.append( clusterWrap( m, depth()+1,      dotName( i )                  , value(i) ? value(i)->dot(m) : "" ) );
       else
        d.append( clusterWrap( m, depth()+1, dotArrayName( i, value(i)->size() ), value(i) ? value(i)->dot(m) : "" ) );
    }
  return d;
}
