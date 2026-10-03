#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusReply>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Инициализация Modbus устройства
    modbusDevice = new QModbusRtuSerialClient(this);

    // Подключаем сигналы состояния и ошибок
    connect(modbusDevice, &QModbusClient::stateChanged, this, &MainWindow::onStateChanged);
    connect(modbusDevice, &QModbusClient::errorOccurred, this, &MainWindow::onErrorOccurred);

    // Заполняем список доступных портов
    setPorts();
    fillSettings();
    setControlsForSendData();
    setControlsForClosedPort();

    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::slotApplySettings);
    connect(ui->btnSendData, &QPushButton::clicked, this, &MainWindow::slotSendData);
}

MainWindow::~MainWindow()
{
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
    }
    delete ui;
}

/**
 * @brief Инициализирует и заполняет выпадающий список доступных COM-портов.
 * Также создает экземпляр класса Comport для первого найденного порта.
 */
void MainWindow::setPorts() {

    ui->cbPorts->clear();
    const auto ports = QSerialPortInfo::availablePorts();

    if (ports.isEmpty()) {
        ui->cbPorts->addItem("Нет доступных COM-портов");
        ui->cbPorts->setEnabled(false);
        ui->btnApply->setEnabled(false);
    } else {

        for (const QSerialPortInfo &info : ports) {
            QString displayText = info.portName();
            if (!info.description().isEmpty()) {
                displayText += QString(" (%1)").arg(info.description());
            }

            // addItem(Текст для отображения, Данные для внутреннего использования)
            ui->cbPorts->addItem(displayText, info.portName());
        }
        ui->cbPorts->setEnabled(true);
        ui->btnApply->setEnabled(true);
    }
}

/**
 * @brief Заполняет ComboBox-ы настройками порта.
 */
void MainWindow::fillSettings() {
    // Скорость порта
    ui->cbBaudRate->clear();
    ui->cbBaudRate->addItem("1200", QSerialPort::Baud1200);
    ui->cbBaudRate->addItem("2400", QSerialPort::Baud2400);
    ui->cbBaudRate->addItem("4800", QSerialPort::Baud4800);
    ui->cbBaudRate->addItem("9600", QSerialPort::Baud9600);
    ui->cbBaudRate->addItem("19200", QSerialPort::Baud19200);
    ui->cbBaudRate->addItem("38400", QSerialPort::Baud38400);
    ui->cbBaudRate->addItem("57600", QSerialPort::Baud57600);
    ui->cbBaudRate->addItem("115200", QSerialPort::Baud115200);
    ui->cbBaudRate->setCurrentText("9600"); // Выбираем по тексту

    // Биты данных
    ui->cbDataBits->clear();
    ui->cbDataBits->addItem("5 бит", QSerialPort::Data5);
    ui->cbDataBits->addItem("6 бит", QSerialPort::Data6);
    ui->cbDataBits->addItem("7 бит", QSerialPort::Data7);
    ui->cbDataBits->addItem("8 бит", QSerialPort::Data8);
    ui->cbDataBits->setCurrentText("8 бит");

    // Четность
    ui->cbParity->clear();
    ui->cbParity->addItem("Без контроля четности (None)", QSerialPort::NoParity);
    ui->cbParity->addItem("Четный (Even)", QSerialPort::EvenParity);
    ui->cbParity->addItem("Нечетный (Odd)", QSerialPort::OddParity);
    ui->cbParity->addItem("Space", QSerialPort::SpaceParity);
    ui->cbParity->addItem("Mark", QSerialPort::MarkParity);
    ui->cbParity->setCurrentText("Без контроля четности (None)");

    // Стоп-биты
    ui->cbStopBits->clear();
    ui->cbStopBits->addItem("1 стоп-бит", QSerialPort::OneStop);
    ui->cbStopBits->addItem("1.5 стоп-бита", QSerialPort::OneAndHalfStop);
    ui->cbStopBits->addItem("2 стоп-бита", QSerialPort::TwoStop);
    ui->cbStopBits->setCurrentText("1 стоп-бит");
}

/**
 * @brief Слот-обработчик нажатия кнопки "Открыть/Закрыть" (btnApply).
 * Реализует логику переключения (toggle): если порт открыт — закрываем его,
 * если закрыт — считываем настройки из UI и открываем.
 */
void MainWindow::slotApplySettings() {
    if (!modbusDevice) {
        return;
    }

    if (modbusDevice->state() == QModbusDevice::ConnectedState) {
        disconnectFromDevice();
    } else {
        connectToDevice();
    }
}

void MainWindow::connectToDevice() {
    if (!modbusDevice) return;

    // Считываем параметры из UI
    QString portName = ui->cbPorts->currentData().toString();
    int baudRate = ui->cbBaudRate->currentData().toInt();
    int parity = ui->cbParity->currentData().toInt();
    int dataBits = ui->cbDataBits->currentData().toInt();
    int stopBits = ui->cbStopBits->currentData().toInt();

    // Применяем параметры к Modbus-устройству
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, portName);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudRate);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, parity);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, dataBits);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, stopBits);

    // Таймаут ответа (по умолчанию 1000 мс, можно увеличить для медленных сетей)
    modbusDevice->setTimeout(1000);
    // Количество повторных попыток при сбое
    modbusDevice->setNumberOfRetries(3);

    ui->textEditLog->append("Попытка подключения к " + portName);

    if (!modbusDevice->connectDevice()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось начать подключение: " + modbusDevice->errorString());
    }
}

void MainWindow::disconnectFromDevice() {
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
    }
}

void MainWindow::onStateChanged(QModbusDevice::State state) {
    if (state == QModbusDevice::UnconnectedState) {
        setControlsForClosedPort();
        ui->textEditLog->append("Modbus: Порт отключен.");
    } else if (state == QModbusDevice::ConnectedState) {
        setControlsForOpenPort();
        ui->textEditLog->append("Modbus: Порт успешно подключен.");
    }
}

void MainWindow::onErrorOccurred(QModbusDevice::Error error) {
    if (error == QModbusDevice::NoError) return;
    ui->textEditLog->append("Warning! Modbus ошибка:" + modbusDevice->errorString());
}

/**
 * @brief Отправка данных в открытый порт
 */
void MainWindow::slotSendData() {
    if (!modbusDevice || modbusDevice->state() != QModbusDevice::ConnectedState) {
        QMessageBox::warning(this, "Ошибка", "Порт не открыт!");
        return;
    }

    // base = 0 позволяет автоматически определять систему счисления:
    // "0xFF" -> HEX, "0" -> OCT, "255" -> DEC
    auto parseByte = [](const QString &text, bool &ok) -> quint8 {
        int val = text.trimmed().toInt(&ok, 0);
        if (!ok || val < 0 || val > 255) {
            ok = false;
            return 0;
        }
        return static_cast<quint8>(val);
    };
    bool ok;

    // 1. Считываем и парсим поля из UI
    quint8 deviceAddr = parseByte(ui->leDeviceAddress->text(), ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный адрес устройства (ожидается 0-255 или 0x00-0xFF)!");
        return;
    }

    quint8 funcCode = static_cast<quint8>(ui->cbCode->currentData().toInt());

    bool okAddr1, okAddr2, okQty1, okQty2;
    quint8 addrHigh = parseByte(ui->leRegisterAddress1->text(), okAddr1);
    quint8 addrLow  = parseByte(ui->leRegisterAddress2->text(), okAddr2);
    quint8 qtyHigh  = parseByte(ui->leRegistersQty1->text(), okQty1);
    quint8 qtyLow   = parseByte(ui->leRegistersQty2->text(), okQty2);

    if (!okAddr1 || !okAddr2) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный адрес регистра!");
        return;
    }
    if (!okQty1 || !okQty2) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректное количество регистров / значение!");
        return;
    }

    // Собираем 16-битные значения из старшего и младшего байтов
    quint16 startAddress = (static_cast<quint16>(addrHigh) << 8) | addrLow;
    quint16 count = (static_cast<quint16>(qtyHigh) << 8) | qtyLow;

    // Очищаем предыдущий ответ, если он вдруг еще жив
    if (currentReply) {
        currentReply->deleteLater();
        currentReply = nullptr;
    }

    QModbusDataUnit request;
    bool isReadRequest = true;

    // Маппинг кода функции на тип данных QModbusDataUnit
    switch (funcCode) {
    case 0x01: request = QModbusDataUnit(QModbusDataUnit::Coils, startAddress, count); break;
    case 0x02: request = QModbusDataUnit(QModbusDataUnit::DiscreteInputs, startAddress, count); break;
    case 0x03: request = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, count); break;
    case 0x04: request = QModbusDataUnit(QModbusDataUnit::InputRegisters, startAddress, count); break;

    // Для записи (упрощенный пример, предполагается, что в поле "Количество" пользователь вводит значение для записи)
    case 0x05:
    case 0x06:
        isReadRequest = false;
        request = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, 1);
        request.setValue(0, count); // Используем поле count как значение для записи
        break;

    default:
        QMessageBox::warning(this, "Ошибка", "Данный код функции не поддерживается в этом примере");
        return;
    }

    // Отправка запроса
    if (isReadRequest) {
        currentReply = modbusDevice->sendReadRequest(request, deviceAddr);
    } else {
        currentReply = modbusDevice->sendWriteRequest(request, deviceAddr);
    }

    if (!currentReply) {
        QMessageBox::critical(this, "Ошибка", "Не удалось отправить запрос: " + modbusDevice->errorString());
        return;
    }

    // Если ответ пришел мгновенно (редко, но бывает), слот не вызовется, поэтому проверяем isFinished()
    if (currentReply->isFinished()) {
        onReplyFinished();
    } else {
        connect(currentReply, &QModbusReply::finished, this, &MainWindow::onReplyFinished);
    }

    ui->textEditLog->append("Запрос отправлен. Ожидание ответа...");
}


void MainWindow::onReplyFinished() {
    if (!currentReply) return;

    if (currentReply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = currentReply->result();

        QString regType;
        switch (unit.registerType()) {
        case QModbusDataUnit::RegisterType::HoldingRegisters:
            regType = "Holding Registers";
            break;
        case QModbusDataUnit::RegisterType::InputRegisters:
            regType = "Input Registers";
            break;
        case QModbusDataUnit::RegisterType::DiscreteInputs:
            regType = "Discrete Inputs";
            break;
        case QModbusDataUnit::RegisterType::Coils:
            regType = "Coils";
            break;
        default:
            regType = "Unknown type";
        }

        QString logMessage = QString("Успешный ответ Modbus!\n"
                                     "Тип регистра: %1\n"
                                     "Начальный адрес: %2\n"
                                     "Количество значений: %3")
                                 .arg(regType)
                                 .arg(unit.startAddress())
                                 .arg(unit.valueCount());

        ui->textEditLog->append(logMessage);
        // Выводим полученные значения
        QString resultStr;
        for (uint i = 0; i < unit.valueCount(); ++i) {
            const QString entry = QString("Адрес %1: %2 (0x%3)")
                                      .arg(unit.startAddress() + i)
                                      .arg(unit.value(i))
                                      .arg(unit.value(i), 4, 16, QChar('0')).toUpper();
            resultStr += entry + "\n";
        }
        ui->textEditRecievedData->append(resultStr);

    } else {
        qWarning() << "Ошибка ответа Modbus:" << currentReply->errorString();
    }

    // Обязательно удаляем объект ответа, чтобы избежать утечки памяти
    currentReply->deleteLater();
    currentReply = nullptr;
}


void MainWindow::setControlsForOpenPort() {
    ui->btnApply->setText("Закрыть");
    ui->cbPorts->setEnabled(false);
    ui->cbBaudRate->setEnabled(false);
    ui->cbDataBits->setEnabled(false);
    ui->cbParity->setEnabled(false);
    ui->cbStopBits->setEnabled(false);

    ui->btnSendData->setEnabled(true);
    ui->leDeviceAddress->setEnabled(true);
    ui->cbCode->setEnabled(true);
    ui->leRegisterAddress1->setEnabled(true);
    ui->leRegisterAddress2->setEnabled(true);
    ui->leRegistersQty1->setEnabled(true);
    ui->leRegistersQty2->setEnabled(true);
}

void MainWindow::setControlsForClosedPort() {
    ui->btnApply->setEnabled(true);
    ui->btnApply->setText("Открыть");
    ui->cbPorts->setEnabled(true);
    ui->cbBaudRate->setEnabled(true);
    ui->cbDataBits->setEnabled(true);
    ui->cbParity->setEnabled(true);
    ui->cbStopBits->setEnabled(true);

    ui->btnSendData->setEnabled(false);
    ui->leDeviceAddress->setEnabled(false);
    ui->cbCode->setEnabled(false);
    ui->leRegisterAddress1->setEnabled(false);
    ui->leRegisterAddress2->setEnabled(false);
    ui->leRegistersQty1->setEnabled(false);
    ui->leRegistersQty2->setEnabled(false);
}

/**
 * @brief Заполняет виджеты параметров отправки данных начальными значениями.
 */
void MainWindow::setControlsForSendData() {
    ui->leDeviceAddress->setText("0x01");
    ui->cbCode->clear();
    // addItem(Текст для отображения, Внутренние данные (int))
    ui->cbCode->addItem("01 (0x01) - Чтение дискретных выходов (Read Coils)", 0x01);
    ui->cbCode->addItem("02 (0x02) - Чтение дискретных входов (Read Discrete Inputs)", 0x02);
    ui->cbCode->addItem("03 (0x03) - Чтение регистров хранения (Read Holding Registers)", 0x03);
    ui->cbCode->addItem("04 (0x04) - Чтение входных регистров (Read Input Registers)", 0x04);
    ui->cbCode->addItem("05 (0x05) - Запись одного дискретного выхода (Write Single Coil)", 0x05);
    ui->cbCode->addItem("06 (0x06) - Запись одного регистра (Write Single Register)", 0x06);
    ui->cbCode->addItem("15 (0x0F) - Запись нескольких дискретных выходов (Write Multiple Coils)", 0x0F);
    ui->cbCode->addItem("16 (0x10) - Запись нескольких регистров (Write Multiple Registers)", 0x10);
    // Выбираем самую популярную функцию (0x03) по умолчанию
    ui->cbCode->setCurrentIndex(2);

    ui->leRegisterAddress1->setText("0x00");
    ui->leRegisterAddress2->setText("0x00");
    ui->leRegistersQty1->setText("0x00");
    ui->leRegistersQty2->setText("0x01");
}


