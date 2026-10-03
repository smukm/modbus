#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusReply>
#include <QMessageBox>

/**
 * @brief Конструктор главного окна приложения.
 * Инициализирует UI, создает экземпляр ModbusManager и настраивает все сигналы и слоты.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_modbusManager = new ModbusManager(this);
    connect(m_modbusManager, &ModbusManager::connected, this, [this]() {
        ui->textEditLog->append("✅ Modbus: Порт успешно подключен.");
        setControlsForOpenPort();
    });

    connect(m_modbusManager, &ModbusManager::disconnected, this, [this]() {
        ui->textEditLog->append("⭕ Modbus: Порт отключен.");
        setControlsForClosedPort();
    });

    connect(m_modbusManager, &ModbusManager::errorOccurred, this, [this](const QString &error) {
        QMessageBox::warning(this, "Ошибка", error);
    });

    connect(m_modbusManager, &ModbusManager::errorCriticalOccured, this, [this](const QString &error) {
        QMessageBox::critical(this, "Ошибка", error);
    });

    // Подключение сигнала получения данных к специализированному слоту обработки
    connect(m_modbusManager, &ModbusManager::dataReceived, this, &MainWindow::onModbusDataReceived);

    // Первичная инициализация элементов интерфейса
    setPorts();
    fillSettings();
    setControlsForSendData();
    setControlsForClosedPort();

    // Подключение кнопок интерфейса к слотам
    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::onApplySettings);
    connect(ui->btnSendData, &QPushButton::clicked, this, &MainWindow::onSendData);
}

MainWindow::~MainWindow()
{
    if (m_modbusManager) {
        m_modbusManager->disconnectFromDevice();
    }
    delete ui;
}

/**
 * @brief Сканирует систему и заполняет выпадающий список доступными COM-портами.
 * Блокирует кнопку подключения, если порты не найдены.
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
void MainWindow::onApplySettings() {
    if (!m_modbusManager) {
        return;
     }

    if (m_modbusManager->isConnected()) {
        m_modbusManager->disconnectFromDevice();
    } else {
        ui->textEditLog->append("Попытка подключения к " + ui->cbPorts->currentData().toString());
        ModbusConnectionSettings settings = {
            .portName = ui->cbPorts->currentData().toString(),
            .baudRate = ui->cbBaudRate->currentData().toInt(),
            .parity = ui->cbParity->currentData().toInt(),
            .dataBits = ui->cbDataBits->currentData().toInt(),
            .stopBits = ui->cbStopBits->currentData().toInt()
        };
        m_modbusManager->connectToDevice(settings);
    }
}

/**
 * @brief Формирует и отправляет Modbus-запрос на основе данных из полей ввода.
 */
void MainWindow::onSendData() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте порт!");
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

    // Парсинг и валидация полей ввода
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

    // Определение типа запроса (чтение или запись) на основе кода функции
    bool isReadRequest = true;

    switch (funcCode) {
    case 0x01:case 0x02:case 0x03:case 0x04:
        // Это команды чтения, isReadRequest остается true
        break;
    case 0x05:case 0x06:
        // Это команды записи одного элемента
        isReadRequest = false;
        break;
    default:
        QMessageBox::warning(this, "Ошибка", "Данный код функции не поддерживается в этом примере");
        return;
    }

    // Делегирование отправки данных менеджеру
    if (isReadRequest) {
        m_modbusManager->sendReadRequest(deviceAddr, funcCode, startAddress, count);
    } else {
        m_modbusManager->sendWriteRequest(deviceAddr, funcCode, startAddress, count);
    }


    ui->textEditLog->append("Запрос отправлен. Ожидание ответа...");
}

/**
 * @brief Слот обработки успешного ответа от устройства Modbus.
 * Форматирует полученные данные и выводит их в лог и поле результатов.
 */
void MainWindow::onModbusDataReceived(const QModbusDataUnit &unit) {
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

    // Формирование сводного сообщения об успехе
    QString logMessage = QString("Успешный ответ Modbus!\n"
                                 "Тип регистра: %1\n"
                                 "Начальный адрес: %2\n"
                                 "Количество значений: %3")
                             .arg(regType)
                             .arg(unit.startAddress())
                             .arg(unit.valueCount());

    ui->textEditLog->append(logMessage);

    // Формирование детального списка полученных значений
    QString resultStr;
    for (uint i = 0; i < unit.valueCount(); ++i) {
        const QString entry = QString("Адрес %1: %2 (0x%3)")
                                  .arg(unit.startAddress() + i)
                                  .arg(unit.value(i))
                                  .arg(unit.value(i), 4, 16, QChar('0')).toUpper();
        resultStr += entry + "\n";
    }
    ui->textEditRecievedData->append(resultStr);
}

/**
 * @brief Блокирует элементы изменения настроек и активирует элементы отправки данных.
 * Вызывается при успешном подключении.
 */
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

/**
 * @brief Разблокирует элементы изменения настроек и деактивирует отправку данных.
 * Вызывается при отключении от устройства.
 */
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


