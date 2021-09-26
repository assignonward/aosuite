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

qint32 dex = 0;

DotSerial KeyValuePair::dot() const
{ DotSerial d;
  DotSerial kn = dict.nameFromCode(key());
  d.append( "subgraph cluster_"+DotSerial::number( dex++ )+" {\n" );
  d.append( "  label =\""+kn+"\";\n" );
  d.append( "  labeljust = \"l\";\n" );
  d.append( "  margin = 4;\n" );
  d.append( "  fontsize = 12;\n\n" );
  if ( value() )
    { DotSerial v = value()->dot();
      // v.replace(DotSerial("\""), DotSerial(""));
      d.append( v+";\n" );
    }
   else
    d.append( "NULL;\n" );
  d.append( "  }\n" );
  return d;
}

DotSerial KeyValueArray::dot() const
{ DotSerial d;
  DotSerial kn = dict.nameFromCode(key());
  d.append( "subgraph cluster_"+DotSerial::number( dex++ )+" {\n" );
  d.append( "  label =\""+kn+"["+DotSerial::number( size() )+"]\";\n" );
  d.append( "  labeljust = \"l\";\n" );
  d.append( "  margin = 4;\n" );
  d.append( "  fontsize = 12;\n" );
  if ( size() == 0 )
    d.append( "    node_"+DotSerial::number( dex++ )+" [ label=\"\"; shape=plaintext; ];\n" );
  quint8 t = type();
  for ( qint32 i = 0; i < size(); i++ )
    { if ( at(i) )
        { d.append( "subgraph cluster_"+DotSerial::number( dex++ )+" {\n" );
          d.append( "  label =\"["+DotSerial::number(i)+"]\";\n" );
          d.append( "  labeljust = \"l\";\n" );
          d.append( "  margin = 4;\n" );
          d.append( "  fontsize = 10;\n\n" );
            DotSerial v = at(i)->dot();
            if ( ( at(i)->type() & RDT_TYPEMASK ) == RDT_RCODE )
              v.replace(DotSerial("\""), DotSerial(""));
            if ( t == RDT_OBJECT_ARRAY )
              d.append( v+"\n" );
             else
              d.append( "node_"+DotSerial::number( dex++ )
                       +" [ label=\""+v+"\"; shape=box; style=rounded; fontsize=10; ];\n" );
          d.append( "  }\n\n" );
        }
       else
        d.append( "    NULL;\n" );
    }
  d.append( "  }\n" );
  return d;
}

DotSerial BlockValueObject::dot() const
{ DotSerial d;
  QList<RiceyInt>keys = value().keys();
  if ( keys.size() == 0 )
    d.append( "    node_"+DotSerial::number( dex++ )+" [ label=\"\"; shape=plaintext; ];\n" );
   else foreach ( RiceyInt i, keys )
    { bool done = false;
      if ( value(i) )
        if (( value(i)->type() & RDT_ARRAY ) == RDT_ARRAY )
           { d.append( value(i)->dot() );
             done = true;
           }
      if ( !done )
        { DotSerial kn = dict.nameFromCode(i);
          d.append( "subgraph cluster_"+DotSerial::number( dex++ )+" {\n" );
          d.append( "  label =\""+kn+"\";\n" );
          d.append( "  margin = 4;\n" );
          d.append( "  style = rounded;\n" );
          d.append( "  fontsize = 10;\n\n" );
          if ( value(i) )
            { DotSerial v = value(i)->dot();
              quint8 t = value(i)->type();
              //if ( t == RDT_STRING )
              //  v.replace(DotSerial("!"), DotSerial(""));
              if (( t == RDT_STRING ) || ( t == RDT_MPZ ) || ( t == RDT_MPQ ) || ( t == RDT_RCODE ) || ( t == RDT_BYTEARRAY ))
                d.append( "node_"+DotSerial::number( dex++ )
                         +" [ label="+v+"; shape=box; style=rounded; fontsize=10; ];\n" );
               else if (( t == RDT_OBJECT ) || (( t & RDT_ARRAY ) == RDT_ARRAY ))
                d.append( v+"\n" );
               else
                d.append( "node_"+DotSerial::number( dex++ )
                         +" [ label=\""+v+"\"; shape=box; style=rounded; fontsize=10; ];\n" );
            }
           else
            d.append( "node_"+DotSerial::number( dex++ )
                     +" [ label=\"NULL\"; shape=box; style=rounded; fontsize=10; ];\n" );
          d.append( "  }\n" );
        }
    }
  return d;
}
