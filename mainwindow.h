#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusmanager.h"
#include <QMainWindow>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>


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

    void setPorts();
    void fillSettings();
    void setControlsForOpenPort();
    void setControlsForClosedPort();
    void setControlsForSendData();
    void connectToDevice();
    void disconnectFromDevice();
    void toLog(const QString& msg);
    void createMenu();

private slots:
    void onApplySettings();
    void onSendData();
    void onModbusDataReceived(const QModbusDataUnit &unit);
};
#endif // MAINWINDOW_H
