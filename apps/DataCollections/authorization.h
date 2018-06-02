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
#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QObject>
#include "assignment.h"
#include "data16.h"
#include "datavarlength.h"
#include "signature.h"

/**
 * @brief The Authorization class - when
 *   complete and valid, contains a description of
 *   the basic assignment contract between all the participants
 *   without the underwriting and recording - only the RBID is specified
 *   which describes the maximum commission payable to
 *   the sum of all underwriters, chain-maker, and recording tax.
 */
class Authorization : public DataVarLength
{
    Q_OBJECT
public:
    explicit Authorization( const DataItemBA &di = DataItemBA(), QObject *p = NULL );
             Authorization( const Authorization &a, QObject *p = NULL )
               : DataVarLength( AO_AUTHORIZATION, p ? p : a.parent() ),
                 assignment( a.assignment ), sigs( a.sigs ) {}
       void  operator = ( const DataItemBA &di );
 DataItemBA  toDataItem( bool cf = false );

private:
         Assignment  assignment;
    QList<Signature> sigs;   // Same length as the participants list in the assignment, index numbers match signature to participant
             Data16  nSigs;  // Used primarily as a check during serialization and deserialization
};

#endif // AUTHORIZATION_H
