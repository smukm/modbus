#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "modbusvalidator.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

/**
 * @brief Конструктор главного окна приложения.
 * Инициализирует UI, создает экземпляр ModbusManager и настраивает все сигналы и слоты.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_modbusManager(new ModbusManager(this))
    , m_pollingTimer(new QTimer(this))
    , m_logManager(new LogManager(this))
    , m_registerModel(new RegisterDataModel(this))
    , m_uiController(new UiController(ui)) // Инициализация контроллера UI
{
    ui->setupUi(this);

    // Инициализация TableView для отображения данных
    ui->tvReceivedData->setModel(m_registerModel);

    // Инициализация ListView для отображения логов
    ui->lvLog->setModel(m_logManager);
    connect(m_logManager, &LogManager::logAdded, this, &MainWindow::onLogAdded);

    // Настройка таймера для периодического опроса
    m_pollingTimer->setInterval(2000); // Интервал опроса в миллисекундах (2 секунды)
    connect(m_pollingTimer, &QTimer::timeout, this, &MainWindow::onPollingTimeout);

    connect(m_modbusManager, &ModbusManager::connected, this, [this]() {
        m_logManager->addLog("✅ Modbus: Порт успешно подключен.");
        m_uiController->setConnectedState();
    });

    connect(m_modbusManager, &ModbusManager::disconnected, this, [this]() {
        m_logManager->addLog("⭕ Modbus: Порт отключен.");
        m_uiController->setDisconnectedState();
    });

    connect(m_modbusManager, &ModbusManager::errorOccurred, this, [this](const QString &error) {
        //stopPolling();
        //QMessageBox::warning(this, "Ошибка", error);
        m_logManager->addLog(error, true);
    });

    connect(m_modbusManager, &ModbusManager::errorCriticalOccured, this, [this](const QString &error) {
        stopPolling();
        QMessageBox::critical(this, "Ошибка", error);
    });

    // Подключение сигнала получения данных к специализированному слоту обработки
    connect(m_modbusManager, &ModbusManager::dataReceived, this, [this](const QModbusDataUnit &unit) {
        m_registerModel->updateData(unit);
        m_logManager->addLog("Данные успешно получены и обновлены в таблице.");
    });

    // Первичная инициализация элементов интерфейса
    createMenu();
    fillSettings();
    m_uiController->initializeCommandWidgets();
    m_uiController->setDisconnectedState();
    setPorts();

    // Подключение кнопок интерфейса к слотам
    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::onApplySettings);
    connect(ui->btnSendData, &QPushButton::clicked, this, &MainWindow::onExecuteCommand);
}

MainWindow::~MainWindow()
{
    if (m_pollingTimer) {
        m_pollingTimer->stop();
    }
    if (m_modbusManager) {
        m_modbusManager->disconnectFromDevice();
    }
    delete ui;
}

/**
 * @brief Создание меню
 */
void MainWindow::createMenu() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&Файл"));
    QAction *openAction = fileMenu->addAction(tr("&Открыть"));
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(tr("&Выход"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
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
        m_logManager->addLog("Попытка подключения к " + ui->cbPorts->currentData().toString());
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


void MainWindow::onExecuteCommand() {

    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте порт!");
        return;
    }

    // Если таймер уже запущен, кнопка работает как "Стоп"
    if (m_pollingTimer->isActive()) {
        stopPolling();
        return;
    }

    // Если таймер не запущен
    if (ui->cbPolling->isChecked()) {
        // Запускаем периодический опрос
        m_pollingTimer->start();
        m_uiController->setPollingActiveState(true);
        m_logManager->addLog("Периодический опрос запущен");
    }

    // Отправляем запрос (сразу для периодического или одиночный)
    sendData();
}

void MainWindow::stopPolling() {
    if (m_pollingTimer->isActive()) {
        m_pollingTimer->stop();
        ui->cbPolling->setChecked(false);
        ui->btnSendData->setText("Выполнить");
        m_uiController->setPollingActiveState(false); // Делегируем изменение UI
        m_logManager->addLog("Периодический опрос остановлен");
    }
}

/**
 * @brief Формирует и отправляет Modbus-запрос на основе данных из полей ввода.
 */
void MainWindow::sendData() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте порт!");
        return;
    }

    // Парсинг базовых значений из UI (с минимальной проверкой на пустоту)
    bool okAddr, okQty, okDev;
    quint8 deviceAddr = ui->leDeviceAddress->text().trimmed().toInt(&okDev, 0);
    quint8 funcCode = static_cast<quint8>(ui->cbCode->currentData().toInt());
    quint16 startAddress = ui->leRegisterAddress->text().trimmed().toInt(&okAddr, 0);
    quint16 count = ui->leRegistersQty->text().trimmed().toInt(&okQty, 0);

    if (!okDev || deviceAddr > 255) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный адрес устройства (0-255)!");
        stopPolling();
        return;
    }
    if (!okAddr || startAddress > 65535) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный адрес регистра (0-65535)!");
        stopPolling();
        return;
    }
    if (!okQty || count < 1 || count > 65535) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректное количество регистров (1-65535)!");
        stopPolling();
        return;
    }
    m_logManager->addLog(QString("%1:\n Адрес устройства: %2, адрес регистра: %3, кол-во: %4")
                             .arg(ui->cbCode->currentText()).arg(deviceAddr).arg(startAddress).arg(count));

    // Делегируем сложную валидацию специализированному классу
    ValidationResult basicCheck = ModbusValidator::validateBasicParams(deviceAddr, funcCode, startAddress, count);
    if (!basicCheck.isValid) {
        QMessageBox::warning(this, "Ошибка валидации", basicCheck.errorMessage);
        stopPolling();
        return;
    }

    bool isReadRequest = (funcCode == 0x01 || funcCode == 0x02 || funcCode == 0x03 || funcCode == 0x04);
    QVector<quint16> writeValues;

    // Валидация данных для записи
    if (!isReadRequest) {
        ValidationResult writeCheck = ModbusValidator::validateWriteData(funcCode, ui->leData->text(), writeValues);
        if (!writeCheck.isValid) {
            QMessageBox::warning(this, "Ошибка ввода", writeCheck.errorMessage);
            stopPolling();
            return;
        }

        // Логика корректировки count при несовпадении количества введенных значений
        if ((funcCode == 0x0F || funcCode == 0x10) && writeValues.size() != count) {
            m_logManager->addLog(QString("Количество введенных значений (%1) не совпадает с указанным (%2).\nБудет записано %1 значений.")
                                     .arg(writeValues.size()).arg(count), true);
            count = static_cast<quint16>(writeValues.size());
        }

        m_logManager->addLog(" Данные для записи: " + ui->leData->text().trimmed());
    }

    // Отправка запроса
    if (isReadRequest) {
        m_modbusManager->sendReadRequest(deviceAddr, funcCode, startAddress, count);
    } else {
        m_modbusManager->sendWriteRequest(deviceAddr, funcCode, startAddress, writeValues);
    }

    m_logManager->addLog("Запрос отправлен. Ожидание ответа...");
}

/**
 * @brief Слот, вызываемый таймером периодического опроса.
 * Автоматически отправляет Modbus-запрос с текущими параметрами.
 */
void MainWindow::onPollingTimeout() {
    if (m_modbusManager && m_modbusManager->isConnected()) {
        sendData();
    } else {
        m_pollingTimer->stop();
        ui->cbPolling->setChecked(false);
        m_logManager->addLog("Опрос остановлен: порт отключен");
        stopPolling();
    }
}

void MainWindow::onLogAdded() {
    // Автоматическая прокрутка к последней записи
    ui->lvLog->scrollToBottom();
}



