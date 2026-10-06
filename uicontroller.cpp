#include "uicontroller.h"
#include "./ui_mainwindow.h" // Реальное определение UI
#include <QSerialPort>
#include <QComboBox>

UiController::UiController(Ui::MainWindow *ui) : m_ui(ui) {}

void UiController::setConnectedState() {
    m_ui->btnApply->setText("Закрыть");

    // Блокируем настройки порта
    m_ui->cbPorts->setEnabled(false);
    m_ui->cbBaudRate->setEnabled(false);
    m_ui->cbDataBits->setEnabled(false);
    m_ui->cbParity->setEnabled(false);
    m_ui->cbStopBits->setEnabled(false);

    // Разблокируем параметры запроса на чтение регистров
    m_ui->btnSendData->setEnabled(true);
    m_ui->leReadDeviceAddress->setEnabled(true);
    m_ui->cbReadCode->setEnabled(true);
    m_ui->leReadRegisterAddress->setEnabled(true);
    m_ui->leReadRegistersQty->setEnabled(true);

    // Разблокируем параметры запроса на запись регистров
    m_ui->btnExecuteOnce->setEnabled(true);
    m_ui->leWriteDeviceAddress->setEnabled(true);
    m_ui->cbWriteCode->setEnabled(true);
    m_ui->leWriteRegisterAddress->setEnabled(true);
    m_ui->leWriteRegistersQty->setEnabled(true);
}

void UiController::setDisconnectedState() {
    m_ui->btnApply->setEnabled(true);
    m_ui->btnApply->setText("Открыть");

    // Разблокируем настройки порта
    m_ui->cbPorts->setEnabled(true);
    m_ui->cbBaudRate->setEnabled(true);
    m_ui->cbDataBits->setEnabled(true);
    m_ui->cbParity->setEnabled(true);
    m_ui->cbStopBits->setEnabled(true);

    // Блокируем параметры запроса на чтение регистров
    m_ui->btnSendData->setEnabled(false);
    m_ui->leReadDeviceAddress->setEnabled(false);
    m_ui->cbReadCode->setEnabled(false);
    m_ui->leReadRegisterAddress->setEnabled(false);
    m_ui->leReadRegistersQty->setEnabled(false);

    // Блокируем параметры запроса на запись регистров
    m_ui->btnExecuteOnce->setEnabled(false);
    m_ui->leWriteDeviceAddress->setEnabled(false);
    m_ui->cbWriteCode->setEnabled(false);
    m_ui->leWriteRegisterAddress->setEnabled(false);
    m_ui->leWriteRegistersQty->setEnabled(false);
}

void UiController::setPollingActiveState(bool isActive) {
    // Инвертируем статус: если опрос активен, поля ввода блокируются
    m_ui->leReadDeviceAddress->setEnabled(!isActive);
    m_ui->cbReadCode->setEnabled(!isActive);
    m_ui->leReadRegisterAddress->setEnabled(!isActive);
    //m_ui->leData->setEnabled(!isActive);
    m_ui->leReadRegistersQty->setEnabled(!isActive);
    //m_ui->cbPolling->setEnabled(!isActive);

    if (isActive) {
        m_ui->btnSendData->setText("Стоп");
    } else {
        m_ui->btnSendData->setText("Выполнить");
    }
}

void UiController::initializeCommandWidgets() {
    m_ui->leReadDeviceAddress->setText("1");
    m_ui->cbReadCode->clear();
    m_ui->cbReadCode->addItem("01 (0x01) - Чтение дискретных выходов (Read Coils)", 0x01);
    m_ui->cbReadCode->addItem("02 (0x02) - Чтение дискретных входов (Read Discrete Inputs)", 0x02);
    m_ui->cbReadCode->addItem("03 (0x03) - Чтение регистров хранения (Read Holding Registers)", 0x03);
    m_ui->cbReadCode->addItem("04 (0x04) - Чтение входных регистров (Read Input Registers)", 0x04);
    // m_ui->cbReadCode->addItem("05 (0x05) - Запись одного дискретного выхода (Write Single Coil)", 0x05);
    // m_ui->cbReadCode->addItem("06 (0x06) - Запись одного регистра (Write Single Register)", 0x06);
    // m_ui->cbReadCode->addItem("15 (0x0F) - Запись нескольких дискретных выходов (Write Multiple Coils)", 0x0F);
    // m_ui->cbReadCode->addItem("16 (0x10) - Запись нескольких регистров (Write Multiple Registers)", 0x10);

    // Выбираем 0x03 по умолчанию
    m_ui->cbReadCode->setCurrentIndex(2);
    m_ui->leReadRegisterAddress->setText("0");
    m_ui->leReadRegistersQty->setText("1");

    // Лямбда для управления видимостью поля данных
    // auto updateDataFieldVisibility = [this]() {
    //     int funcCode = m_ui->cbReadCode->currentData().toInt();
    //     bool isWriteCommand = (funcCode == 0x05 || funcCode == 0x06 || funcCode == 0x0F || funcCode == 0x10);
    //     m_ui->leData->setVisible(isWriteCommand);
    // };

    // QObject::connect(m_ui->cbReadCode, &QComboBox::currentIndexChanged, [updateDataFieldVisibility]() {
    //     updateDataFieldVisibility();
    // });

    // Первоначальный вызов
    //updateDataFieldVisibility();


    m_ui->leWriteDeviceAddress->setText("1");
    m_ui->cbWriteCode->clear();
    // m_ui->cbWriteCode->addItem("01 (0x01) - Чтение дискретных выходов (Read Coils)", 0x01);
    // m_ui->cbWriteCode->addItem("02 (0x02) - Чтение дискретных входов (Read Discrete Inputs)", 0x02);
    // m_ui->cbWriteCode->addItem("03 (0x03) - Чтение регистров хранения (Read Holding Registers)", 0x03);
    // m_ui->cbWriteCode->addItem("04 (0x04) - Чтение входных регистров (Read Input Registers)", 0x04);
    m_ui->cbWriteCode->addItem("05 (0x05) - Запись одного дискретного выхода (Write Single Coil)", 0x05);
    m_ui->cbWriteCode->addItem("06 (0x06) - Запись одного регистра (Write Single Register)", 0x06);
    m_ui->cbWriteCode->addItem("15 (0x0F) - Запись нескольких дискретных выходов (Write Multiple Coils)", 0x0F);
    m_ui->cbWriteCode->addItem("16 (0x10) - Запись нескольких регистров (Write Multiple Registers)", 0x10);

    // Выбираем 0x06 по умолчанию
    m_ui->cbWriteCode->setCurrentIndex(1);
    m_ui->leWriteRegisterAddress->setText("0");
    m_ui->leWriteRegistersQty->setText("1");
}