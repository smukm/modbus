#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "abstractmodbusmanager.h"
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
    void setupModbusConnections();
    void ensureCorrectManager(ModbusConnectionSettings::ConnectionType type);
    ModbusConnectionSettings::ConnectionType getCurrentConnectionType() const;

    Ui::MainWindow *ui;
    AbstractModbusManager* m_modbusManager; // Базовый указатель на активный менеджер Modbus (RTU или TCP).
    LogManager* m_logManager; // Менеджер для ведения и отображения журнала событий.
    RegisterDataModel* m_registerModel; // Модель данных для отображения полученных регистров в TableView.
    UiController* m_uiController; // Вспомогательный контроллер для управления состоянием элементов UI.
    QTimer *m_pollingTimer; // Таймер для автоматического периодического опроса устройств.

    void setPorts();
    void fillSettings();
    void sendReadData();
    void sendWriteData();
    void stopPolling();
    void createMenu();
    void loadPortSettings();
    void savePortSettings(const ModbusConnectionSettings& settings);
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
