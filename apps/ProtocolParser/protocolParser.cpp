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
#include "protocolParser.h"

/**
 * @brief ProtocolParser::isValid
 * @return true if the passed protocol is valid
 */
bool  ProtocolParser::isValid()
{ if (( pr->key() != RCD_ProtocolDef_o ) &&
      ( pr->key() != RCD_ProtocolDef_O ))
    return false;
  if ( name().size() < 1 )
    return false;
  // TODO: more checks as needed
  return true;
}

/**
 * @brief ProtocolParser::name
 * @return name of the protocol, or empty string if name is not found.
 */
Utf8String  ProtocolParser::name()
{ if ( pr->key() == RCD_ProtocolDef_o )
    { BlockValueObject *bvo = (BlockValueObject *)pr->value();
      if ( !bvo->contains( RCD_hash_o ) )     return "1";
      bvo = (BlockValueObject *)bvo->value( RCD_hash_o );
      if ( !bvo->contains( RCD_hashInfo_o ) ) return "2";
      bvo = (BlockValueObject *)bvo->value( RCD_hashInfo_o );
      if ( !bvo->contains( RCD_text_s ) )     return "3";
      BlockValueString *bvs = (BlockValueString *)bvo->value( RCD_text_s );
      return bvs->value();
    }
  if ( pr->key() == RCD_ProtocolDef_O )
    { BlockValueObjectArray *bvo = (BlockValueObjectArray *)pr->value();
      if ( bvo->size() < 1 )                        return "A1";
      if ( !bvo->at(0).contains( RCD_hash_O ) )     return "A2";
      bvo = (BlockValueObjectArray *)bvo->at(0).value( RCD_hash_O );
      if ( bvo->size() != 1 )                       return "A3";
      if ( !bvo->at(0).contains( RCD_hashInfo_O ) ) return "A4";
      bvo = (BlockValueObjectArray *)bvo->at(0).value( RCD_hashInfo_O );
      if ( bvo->size() != 1 )                       return "A5";
      if ( !bvo->at(0).contains( RCD_text_S ) )     return "A6";
      BlockValueStringArray *bvs = (BlockValueStringArray *)bvo->at(0).value( RCD_text_S );
      if ( bvs->size() != 1 )                       return "A7";
      return bvs->at(0);
    }
  return "0";
}
