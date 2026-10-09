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

    // 1. Получаем системную локаль пользователя (например, "ru_RU", "en_US", "de_DE")
    QLocale systemLocale = QLocale::system();
    QString localeName = systemLocale.name();

    QTranslator translator;
    if (translator.load(systemLocale, "", "", ":/translations")) {
        a.installTranslator(&translator);
    } else {
        qDebug() << "⚠️ Translation for" << localeName << "not found. Using default (source) language.";

    }
    MainWindow w;

    w.setWindowTitle(APP_NAME);
    w.setWindowIcon(QIcon(":/icons/com.ico"));
    w.show();
    return QApplication::exec();
}
