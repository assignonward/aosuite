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
#include <unistd.h>

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

void BlockTool::liveDelay( int t )
{ QTimer timer;
  timer.setSingleShot( true );
  timer.start( t );
  QCoreApplication::processEvents( QEventLoop::AllEvents, t );
  while ( timer.isActive() )
    { usleep( 5000 ); // Get 5ms of "real sleep"
      QCoreApplication::processEvents( QEventLoop::AllEvents, 50 );
    }
}

void  BlockTool::on_start_clicked()
{ ui->report->clear();
  ui->report->append( "Static test block" );
  BlockValueObject *cbo = new BlockValueObject( this );
  KeyValuePair     *kvp = new KeyValuePair(RCD_chainBlock_o,cbo,this);
  BlockValueObject *hdo = new BlockValueObject( this );
  cbo->insert( RCD_hashedOb_o, hdo );
  BlockValueObject *hso = new BlockValueObject( this );
  cbo->insert( RCD_hash_o, hso );
  BlockValueInt64 *hti = new BlockValueInt64( 1234, this );
  hso->insert( RCD_time_i, hti );

  writeWrappedDot( kvp->dot() );
  updateGraph();
}

void  BlockTool::writeWrappedDot( QByteArray d )
{ QFile fd("/tmp/x.dot");
  fd.open( QIODevice::WriteOnly );
  fd.write( "digraph AO {\n" );
  fd.write( "rankdir=LR;\n\n" );
  fd.write( d );
  fd.write( "}\n" );
  // qWarning( "%s", d.data() );
  updateGraph();
}

void  BlockTool::updateGraph()
{ // dot -Tpng /tmp/x.dot -O
  QStringList args;
  args.append( "-Tpng" );
  args.append( "/tmp/x.dot" );
  args.append( "-O" );
  if ( pp )
    pp->deleteLater();
  pp = new QProcess(this);
  connect( pp, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(graphvizDone(int,QProcess::ExitStatus)) );
  pp->start( "dot", args );
}

void  BlockTool::graphvizDone(int code,QProcess::ExitStatus status)
{ qWarning( "finished %d %d", code, status );
  liveDelay( 50 );
  QPixmap p( "/tmp/x.dot.png" );
  ui->graphic->setPixmap( p );
  if ( pp )
    pp->deleteLater();
}
