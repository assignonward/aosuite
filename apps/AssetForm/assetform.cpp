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
#include "assetform.h"
#include <QSettings>

AssetForm::AssetForm( QWidget *cw, MainWinCommon *mw ) :
    QScrollArea(cw),
    ui(new Ui::AssetForm)
{
    ui->setupUi(this);
    new QVBoxLayout( cw );
    cw->layout()->addWidget( this );

    if ( mw )
      { connect( mw, SIGNAL(restoringConfig()), SLOT(restoreConfig()));
        connect( mw, SIGNAL(   savingConfig()), SLOT(   saveConfig()));
      }

}

AssetForm::~AssetForm()
{
    delete ui;
}

void  AssetForm::restoreConfig()
{ QSettings s;
  if ( s.contains( "assets" ) )
    assets = s.value( "assets" ).toByteArray();
  updateLabels();
}

void  AssetForm::saveConfig()
{ QSettings s;
  s.setValue( "assets", assets.toDataItem() );
}

void  AssetForm::updateLabels()
{ int unused            = 0;
  int controlShares     = 0;
  int sharesAssigned    = 0;
  int assignmentPending = 0;
  int sharesEscrowed    = 0;
  foreach ( GenericCollection gc, assets.keyAssets )
    { if ( gc.getTypeCode() == AO_KEY_ASSET )
        { if ( gc.properties.contains( AO_KEYPAIR ) )
            { if ( gc.properties.contains( AO_SHARES_REF ) )
                { SharesRef *sr = qobject_cast<SharesRef *>(DataItem::fromDataItem( gc.properties[AO_SHARES_REF] ));
                  switch ( sr->shareState.get() )
                    { case KEYS_UNUSED:             unused++;            break;
                      case KEYS_CONTROL_SHARES:     controlShares++;     break;
                      case KEYS_SHARES_ASSIGNED:    sharesAssigned++;    break;
                      case KEYS_ASSIGNMENT_PENDING: assignmentPending++; break;
                      case KEYS_SHARES_ESCROWED:    sharesEscrowed++;    break;
                      default: qDebug( "unexpected shareState value: %d", sr->shareState.get() );
                    }
                }
               else
                unused++;  // No AO_SHARES_REF implies that the keypair is unused
            }
           else
            { qDebug( "unexpected: AO_KEY_ASSET with no AO_KEYPAIR" ); }
        }
       else
        { qDebug( "unexpected: keyAssets with typecode %d", gc.getTypeCode() ); }
    }
  ui->keysUnused           ->setText( QString::number( unused            ) );
  ui->keysControlShares    ->setText( QString::number( controlShares     ) );
  ui->keysSharesAssigned   ->setText( QString::number( sharesAssigned    ) );
  ui->keysAssignmentPending->setText( QString::number( assignmentPending ) );
  ui->keysSharesEscrowed   ->setText( QString::number( sharesEscrowed    ) );
}
