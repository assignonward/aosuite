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
#ifndef ASSETS_H
#define ASSETS_H

#include "keypair.h"
#include "organizer.h"
#include "recorder.h"
#include "sharesref.h"

/**
 * @brief The Assets class - collections of valuable information
 *   for the Asset Organizer.  Includes shares records and contact info.
 */
class Assets : public DataVarLength
{
    Q_OBJECT
public:
    explicit  Assets( const DataItemBA &di = DataItemBA(), QObject *p = NULL );
              Assets( const Assets &a, QObject *p = NULL )
                : DataVarLength( AO_ASSETS, p ? p : a.parent() ), organizers( a.organizers ),
                  recorders( a.recorders ), sharesRefs( a.sharesRefs ), keyPairs( a.keyPairs ) {}
        void  operator = ( const DataItemBA &di );
  DataItemBA  toDataItem( bool cf = false );

private:
    QList<Organizer> organizers;
    QList< Recorder> recorders;
    QList<SharesRef> sharesRefs;
    QList<  KeyPair> keyPairs;     // Previously unused key pairs, for quick access
};

#endif // ASSETS_H
