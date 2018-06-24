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
#ifndef CRYPTOFORM_H
#define CRYPTOFORM_H

#include <QtGui>
#include "ui_cryptoForm.h"
#include "MainWinCommon.h"
#include "cryptoEngine.h"
#include "dataitem.h"
#include <QPointer>

namespace Ui {
class CryptoForm;
}

class CryptoForm : public QScrollArea
{
    Q_OBJECT

public:
         explicit  CryptoForm( QWidget *cw, MainWinCommon *mw = NULL, CryptoEngine *ice = NULL );
                  ~CryptoForm();

             bool  getGpgKeys();
             bool  getGpgInfo();
             bool  getKeyInfo();

public slots:
             void  on_selectedKeyNumber_valueChanged( int v );
             void  on_selectedSubkeyNumber_valueChanged( int v );
             void  on_homeFolder_textEdited( QString );
             void  on_passphrase_textEdited( QString );
             void  on_reread_clicked();
             void  restoreConfig();
             void  saveConfig();

public:
QPointer<CryptoEngine> ce;
       Ui::CryptoForm *ui;

};

#endif // CRYPTOFORM_H
