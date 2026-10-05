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

    // Разблокируем параметры запроса
    m_ui->btnSendData->setEnabled(true);
    m_ui->leDeviceAddress->setEnabled(true);
    m_ui->cbCode->setEnabled(true);
    m_ui->leRegisterAddress->setEnabled(true);
    m_ui->leRegistersQty->setEnabled(true);
    m_ui->cbPolling->setEnabled(true);
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

    // Блокируем параметры запроса
    m_ui->btnSendData->setEnabled(false);
    m_ui->leDeviceAddress->setEnabled(false);
    m_ui->cbCode->setEnabled(false);
    m_ui->leRegisterAddress->setEnabled(false);
    m_ui->leRegistersQty->setEnabled(false);
    m_ui->cbPolling->setEnabled(false);
    m_ui->cbPolling->setChecked(false);
}

void UiController::setPollingActiveState(bool isActive) {
    // Инвертируем статус: если опрос активен, поля ввода блокируются
    m_ui->leDeviceAddress->setEnabled(!isActive);
    m_ui->cbCode->setEnabled(!isActive);
    m_ui->leRegisterAddress->setEnabled(!isActive);
    m_ui->leData->setEnabled(!isActive);
    m_ui->leRegistersQty->setEnabled(!isActive);
    m_ui->cbPolling->setEnabled(!isActive);

    if (isActive) {
        m_ui->btnSendData->setText("Стоп");
    } else {
        m_ui->btnSendData->setText("Выполнить");
    }
}

void UiController::initializeCommandWidgets() {
    m_ui->leDeviceAddress->setText("1");
    m_ui->cbCode->clear();

    m_ui->cbCode->addItem("01 (0x01) - Чтение дискретных выходов (Read Coils)", 0x01);
    m_ui->cbCode->addItem("02 (0x02) - Чтение дискретных входов (Read Discrete Inputs)", 0x02);
    m_ui->cbCode->addItem("03 (0x03) - Чтение регистров хранения (Read Holding Registers)", 0x03);
    m_ui->cbCode->addItem("04 (0x04) - Чтение входных регистров (Read Input Registers)", 0x04);
    m_ui->cbCode->addItem("05 (0x05) - Запись одного дискретного выхода (Write Single Coil)", 0x05);
    m_ui->cbCode->addItem("06 (0x06) - Запись одного регистра (Write Single Register)", 0x06);
    m_ui->cbCode->addItem("15 (0x0F) - Запись нескольких дискретных выходов (Write Multiple Coils)", 0x0F);
    m_ui->cbCode->addItem("16 (0x10) - Запись нескольких регистров (Write Multiple Registers)", 0x10);

    // Выбираем 0x03 по умолчанию
    m_ui->cbCode->setCurrentIndex(2);
    m_ui->leRegisterAddress->setText("0");
    m_ui->leRegistersQty->setText("1");

    // Лямбда для управления видимостью поля данных
    auto updateDataFieldVisibility = [this]() {
        int funcCode = m_ui->cbCode->currentData().toInt();
        bool isWriteCommand = (funcCode == 0x05 || funcCode == 0x06 || funcCode == 0x0F || funcCode == 0x10);
        m_ui->leData->setVisible(isWriteCommand);
    };

    QObject::connect(m_ui->cbCode, &QComboBox::currentIndexChanged, [updateDataFieldVisibility]() {
        updateDataFieldVisibility();
    });

    // Первоначальный вызов
    updateDataFieldVisibility();
}