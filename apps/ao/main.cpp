#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set names for QSettings use
    QCoreApplication::setOrganizationName("AssignOnward");
    QCoreApplication::setOrganizationDomain("assignonward.com");
    QCoreApplication::setApplicationName("AOAO");  // Assign Onward Asset Organizer

    MainWindow w;
    w.show();

    return a.exec();
}
