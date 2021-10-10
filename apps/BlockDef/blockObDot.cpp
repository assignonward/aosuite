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

DotSerial  ValueBase::dotEmptyNode()
{ return "    node_"+DotSerial::number( dex++ )+" [ label=\"\"; shape=plaintext; ];\n"; }

DotSerial ValueBase::dotName( RiceyInt k )
{ if ( dict.codesContainCode( k ) )
    return dict.nameFromCode(k);
   else
    return intToRice( k ).toHex();
}

DotSerial ValueBase::dotArrayName( RiceyInt k, qint32 sz )
{ return dotName(k) + "["+DotSerial::number( sz )+"]"; }

DotSerial ValueBase::clusterWrap( DotSerial kn, DotSerial v )
{ DotSerial d;
  kn = ensureQuotes( removeQuotes( kn ) );
  d.append( "subgraph cluster_"+DotSerial::number( dex++ )+" {\n" );
  d.append( "  label     = "+kn+";\n" );
  d.append( "  labeljust = \"l\";\n" );
  d.append( "  style     = rounded;\n" );
  d.append( "  margin    = 4;\n" );
  d.append( "  fontsize  = 10;\n\n" );
  if ( v.size() > 0 )
    { if ( !v.trimmed().startsWith( "subgraph" ) && !v.trimmed().startsWith( "node" ) )
        { d.append( "node_"+DotSerial::number( dex++ )
                   +" [ label="+v+"; shape=box; style=rounded; fontsize=10; ];\n" );
        }
       else
        { d.append( v );
          if ( !v.trimmed().endsWith( ";" ) )
            d.append( ";" );
          d.append( "\n" );
        }
    }
   else
    d.append( dotEmptyNode() );
  d.append( "  }\n" );
  return d;
}

DotSerial KeyValuePair::dot() const
{ return clusterWrap(      dotName( key() )        , value() ? value()->dot() : "" ); }

DotSerial KeyValueArray::dot() const
{ return clusterWrap( dotArrayName( key(), size() ), value() ? value()->dot() : "" ); }

DotSerial  BlockValueArray::dot() const
{ DotSerial d;
  if ( size() == 0 )
    d.append( dotEmptyNode() );
   else for ( qint32 i = 0; i < size(); i++ )
    d.append( clusterWrap( "["+DotSerial::number(i)+"]", at(i) ? at(i)->dot() : "" ) );
  return d;
}

DotSerial BlockValueObject::dot() const
{ DotSerial d;
  QList<RiceyInt>keys = value().keys();
  if ( keys.size() == 0 )
    d.append( dotEmptyNode() );
   else foreach ( RiceyInt i, keys )
    { if (( value(i)->type() & RDT_ARRAY ) == 0 )
        d.append( clusterWrap(      dotName( i )                  , value(i) ? value(i)->dot() : "" ) );
       else
        d.append( clusterWrap( dotArrayName( i, value(i)->size() ), value(i) ? value(i)->dot() : "" ) );
    }
  return d;
}
