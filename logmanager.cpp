#include "logmanager.h"

LogManager::LogManager(QObject* parent) : QStandardItemModel(parent) {}

void LogManager::addLog(const QString &message, bool isError) {
    QString t = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss:zz");
    QString logEntry = QString("%1 %2").arg(t, message);

    QStandardItem* item = new QStandardItem(logEntry);
    if (isError) {
         item->setForeground(Qt::darkRed);
    }
    appendRow(item);

    int rowCnt = rowCount();
    if (rowCnt > MAX_LOG_ENTRIES) {
        int rowsToRemove = rowCnt - MAX_LOG_ENTRIES / 2;
         removeRows(0, rowsToRemove);
    }

    emit logAdded();
}

void LogManager::clear() {
    QStandardItemModel::clear();
}