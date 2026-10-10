#ifndef UICONTROLLER_H
#define UICONTROLLER_H
#include "./ui_mainwindow.h"
#include "abstractmodbusmanager.h"

class UiController
{
public:
    explicit UiController(Ui::MainWindow *ui);

    // Управление состоянием подключения
    void setConnectedState();
    void setDisconnectedState();

    // Управление состоянием периодического опроса
    void setPollingActiveState(bool isActive);

    // Первичная инициализация виджетов отправки команд
    void initializeCommandWidgets();

    void initializePortSettingsCombo();
    void initializeConnectionsCombo();

    void updateConnectionUiVisibility(ModbusConnectionSettings::ConnectionType type);

private:
    Ui::MainWindow *m_ui;
};

#endif // UICONTROLLER_H
