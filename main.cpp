#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    const QString ORGANIZATION_NAME = "Smukm";
    const QString APP_NAME = "ModbusClient";
    QCoreApplication::setOrganizationDomain(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APP_NAME);

    QApplication a(argc, argv);
    QTranslator translator;
    if (translator.load(":/translations/ru_RU.qm")) {
        a.installTranslator(&translator);
    } else {
        qWarning() << "❌ Не удалось загрузить файл перевода ru_RU.qm";
    }
    MainWindow w;

    w.setWindowTitle(APP_NAME);
    w.setWindowIcon(QIcon(":/icons/com.ico"));
    w.show();
    return QApplication::exec();
}
