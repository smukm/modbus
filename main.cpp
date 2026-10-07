#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    const QString ORGANIZATION_NAME = "Smukm";
    const QString APP_NAME = "ModbusClient";
    QCoreApplication::setOrganizationDomain(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APP_NAME);

    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowTitle(APP_NAME);
    w.setWindowIcon(QIcon(":/icons/com.ico"));
    w.show();
    return QApplication::exec();
}
