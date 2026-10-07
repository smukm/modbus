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
        m_logManager->addLog(error, true);
    });

    connect(m_modbusManager, &ModbusManager::errorCriticalOccurred, this, [this](const QString &error) {
        stopPolling();
        QMessageBox::critical(this, "Ошибка", error);
    });

    // Подключение сигнала получения данных к специализированному слоту обработки
    connect(m_modbusManager, &ModbusManager::dataReceived, this, [this](const QModbusDataUnit &unit) {
        m_registerModel->updateData(unit);
        m_logManager->addLog("Данные успешно получены и обновлены в таблице.");
    });
    // Новое подключение для отслеживания успешного завершения записи
    connect(m_modbusManager, &ModbusManager::writeCompleted, this, [this]() {
        m_logManager->addLog("✅ Запись данных успешно завершена.");
    });

    // Первичная инициализация элементов интерфейса
    createMenu();
    //fillSettings();
    m_uiController->initializePortSettingsCombo();
    m_uiController->initializeCommandWidgets();
    m_uiController->setDisconnectedState();
    setPorts();

    // Подключение кнопок интерфейса к слотам
    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::onApplySettings);
    connect(ui->btnSendData, &QPushButton::clicked, this, &MainWindow::onStartReading);
    connect(ui->btnExecuteOnce, &QPushButton::clicked, this, &MainWindow::onStartWriting);
    connect(ui->btnClearLogs, &QPushButton::clicked, this, [this]() {
        m_logManager->clear();
    });
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


void MainWindow::onStartReading() {

    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте порт!");
        return;
    }

    // Если таймер уже запущен, кнопка работает как "Стоп"
    if (m_pollingTimer->isActive()) {
        stopPolling();
        return;
    }

    // Запускаем периодический опрос
    m_pollingTimer->start();
    m_uiController->setPollingActiveState(true);
    m_logManager->addLog("Периодический опрос запущен");

    // Сразу отправляем первый запрос
    sendReadData();
}

void MainWindow::onStartWriting() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте порт!");
        return;
    }

    m_logManager->addLog("▶ Выполнение одиночной команды записи...");
    sendWriteData();
}

void MainWindow::stopPolling() {
    if (m_pollingTimer->isActive()) {
        m_pollingTimer->stop();
        ui->btnSendData->setText("Выполнить");
        m_uiController->setPollingActiveState(false); // Делегируем изменение UI
        m_logManager->addLog("Периодический опрос остановлен");
    }
}

/**
 * @brief Формирует и отправляет Modbus-запрос на основе данных из полей ввода.
 */
void MainWindow::sendReadData() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте порт!");
        return;
    }

    // Парсинг базовых значений из UI (с минимальной проверкой на пустоту)
    bool okAddr, okQty, okDev;
    quint8 deviceAddr = ui->leReadDeviceAddress->text().trimmed().toInt(&okDev, 0);
    quint8 funcCode = static_cast<quint8>(ui->cbReadCode->currentData().toInt());
    quint16 startAddress = ui->leReadRegisterAddress->text().trimmed().toInt(&okAddr, 0);
    quint16 count = ui->leReadRegistersQty->text().trimmed().toInt(&okQty, 0);

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
    // Логируем параметры только при старте опроса, чтобы не спамить
     if (!m_pollingTimer->isActive() || m_registerModel->rowCount() == 0) {
         m_logManager->addLog(QString("Чтение: %1 | Addr: %2, Reg: %3, Qty: %4")
                                  .arg(ui->cbReadCode->currentText()).arg(deviceAddr).arg(startAddress).arg(count));
     }

    // Делегируем сложную валидацию специализированному классу
    ValidationResult basicCheck = ModbusValidator::validateBasicParams(deviceAddr, funcCode, startAddress, count);
    if (!basicCheck.isValid) {
        QMessageBox::warning(this, "Ошибка валидации", basicCheck.errorMessage);
        stopPolling();
        return;
    }

    m_modbusManager->sendReadRequest(deviceAddr, funcCode, startAddress, count);

    m_logManager->addLog("Запрос отправлен. Ожидание ответа...");
}

void MainWindow::sendWriteData() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) return;

    bool okAddr, okQty, okDev;
    quint8 deviceAddr = ui->leWriteDeviceAddress->text().trimmed().toInt(&okDev, 0);
    quint8 funcCode = static_cast<quint8>(ui->cbWriteCode->currentData().toInt());
    quint16 startAddress = ui->leWriteRegisterAddress->text().trimmed().toInt(&okAddr, 0);
    quint16 count = ui->leWriteRegistersQty->text().trimmed().toInt(&okQty, 0);

    if (!okDev || deviceAddr > 255) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный адрес устройства (0-255)!");
        return;
    }
    if (!okAddr || startAddress > 65535) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный адрес регистра (0-65535)!");
        return;
    }
    if (!okQty || count < 1 || count > 65535) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректное количество регистров (1-65535)!");
        return;
    }
    m_logManager->addLog(QString("Запись: %1 | Addr: %2, Reg: %3, Qty: %4")
                             .arg(ui->cbWriteCode->currentText()).arg(deviceAddr).arg(startAddress).arg(count));

    ValidationResult basicCheck = ModbusValidator::validateBasicParams(deviceAddr, funcCode, startAddress, count);
    if (!basicCheck.isValid) {
        QMessageBox::warning(this, "Ошибка валидации", basicCheck.errorMessage);
        return;
    }

    QVector<quint16> writeValues;
    ValidationResult writeCheck = ModbusValidator::validateWriteData(funcCode, ui->leData->text(), writeValues);
    if (!writeCheck.isValid) {
        QMessageBox::warning(this, "Ошибка ввода", writeCheck.errorMessage);
        return;
    }

    if ((funcCode == 0x0F || funcCode == 0x10) && writeValues.size() != count) {
        m_logManager->addLog(QString("Кол-во значений (%1) не совпадает с указанным (%2). Будет записано %1.")
                                 .arg(writeValues.size()).arg(count), true);
    }

    m_logManager->addLog(" Данные для записи: " + ui->leData->text().trimmed());

    m_modbusManager->sendWriteRequest(deviceAddr, funcCode, startAddress, writeValues);
}

/**
 * @brief Слот, вызываемый таймером периодического опроса.
 * Автоматически отправляет Modbus-запрос с текущими параметрами.
 */
void MainWindow::onPollingTimeout() {
    if (m_modbusManager && m_modbusManager->isConnected()) {
        sendReadData();
    } else {
        stopPolling();
        m_logManager->addLog("Опрос остановлен: порт отключен");
    }
}

void MainWindow::onLogAdded() {
    // Автоматическая прокрутка к последней записи
    ui->lvLog->scrollToBottom();
}



