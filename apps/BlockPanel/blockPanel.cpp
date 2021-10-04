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
#include "blockPanel.h"
#include "ui_blockPanel.h"
#include "blockOb.h"
#include <unistd.h>

// TODO: test intentional failures, error checking, length reporting, trailing input data, etc.

BlockPanel::BlockPanel( QString l, Mode m, QWidget *cw ) :
    QScrollArea(cw),
    ui(new Ui::BlockPanel)
{ drawingInProgress = false;
  m_mode = m;
  ui->setupUi(this);
  if ( cw )
    { QVBoxLayout *vb = new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      vb->setContentsMargins( 0,0,0,0 );
    }
  setLabel(l);
}

BlockPanel::~BlockPanel()
{ delete ui;
  if ( m_kvp )
    m_kvp->deleteLater();
}

void  BlockPanel::setBlock( KeyValuePair *p )
{ if ( m_kvp )
    { m_kvp->clear();
      m_kvp->setBsonish( p->bsonish() );
    }
   else
    m_kvp = new KeyValuePair( p->bsonish() );
  update();
}

void  BlockPanel::update()
{ if ( m_kvp )
    writeWrappedDot( m_kvp->dot() );
   else
    { ui->view->clear();
      ui->view->setText( m_label );
    }
}

/**
 * @brief BlockPanel::setLabel - label is a unique identifier for this panel, and is shown in the ui at creation
 * @param t - text to serve in label
 */
void  BlockPanel::setLabel( QString t )
{ m_label = t;
  ui->view->setText( t );
}

void BlockPanel::liveDelay( int t )
{ QTimer timer;
  timer.setSingleShot( true );
  timer.start( t );
  QCoreApplication::processEvents( QEventLoop::AllEvents, t );
  while ( timer.isActive() )
    { usleep( 5000 ); // Get 5ms of "real sleep"
      QCoreApplication::processEvents( QEventLoop::AllEvents, 50 );
    }
}

void  BlockPanel::writeWrappedDot( QByteArray d )
{ if ( drawingInProgress )
    { qint32 loops = 0;
      while ( drawingInProgress )
        { liveDelay(30);
          loops++;
        }
      if ( loops > 3 )
        qWarning( "drawingInProgress paused %d loops", loops );
    }
  drawingInProgress = true;
  QFile fd(QString("/tmp/%1.dot").arg(m_label));
  fd.open( QIODevice::WriteOnly );
  fd.write( "digraph AO {\n" );
  fd.write( "rankdir=LR;\n" );
  liveDelay( 50 );
  switch ( m_mode )
    { case make:  fd.write( "node [color=darkgreen]; graph [color=darkgreen] bgcolor=mintcream;\n"  ); break;
      case build: fd.write( "node [color=darkblue];  graph [color=darkblue]  bgcolor=ghostwhite;\n" ); break;
      case idle:  fd.write( "node [color=grey];      graph [color=grey]      bgcolor=grey90;\n" ); break;
    }
  fd.write( d );
  fd.write( "}\n" );
  fd.flush();
  fd.close();
  updateGraph();
}

void  BlockPanel::updateGraph()
{ // dot -Tpng /tmp/x.dot -O
  QVector<QString> args;
  args.append( "-Tpng" );
  args.append( QString("/tmp/%1.dot").arg(m_label) );
  args.append( "-O" );
  if ( pp )
    pp->deleteLater();
  pp = new QProcess(this);
  connect( pp, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(graphvizDone(int,QProcess::ExitStatus)) );
  pp->start( "dot", args );
}

void  BlockPanel::graphvizDone(int code,QProcess::ExitStatus status)
{ if ( code != 0 )
    qWarning( "BlockPanel::graphvizDone() %d %d", code, status );
  liveDelay( 50 );
  QPixmap p( QString("/tmp/%1.dot.png").arg(m_label) );
  ui->view->setPixmap( p );
  this->setMinimumWidth( p.width() + qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) );
  if ( pp )
    pp->deleteLater();
  pp = nullptr;
  drawingInProgress = false;
}
