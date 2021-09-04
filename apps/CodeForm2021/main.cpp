#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

/**
 * @brief setAppNames - set names for QSettings use
 */
void setAppNames()
{ QCoreApplication::setOrganizationName( "AssignOnward" );
  QCoreApplication::setOrganizationDomain( "assignonward.com" );
  QCoreApplication::setApplicationName( "CodeForm2021" );
  QSettings::setDefaultFormat( QSettings::IniFormat );
}

int main(int argc, char *argv[])
{   setAppNames();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
