#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modbusmanager.h"
#include "logmanager.h"
#include "registerdatamodel.h"
#include "uicontroller.h"
#include <QMainWindow>
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
    LogManager* m_logManager;
    RegisterDataModel* m_registerModel;
    UiController* m_uiController; // Helper
    QTimer *m_pollingTimer;

    void setPorts();
    void fillSettings();
    void sendReadData();
    void sendWriteData();
    void stopPolling();
    void createMenu();
    void loadSettings();
    void saveSettings(const ModbusConnectionSettings& settings);
    void loadLastCommandParams();
    void saveLastCommandParams();

private slots:
    void onOpenPort();
    void onStartReading();
    void onStartWriting();
    void onPollingTimeout();
    void onLogAdded();
};
#endif // MAINWINDOW_H
