#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <QStandardItemModel>
#include <QDateTime>

class LogManager : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit LogManager(QObject *parent = nullptr);
    void addLog(const QString &message, bool isError = false);
    void clear();

private:
    const int MAX_LOG_ENTRIES = 1000;

signals:
    void logAdded();
};

#endif // LOGMANAGER_H
