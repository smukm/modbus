#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusmanager.h"
#include <QMainWindow>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    ModbusManager* m_modbusManager;
    QStandardItemModel* m_receivedDataModel;
    QStandardItemModel *m_logModel;
    QTimer *m_pollingTimer;

    void setPorts();
    void fillSettings();
    void setControlsForOpenPort();
    void setControlsForClosedPort();
    void setControlsForSendData();
    void setCommandControlsStatus(bool status);
    void connectToDevice();
    void sendData();
    void stopPolling();
    void disconnectFromDevice();
    void toLog(const QString& msg, bool isError = false);
    void createMenu();
    int findRowByAddressAndType(int address, const QString &regType);

private slots:
    void onApplySettings();
    void onExecuteCommand();
    void onModbusDataReceived(const QModbusDataUnit &unit);
    void onPollingTimeout();
};
#endif // MAINWINDOW_H
