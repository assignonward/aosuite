#ifndef ASSETSENGINE_H
#define ASSETSENGINE_H

#include <QObject>
#include "genericcollection.h"

/**
 * @brief The AssetsEngine class - wraps an AO_ASSETS collection and provides
 *   subset retrieval functions, change notifier signals, etc.
 */
class AssetsEngine : public QObject
{
    Q_OBJECT
public:
    explicit  AssetsEngine(QObject *parent = NULL);
        void  restoreConfig();
        void  saveConfig();
        void  insert( DataItem *it ) { assets.insert( it ); }
 DataItemMap  itemMM() { return assets.itemMM; }

signals:

public slots:

private:
  GenericCollection  assets;
};

#endif // ASSETSENGINE_H
