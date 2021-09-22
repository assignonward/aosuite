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
#include "blockTool.h"
#include "ui_blockTool.h"
#include "blockOb.h"

BlockTool::BlockTool( QWidget *cw ) :
    QScrollArea(cw),
    ui(new Ui::BlockTool)
{ ui->setupUi(this);
  if ( cw )
    { new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
    }
}

BlockTool::~BlockTool()
{ delete ui; }

void  BlockTool::on_start_clicked()
{ ui->report->clear();
  ui->report->append( "FAILED one or more tests" );

  QFile fs(":/files/x.dot");
  QFile fd("/tmp/x.dot");
  fs.open( QIODevice::ReadOnly );
  fd.open( QIODevice::WriteOnly );
  fd.write( fs.readAll() );
  // dot -Tpng x.dot -O
  QStringList args;
  args.append( "-Tpng" );
  args.append( "/tmp/x.dot" );
  args.append( "-O" );
  if ( pp )
    pp->deleteLater();
  pp = new QProcess(this);
  connect( pp, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(graphvizDone()) );
  pp->start( "dot", args );
}

void  BlockTool::graphvizDone()
{ QPixmap p( "/tmp/x.dot.png" );
  ui->graphic->setPixmap( p );
  if ( pp )
    pp->deleteLater();
}
