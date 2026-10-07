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
    m_ui->leReadRegistersQty->setEnabled(!isActive);

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
    // Выбираем 0x03 по умолчанию
    m_ui->cbReadCode->setCurrentIndex(2);
    m_ui->leReadRegisterAddress->setText("0");
    m_ui->leReadRegistersQty->setText("1");

    m_ui->leWriteDeviceAddress->setText("1");
    m_ui->cbWriteCode->clear();
    m_ui->cbWriteCode->addItem("05 (0x05) - Запись одного дискретного выхода (Write Single Coil)", 0x05);
    m_ui->cbWriteCode->addItem("06 (0x06) - Запись одного регистра (Write Single Register)", 0x06);
    m_ui->cbWriteCode->addItem("15 (0x0F) - Запись нескольких дискретных выходов (Write Multiple Coils)", 0x0F);
    m_ui->cbWriteCode->addItem("16 (0x10) - Запись нескольких регистров (Write Multiple Registers)", 0x10);
    // Выбираем 0x06 по умолчанию
    m_ui->cbWriteCode->setCurrentIndex(1);
    m_ui->leWriteRegisterAddress->setText("0");
    m_ui->leWriteRegistersQty->setText("1");
}

/**
 * @brief Заполняет ComboBox-ы настройками порта.
 */
void UiController::initializePortSettingsCombo() {
    // Скорость порта
    m_ui->cbBaudRate->clear();
    m_ui->cbBaudRate->addItem("1200", QSerialPort::Baud1200);
    m_ui->cbBaudRate->addItem("2400", QSerialPort::Baud2400);
    m_ui->cbBaudRate->addItem("4800", QSerialPort::Baud4800);
    m_ui->cbBaudRate->addItem("9600", QSerialPort::Baud9600);
    m_ui->cbBaudRate->addItem("19200", QSerialPort::Baud19200);
    m_ui->cbBaudRate->addItem("38400", QSerialPort::Baud38400);
    m_ui->cbBaudRate->addItem("57600", QSerialPort::Baud57600);
    m_ui->cbBaudRate->addItem("115200", QSerialPort::Baud115200);
    m_ui->cbBaudRate->setCurrentText("9600"); // Выбираем по тексту

    // Биты данных
    m_ui->cbDataBits->clear();
    m_ui->cbDataBits->addItem("5 бит", QSerialPort::Data5);
    m_ui->cbDataBits->addItem("6 бит", QSerialPort::Data6);
    m_ui->cbDataBits->addItem("7 бит", QSerialPort::Data7);
    m_ui->cbDataBits->addItem("8 бит", QSerialPort::Data8);
    m_ui->cbDataBits->setCurrentText("8 бит");

    // Четность
    m_ui->cbParity->clear();
    m_ui->cbParity->addItem("Без контроля четности (None)", QSerialPort::NoParity);
    m_ui->cbParity->addItem("Четный (Even)", QSerialPort::EvenParity);
    m_ui->cbParity->addItem("Нечетный (Odd)", QSerialPort::OddParity);
    m_ui->cbParity->addItem("Space", QSerialPort::SpaceParity);
    m_ui->cbParity->addItem("Mark", QSerialPort::MarkParity);
    m_ui->cbParity->setCurrentText("Без контроля четности (None)");

    // Стоп-биты
    m_ui->cbStopBits->clear();
    m_ui->cbStopBits->addItem("1 стоп-бит", QSerialPort::OneStop);
    m_ui->cbStopBits->addItem("1.5 стоп-бита", QSerialPort::OneAndHalfStop);
    m_ui->cbStopBits->addItem("2 стоп-бита", QSerialPort::TwoStop);
    m_ui->cbStopBits->setCurrentText("1 стоп-бит");
}
