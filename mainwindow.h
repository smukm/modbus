#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QModbusClient* modbusDevice = nullptr;
    QModbusReply *currentReply = nullptr;  // <-- Для отслеживания текущего запроса

    void setPorts();
    void fillSettings();
    void setControlsForOpenPort();
    void setControlsForClosedPort();
    void setControlsForSendData();
    void connectToDevice();
    void disconnectFromDevice();

private slots:
    void slotApplySettings();
    //void slotPortOpened();
    //void slotPortClosed();
    void slotSendData();

    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    void onReplyFinished();
};
#endif // MAINWINDOW_H
