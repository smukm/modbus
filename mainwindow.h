#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "comport.h"

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
    Comport *m_comPort;

    void setPorts();
    void fillSettings();
    void openPortWithSettings();
    void setControlsForOpenPort();
    void setControlsForClosedPort();
    void setControlsForSendData();

private slots:
    void slotApplySettings();
    void slotPortOpened();
    void slotPortClosed();
    void slotSendData();
    void slotModbusResponse(const QByteArray &frame);
};
#endif // MAINWINDOW_H
