#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "modbusvalidator.h"
#include "settingsmanager.h"
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
        m_logManager->addLog(tr("✅ Modbus: Port connected successfully."));
        m_uiController->setConnectedState();
    });

    connect(m_modbusManager, &ModbusManager::disconnected, this, [this]() {
        m_logManager->addLog(tr("⭕ Modbus: Port disconnected."));
        m_uiController->setDisconnectedState();
    });

    connect(m_modbusManager, &ModbusManager::errorOccurred, this, [this](const QString &error) {
        m_logManager->addLog(error, true);
    });

    connect(m_modbusManager, &ModbusManager::errorCriticalOccurred, this, [this](const QString &error) {
        stopPolling();
        QMessageBox::critical(this, tr("Critical Error"), error);
    });

    // Подключение сигнала получения данных к специализированному слоту обработки
    connect(m_modbusManager, &ModbusManager::dataReceived, this, [this](quint8 serverAddress, const QModbusDataUnit &unit) {
        m_registerModel->updateData(serverAddress, unit);
        m_logManager->addLog(tr("Data received from device #%1").arg(serverAddress));
    });
    // Подключение для отслеживания успешного завершения записи
    connect(m_modbusManager, &ModbusManager::writeCompleted, this, [this]() {
        m_logManager->addLog(tr("✅ Data write completed successfully."));
    });
    // Подключение для активации кнопки Clear errors
    connect(m_modbusManager, &ModbusManager::deviceExcludedFromPolling, this, [this](quint8 serverAddress, const QString& msg) {
        m_logManager->addLog(msg, true);
        ui->btnClearErrors->setEnabled(true);

        m_registerModel->setExcludeDevice(serverAddress);
    });

    // Первичная инициализация элементов интерфейса
    createMenu();
    m_uiController->initializePortSettingsCombo();
    loadPortSettings();
    m_uiController->initializeCommandWidgets();
    m_uiController->setDisconnectedState();
    loadLastCommandParams();
    setPorts();

    // Подключение кнопок интерфейса к слотам
    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::onOpenPort);
    connect(ui->btnSendData, &QPushButton::clicked, this, &MainWindow::onStartReading);
    connect(ui->btnExecuteOnce, &QPushButton::clicked, this, &MainWindow::onStartWriting);
    connect(ui->btnClearLogs, &QPushButton::clicked, this, [this]() {
        m_logManager->clear();
    });
    connect(ui->btnClearErrors, &QPushButton::clicked, this, [this]() {
        m_modbusManager->clearAllExclusions();
        m_registerModel->clearAllExclusions();
        ui->btnClearErrors->setEnabled(false);
        m_logManager->addLog(tr("✅ Device exclusions cleared."));
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
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAction = fileMenu->addAction(tr("&Open"));
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(tr("&Exit"));
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
        ui->cbPorts->addItem(tr("No available COM ports"));
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
 * @brief Применяет сохраненные настройки
 */
void MainWindow::loadPortSettings() {
    ModbusConnectionSettings savedSettings = SettingsManager::loadPortSettings();
    if (!savedSettings.portName.isEmpty()) {
        int portIndex = ui->cbPorts->findData(savedSettings.portName);
        if (portIndex != -1) {
            ui->cbPorts->setCurrentIndex(portIndex);
        }
    }
    // Устанавливаем остальные параметры по их числовым значениям (data)
    auto setComboByData = [](QComboBox *cb, int value) {
        int idx = cb->findData(value);
        if (idx != -1) cb->setCurrentIndex(idx);
    };
    setComboByData(ui->cbBaudRate, savedSettings.baudRate);
    setComboByData(ui->cbParity,   savedSettings.parity);
    setComboByData(ui->cbDataBits, savedSettings.dataBits);
    setComboByData(ui->cbStopBits, savedSettings.stopBits);
}

/**
 * @brief Сохраняет настройки в json
 */
void MainWindow::savePortSettings(const ModbusConnectionSettings& settings) {
    if (SettingsManager::savePortSettings(settings)) {
        m_logManager->addLog(tr("💾 Port settings saved."));
    } else {
        m_logManager->addLog(tr("⚠ Failed to save port settings."), true);
    }
}

/**
 * @brief Загружает последние параметры чтения/записи в UI
 */
void MainWindow::loadLastCommandParams() {
    ModbusLastParams params = SettingsManager::loadLastParams();

    // Восстанавливаем параметры чтения
    ui->leReadDeviceAddress->setText(params.readDeviceAddrs);
    int readCodeIdx = ui->cbReadCode->findData(params.readFuncCode);
    if (readCodeIdx != -1) ui->cbReadCode->setCurrentIndex(readCodeIdx);

    ui->leReadRegisterAddress->setText(QString::number(params.readStartAddr));
    ui->leReadRegistersQty->setText(QString::number(params.readCount));

    // Восстанавливаем параметры записи
    ui->leWriteDeviceAddress->setText(QString::number(params.writeDeviceAddr));

    int writeCodeIdx = ui->cbWriteCode->findData(params.writeFuncCode);
    if (writeCodeIdx != -1) ui->cbWriteCode->setCurrentIndex(writeCodeIdx);

    ui->leWriteRegisterAddress->setText(QString::number(params.writeStartAddr));
    ui->leWriteRegistersQty->setText(QString::number(params.writeCount));
    ui->leData->setText(params.writeData);
}

/**
 * @brief Считывает текущие значения из UI и сохраняет их как "последние использованные"
 */
void MainWindow::saveLastCommandParams() {
    ModbusLastParams params;

    // Считываем параметры чтения
    params.readDeviceAddrs = ui->leReadDeviceAddress->text().trimmed();
    params.readFuncCode = static_cast<quint8>(ui->cbReadCode->currentData().toInt());
    params.readStartAddr = ui->leReadRegisterAddress->text().trimmed().toUInt();
    params.readCount = ui->leReadRegistersQty->text().trimmed().toUInt();

    // Считываем параметры записи
    params.writeDeviceAddr = ui->leWriteDeviceAddress->text().trimmed().toUInt();
    params.writeFuncCode = static_cast<quint8>(ui->cbWriteCode->currentData().toInt());
    params.writeStartAddr = ui->leWriteRegisterAddress->text().trimmed().toUInt();
    params.writeCount = ui->leWriteRegistersQty->text().trimmed().toUInt();
    params.writeData = ui->leData->text().trimmed();

    SettingsManager::saveLastParams(params);
}

/**
 * @brief Слот-обработчик нажатия кнопки "Открыть/Закрыть" (btnApply).
 * Реализует логику переключения (toggle): если порт открыт — закрываем его,
 * если закрыт — считываем настройки из UI и открываем.
 */
void MainWindow::onOpenPort() {
    if (!m_modbusManager) {
        return;
     }

    if (m_modbusManager->isConnected()) {
        m_modbusManager->disconnectFromDevice();
    } else {
        m_logManager->addLog(tr("Attempting to connect to %1").arg(ui->cbPorts->currentData().toString()));
        ModbusConnectionSettings settings = {
            .portName = ui->cbPorts->currentData().toString(),
            .baudRate = ui->cbBaudRate->currentData().toInt(),
            .parity = ui->cbParity->currentData().toInt(),
            .dataBits = ui->cbDataBits->currentData().toInt(),
            .stopBits = ui->cbStopBits->currentData().toInt()
        };

        savePortSettings(settings);

        m_modbusManager->connectToDevice(settings);
    }
}


void MainWindow::onStartReading() {

    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, tr("Error"), tr("Open the port first!"));
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
    m_logManager->addLog(tr("Periodic polling started"));

    // Сразу отправляем первый запрос
    sendReadData();
}

void MainWindow::onStartWriting() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, tr("Error"), tr("Open the port first!"));
        return;
    }

    m_logManager->addLog(tr("▶ Executing single write command..."));
    sendWriteData();
}

void MainWindow::stopPolling() {
    if (m_pollingTimer->isActive()) {
        m_pollingTimer->stop();
        ui->btnSendData->setText(tr("Execute"));
        m_uiController->setPollingActiveState(false); // Делегируем изменение UI
        m_logManager->addLog(tr("Periodic polling stopped"));
    }
}

/**
 * @brief Формирует и отправляет Modbus-запрос на основе данных из полей ввода.
 */
void MainWindow::sendReadData() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) {
        QMessageBox::warning(this, tr("Error"), tr("Open the port first!"));
        return;
    }

    if (m_modbusManager->isProcessing()) {
        return;
    }

    // Получаем и парсим список адресов
    QString addrsText = ui->leReadDeviceAddress->text().trimmed();
    if (addrsText.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Enter the device address!"));
        stopPolling();
        return;
    }

    const QStringList addrStrings = addrsText.split(',', Qt::SkipEmptyParts);
    if (addrStrings.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Invalid address list format!"));
        stopPolling();
        return;
    }

    // Парсим общие параметры (они одинаковы для всех устройств в этом запросе)
    bool okAddr, okQty;
    quint8 funcCode = static_cast<quint8>(ui->cbReadCode->currentData().toInt());
    quint16 startAddress = ui->leReadRegisterAddress->text().trimmed().toInt(&okAddr, 0);
    quint16 count = ui->leReadRegistersQty->text().trimmed().toInt(&okQty, 0);

    if (!okAddr || startAddress > 65535) {
        QMessageBox::warning(this, tr("Input Error"), tr("Invalid register address (0-65535)!"));
        stopPolling();
        return;
    }
    if (!okQty || count < 1 || count > 65535) {
        QMessageBox::warning(this, tr("Input Error"), tr("Invalid number of registers (1-65535)!"));
        stopPolling();
        return;
    }

    // Проходим по каждому адресу и ставим запрос в очередь
    int successCount = 0;
    for (const QString &addrStr : addrStrings) {
        bool okDev;
        // Modbus допустимый диапазон адресов устройств: 1-247 (0 - широковещательный)
        quint8 deviceAddr = addrStr.trimmed().toUInt(&okDev);

        if (!okDev || deviceAddr == 0 || deviceAddr > 247) {
            m_logManager->addLog(tr("⚠ Skipping invalid device address: '%1' (допустимо 1-247)").arg(addrStr), true);
            continue;
        }

        ValidationResult basicCheck = ModbusValidator::validateBasicParams(deviceAddr, funcCode, startAddress, count);
        if (!basicCheck.isValid) {
            m_logManager->addLog(tr("⚠ Validation error for address %1: %2").arg(deviceAddr).arg(basicCheck.errorMessage), true);
            continue;
        }

        // Ставим запрос в очередь
        m_modbusManager->sendReadRequest(deviceAddr, funcCode, startAddress, count);
        successCount++;
    }


    if (successCount > 0) {
        saveLastCommandParams();

        if (!m_pollingTimer->isActive() || m_registerModel->rowCount() == 0) {
            m_logManager->addLog(tr("📡 Requests sent: %1. Waiting for responses...").arg(successCount));
        }
    } else {
        m_logManager->addLog(tr("❌ Failed to generate any valid request."), true);
        stopPolling();
    }
}

void MainWindow::sendWriteData() {
    if (!m_modbusManager || !m_modbusManager->isConnected()) return;

    bool okAddr, okQty, okDev;
    quint8 deviceAddr = ui->leWriteDeviceAddress->text().trimmed().toInt(&okDev, 0);
    quint8 funcCode = static_cast<quint8>(ui->cbWriteCode->currentData().toInt());
    quint16 startAddress = ui->leWriteRegisterAddress->text().trimmed().toInt(&okAddr, 0);
    quint16 count = ui->leWriteRegistersQty->text().trimmed().toInt(&okQty, 0);

    if (!okDev || deviceAddr > 255) {
        QMessageBox::warning(this, tr("Input Error"), tr("Invalid device address (0-255)!"));
        return;
    }
    if (!okAddr || startAddress > 65535) {
        QMessageBox::warning(this, tr("Input Error"), tr("Invalid register address (0-65535)!"));
        return;
    }
    if (!okQty || count < 1 || count > 65535) {
        QMessageBox::warning(this, tr("Input Error"), tr("Invalid number of registers (1-65535)!"));
        return;
    }
    m_logManager->addLog(tr("Write: %1 | Addr: %2, Reg: %3, Qty: %4")
                             .arg(ui->cbWriteCode->currentText()).arg(deviceAddr).arg(startAddress).arg(count));

    ValidationResult basicCheck = ModbusValidator::validateBasicParams(deviceAddr, funcCode, startAddress, count);
    if (!basicCheck.isValid) {
        QMessageBox::warning(this, tr("Validation Error"), basicCheck.errorMessage);
        return;
    }

    QVector<quint16> writeValues;
    ValidationResult writeCheck = ModbusValidator::validateWriteData(funcCode, ui->leData->text(), writeValues);
    if (!writeCheck.isValid) {
        QMessageBox::warning(this, tr("Input Error"), writeCheck.errorMessage);
        return;
    }

    if ((funcCode == 0x0F || funcCode == 0x10) && writeValues.size() != count) {
        m_logManager->addLog(tr("Number of values (%1) does not match the specified (%2). will be written %1.")
                                 .arg(writeValues.size()).arg(count), true);
    }

    m_logManager->addLog(tr("Data to write: %1").arg(ui->leData->text().trimmed()));

    saveLastCommandParams();

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
        m_logManager->addLog(tr("Polling stopped: port disconnected"));
    }
}

void MainWindow::onLogAdded() {
    // Автоматическая прокрутка к последней записи
    ui->lvLog->scrollToBottom();
}



