#include "assetsengine.h"
#include <QSettings>

AssetsEngine::AssetsEngine(QObject *parent) : QObject(parent)
{ assets.setTypeCode( AO_ASSETS );

}

void  AssetsEngine::restoreConfig()
{ QSettings s;
  if ( s.contains( "assets" ) )
    assets = s.value( "assets" ).toByteArray();
  // assets.debugShow();
}

void  AssetsEngine::saveConfig()
{ // qDebug( "AssetForm::saveConfig()" );
  QSettings s;
  s.setValue( "assets", assets.toDataItem() );
  // assets.debugShow();
}


