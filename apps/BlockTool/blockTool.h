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
#ifndef BLOCKTOOL_H
#define BLOCKTOOL_H

#include <QScrollArea>
#include <QProcess>
#include "blockOb.h"
#include "blockPanel.h"

namespace Ui {
class BlockTool;
}

class BlockTool : public QScrollArea
{
    Q_OBJECT

public:
   explicit  BlockTool( QWidget *cw = nullptr );
            ~BlockTool();
       void  liveDelay( int t );
 QByteArray  jsonReformat( QByteArray );

signals:
       void  showA( KeyValuePair * );
       void  showX( KeyValuePair * );
       void  showY( KeyValuePair * );

public slots:
       void  on_chain_clicked();
       void  on_hash_clicked();
       void  on_DAO0_clicked();
       void  on_makeX_toggled(bool);
       void  on_makeY_toggled(bool);
       void  on_buildA_toggled(bool);
       void  on_buildX_toggled(bool);
       void  on_buildY_toggled(bool);
       void  writeWrappedDot( QByteArray d );
       void  updateGraph();
       void  graphvizDone(int,QProcess::ExitStatus);

public:
   QPointer<BlockPanel> panelA;
   QPointer<BlockPanel> panelX;
   QPointer<BlockPanel> panelY;
         Ui::BlockTool *ui;
     QPointer<QProcess> pp;
};

#endif // BLOCKTOOL_H
