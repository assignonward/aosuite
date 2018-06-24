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
#ifndef GENESISFORM_H
#define GENESISFORM_H

#include <QtGui>
#include "assetsengine.h"
#include "ui_genesisForm.h"
#include "MainWinCommon.h"

namespace Ui {
class GenesisForm;
}

class GenesisForm : public QScrollArea
{
    Q_OBJECT

public:
         explicit  GenesisForm( QWidget *cw, MainWinCommon *mw = NULL, AssetsEngine *iae = NULL );
                  ~GenesisForm();
GenericCollection  calculateGenesisBlock();

public slots:
             void  restoreConfig();
             void  saveConfig();
             void  updateHash();
             void  updateKeyList();
             void  on_chooseIcon_clicked();
             void  on_chooseImage_clicked();
             void  on_importGenesisBlock_clicked();
             void  on_publishGenesisBlock_clicked();

public:
        Ui::GenesisForm *ui;

private:
  QPointer<AssetsEngine> ae;
};

#endif // RABBITFORM_H
