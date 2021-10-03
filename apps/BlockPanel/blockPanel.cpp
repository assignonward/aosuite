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

BlockPanel::BlockPanel( QString l, QWidget *cw ) :
    QScrollArea(cw),
    ui(new Ui::BlockPanel)
{ ui->setupUi(this);
  if ( cw )
    { QVBoxLayout *vb = new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      vb->setContentsMargins( 0,0,0,0 );
    }
  setLabel(l);
}

BlockPanel::~BlockPanel()
{ delete ui;
  if ( kvp )
    kvp->deleteLater();
}

void  BlockPanel::setBlock( KeyValuePair *p )
{ if ( kvp )
    { kvp->clear();
      kvp->setBsonish( p->bsonish() );
    }
   else
    kvp = new KeyValuePair( p->bsonish() );
  ui->view->setText( kvp->json() );
  this->setMinimumWidth( 300 );
}

/**
 * @brief BlockPanel::setLabel - label is a unique identifier for this panel, and is shown in the ui at creation
 * @param t - text to serve in label
 */
void  BlockPanel::setLabel( QString t )
{ label = t;
  ui->view->setText( t );
}
