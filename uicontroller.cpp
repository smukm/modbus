#include "uicontroller.h"
#include "./ui_mainwindow.h" // Реальное определение UI
#include <QSerialPort>
#include <QComboBox>
#include <QObject>

UiController::UiController(Ui::MainWindow *ui) : m_ui(ui) {}

void UiController::setConnectedState() {
    m_ui->btnApply->setText("Закрыть");

    m_ui->cbConnectionType->setEnabled(false);
    m_ui->leIpAddress->setEnabled(false);
    m_ui->sbPort->setEnabled(false);

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
    m_ui->btnApply->setText(QObject::tr("Connect"));

    m_ui->cbConnectionType->setEnabled(true);
    m_ui->leIpAddress->setEnabled(true);
    m_ui->sbPort->setEnabled(true);

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
        m_ui->btnSendData->setText(QObject::tr("Stop"));
    } else {
        m_ui->btnSendData->setText(QObject::tr("Execute"));
    }
}

void UiController::initializeCommandWidgets() {
    m_ui->leReadDeviceAddress->setText("1");
    m_ui->cbReadCode->clear();
    m_ui->cbReadCode->addItem(QObject::tr("01 (0x01) - Read Coils"), 0x01);
    m_ui->cbReadCode->addItem(QObject::tr("02 (0x02) - Read Discrete Inputs"), 0x02);
    m_ui->cbReadCode->addItem(QObject::tr("03 (0x03) - Read Holding Registers"), 0x03);
    m_ui->cbReadCode->addItem(QObject::tr("04 (0x04) - Read Input Registers"), 0x04);
    // Выбираем 0x03 по умолчанию
    m_ui->cbReadCode->setCurrentIndex(2);
    m_ui->leReadRegisterAddress->setText("0");
    m_ui->leReadRegistersQty->setText("1");

    m_ui->leWriteDeviceAddress->setText("1");
    m_ui->cbWriteCode->clear();
    m_ui->cbWriteCode->addItem(QObject::tr("05 (0x05) - Write Single Coil"), 0x05);
    m_ui->cbWriteCode->addItem(QObject::tr("06 (0x06) - Write Single Register"), 0x06);
    m_ui->cbWriteCode->addItem(QObject::tr("15 (0x0F) - Write Multiple Coils"), 0x0F);
    m_ui->cbWriteCode->addItem(QObject::tr("16 (0x10) - Write Multiple Registers"), 0x10);
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
    m_ui->cbDataBits->addItem(QObject::tr("5 bits"), QSerialPort::Data5);
    m_ui->cbDataBits->addItem(QObject::tr("6 bits"), QSerialPort::Data6);
    m_ui->cbDataBits->addItem(QObject::tr("7 bits"), QSerialPort::Data7);
    m_ui->cbDataBits->addItem(QObject::tr("8 bits"), QSerialPort::Data8);
    m_ui->cbDataBits->setCurrentText(QObject::tr("8 bits"));

    // Четность
    m_ui->cbParity->clear();
     m_ui->cbParity->addItem(QObject::tr("None"), QSerialPort::NoParity);
     m_ui->cbParity->addItem(QObject::tr("Even"), QSerialPort::EvenParity);
     m_ui->cbParity->addItem(QObject::tr("Odd"), QSerialPort::OddParity);
     m_ui->cbParity->addItem(QObject::tr("Space"), QSerialPort::SpaceParity);
     m_ui->cbParity->addItem(QObject::tr("Mark"), QSerialPort::MarkParity);
     m_ui->cbParity->setCurrentText(QObject::tr("None"));

    // Стоп-биты
    m_ui->cbStopBits->clear();
     m_ui->cbStopBits->addItem(QObject::tr("1 stop bit"), QSerialPort::OneStop);
     m_ui->cbStopBits->addItem(QObject::tr("1.5 stop bits"), QSerialPort::OneAndHalfStop);
     m_ui->cbStopBits->addItem(QObject::tr("2 stop bits"), QSerialPort::TwoStop);
     m_ui->cbStopBits->setCurrentText(QObject::tr("1 stop bit"));
}

void UiController::initializeConnectionsCombo() {
    m_ui->cbConnectionType->clear();
    m_ui->cbConnectionType->addItem("Serial (RTU)", static_cast<int>(ModbusConnectionSettings::Serial));
    m_ui->cbConnectionType->addItem("TCP",         static_cast<int>(ModbusConnectionSettings::Tcp));
}

void UiController::updateConnectionUiVisibility(ModbusConnectionSettings::ConnectionType type) {
    // Показываем/скрываем элементы UI в зависимости от типа подключения
    bool isSerial = (type == ModbusConnectionSettings::Serial);

    if (m_ui->cbPorts) m_ui->cbPorts->setVisible(isSerial);
    if (m_ui->cbBaudRate) m_ui->cbBaudRate->setVisible(isSerial);
    if (m_ui->lblBaudRate) m_ui->lblBaudRate->setVisible(isSerial);
    if (m_ui->cbParity) m_ui->cbParity->setVisible(isSerial);
    if (m_ui->lblParity) m_ui->lblParity->setVisible(isSerial);
    if (m_ui->cbDataBits) m_ui->cbDataBits->setVisible(isSerial);
    if (m_ui->lblDataBits) m_ui->lblDataBits->setVisible(isSerial);
    if (m_ui->cbStopBits) m_ui->cbStopBits->setVisible(isSerial);
    if (m_ui->lblStopBits) m_ui->lblStopBits->setVisible(isSerial);

    if (m_ui->leIpAddress) m_ui->leIpAddress->setVisible(!isSerial);
    if (m_ui->lblIpAddress) m_ui->lblIpAddress->setVisible(!isSerial);
    if (m_ui->sbPort) m_ui->sbPort->setVisible(!isSerial);
    if (m_ui->lblPort) m_ui->lblPort->setVisible(!isSerial);
    if (isSerial) {
        m_ui->gbPortSettings->setTitle(QObject::tr("Port settings"));
    } else {
        m_ui->gbPortSettings->setTitle(QObject::tr("Connection settings"));
    }
}
