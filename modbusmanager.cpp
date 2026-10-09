#include "modbusmanager.h"
#include <qdebug.h>
#include <qvariant.h>

/**
 * @brief Конструктор класса ModbusManager.
 * Инициализирует Modbus-клиент, подключает сигналы состояния и ошибок,
 * а также задает базовые настройки надежности.
 */
ModbusManager::ModbusManager(QObject *parent)
    : QObject{parent}
    , m_modbusDevice(new QModbusRtuSerialClient(this))
    , m_currentReply(nullptr)
    , m_isProcessing(false)
{
    // Подключаем сигналы для отслеживания состояния соединения и системных ошибок
    connect(m_modbusDevice, &QModbusClient::stateChanged, this, &ModbusManager::onStateChanged);
    connect(m_modbusDevice, &QModbusClient::errorOccurred, this, &ModbusManager::onErrorOccurred);

    // Базовые настройки надежности: таймаут 1000 мс и 3 попытки повторной отправки при сбое
    m_modbusDevice->setTimeout(1000);
    m_modbusDevice->setNumberOfRetries(3);
}

/**
 * @brief Деструктор. Гарантирует корректное закрытие соединения перед удалением объекта.
 */
ModbusManager::~ModbusManager() {
    disconnectFromDevice();
}

/**
 * @brief Инициирует подключение к COM-порту с заданными настройками.
 * @param settings Структура с параметрами подключения (порт, скорость, четность и т.д.).
 */
void ModbusManager::connectToDevice(const ModbusConnectionSettings &settings) {
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, settings.portName);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, settings.baudRate);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, settings.parity);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, settings.dataBits);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, settings.stopBits);

    if (!m_modbusDevice->connectDevice()) {
        emit errorCriticalOccurred(tr("Failed to initiate connection: ") + m_modbusDevice->errorString());
    }
}

/**
 * @brief Разрывает текущее соединение с устройством.
 */
void ModbusManager::disconnectFromDevice() {
    if (m_modbusDevice) {
        m_modbusDevice->disconnectDevice();
    }
    // Очищаем очередь и сбрасываем флаги при разрыве соединения
    m_requestQueue.clear();
    m_isProcessing = false;

    // При отключении очищаем историю таймаутов и черные списки
    m_timeoutHistory.clear();
    m_excludedDevices.clear();

    if (m_currentReply) {
        // Отключаем сигнал, чтобы onReplyFinished не вызвался для удаляемого объекта
        disconnect(m_currentReply, &QModbusReply::finished, this, &ModbusManager::onReplyFinished);
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

/**
 * @brief Проверяет, находится ли устройство в состоянии успешного подключения.
 * @return true, если порт открыт и готов к обмену данными; иначе false.
 */
bool ModbusManager::isConnected() const {
    return m_modbusDevice && m_modbusDevice->state() == QModbusDevice::ConnectedState;
}

/**
 * @brief Отправляет запрос на чтение данных с устройства Modbus.
 * @param serverAddress Адрес устройства (Slave ID).
 * @param funcCode Код функции Modbus (0x01, 0x02, 0x03, 0x04).
 * @param startAddress Начальный адрес регистра для чтения.
 * @param count Количество запрашиваемых регистров/коиллов.
 */
void ModbusManager::sendReadRequest(
    quint8 serverAddress,
    quint8 funcCode,
    quint16 startAddress,
    quint16 count
    ) {
    if (!isConnected()) {
        emit errorOccurred(tr("Port is not open!"));
        return;
    };

    // Проверка, если устройство в черном списке
    if (m_excludedDevices.contains(serverAddress)) {
        emit errorOccurred(tr("Device %1 is excluded from polling due to consecutive timeouts.").arg(serverAddress));
        return;
    }

    ModbusRequest req;
    req.type = ModbusRequest::Type::Read;
    req.serverAddress = serverAddress;
    req.funcCode = funcCode;
    req.startAddress = startAddress;
    req.count = count;

    // Добавляем запрос на чтение в конец очереди (низкий приоритет)
    m_requestQueue.enqueue(req);
    processNextRequest();
}

/**
 * @brief Отправляет запрос на запись данных в устройство Modbus.
 * @param serverAddress Адрес устройства (Slave ID).
 * @param funcCode Код функции Modbus (0x05, 0x06, 0x0F или 0x10).
 * @param startAddress Адрес регистра/коилла для записи.
 */
void ModbusManager::sendWriteRequest(
    quint8 serverAddress,
    quint8 funcCode,
    quint16 startAddress,
    const QVector<quint16> &values)
{
    if (!isConnected()) {
        emit errorOccurred(tr("Port is not open!"));
        return;
    }

    if (values.isEmpty()) {
        emit errorOccurred(tr("No data to write!"));
        return;
    }

    // Проверка, если устройство в черном списке
    if (m_excludedDevices.contains(serverAddress)) {
        emit errorOccurred(tr("Device %1 is excluded. Clear exclusions before writing.").arg(serverAddress));
        return;
    }

    ModbusRequest req;
    req.type = ModbusRequest::Type::Write;
    req.serverAddress = serverAddress;
    req.funcCode = funcCode;
    req.startAddress = startAddress;
    req.count = static_cast<quint16>(values.size());
    req.values = values;

    // Добавляем запрос на запись в начало очереди (высокий приоритет)
    m_requestQueue.prepend(req);
    processNextRequest();
}

void ModbusManager::processNextRequest() {
    // Если уже идет обработка, очередь пуста или порт закрыт — выходим
    if (m_isProcessing || m_requestQueue.isEmpty() || !isConnected()) {
        return;
    }

    m_isProcessing = true;
    m_currentRequest = m_requestQueue.dequeue();

    if (m_currentReply) {
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    if (m_currentRequest.type == ModbusRequest::Type::Read) {
        QModbusDataUnit::RegisterType type;
        switch (m_currentRequest.funcCode) {
        case 0x01: type = QModbusDataUnit::Coils; break;
        case 0x02: type = QModbusDataUnit::DiscreteInputs; break;
        case 0x03: type = QModbusDataUnit::HoldingRegisters; break;
        case 0x04: type = QModbusDataUnit::InputRegisters; break;
        default:
            emit errorOccurred(tr("Unsupported function code for reading"));
            m_isProcessing = false;
            processNextRequest(); // Пробуем следующий запрос, если этот невалиден
            return;
        }

        QModbusDataUnit request(type, m_currentRequest.startAddress, m_currentRequest.count);
        m_currentReply = m_modbusDevice->sendReadRequest(request, m_currentRequest.serverAddress);

    } else if (m_currentRequest.type == ModbusRequest::Type::Write) {
        QModbusDataUnit::RegisterType type;
        if (m_currentRequest.funcCode == 0x05 || m_currentRequest.funcCode == 0x0F) {
            type = QModbusDataUnit::Coils;
        } else if (m_currentRequest.funcCode == 0x06 || m_currentRequest.funcCode == 0x10) {
            type = QModbusDataUnit::HoldingRegisters;
        } else {
            emit errorOccurred(tr("Unsupported function code for writing (supported 0x05, 0x06, 0x0F, 0x10)"));
            m_isProcessing = false;
            processNextRequest();
            return;
        }

        QModbusDataUnit request(type, m_currentRequest.startAddress, m_currentRequest.count);
        for (int i = 0; i < m_currentRequest.count; ++i) {
            request.setValue(i, m_currentRequest.values[i]);
        }

        m_currentReply = m_modbusDevice->sendWriteRequest(request, m_currentRequest.serverAddress);
    }

    if (m_currentReply) {
        if (!m_currentReply->isFinished()) {
            connect(m_currentReply, &QModbusReply::finished, this, &ModbusManager::onReplyFinished);
        } else {
            onReplyFinished();
        }
    } else {
        emit errorCriticalOccurred(tr("Failed to send request: ") + m_modbusDevice->errorString());
        m_isProcessing = false;
        processNextRequest(); // Чтобы очередь не застряла в случае ошибки
    }
}

/**
 * @brief Слот-обработчик изменения состояния подключения.
 * Транслирует внутренние состояния QModbusDevice в понятные сигналы для UI.
 */
void ModbusManager::onStateChanged(QModbusDevice::State state) {
    switch (state) {
    case QModbusDevice::UnconnectedState:
        qDebug() << tr("ModbusManager: Device disconnected.");
        emit disconnected();
        break;

    case QModbusDevice::ConnectedState:
        qDebug() << tr("ModbusManager: Device successfully connected.");
        emit connected();
        break;

    case QModbusDevice::ConnectingState:
        qDebug() << tr("ModbusManager: Attempting to connect...");
        // Сигнал не отправляем, так как процесс еще идет
        break;

    case QModbusDevice::ClosingState:
        qDebug() << tr("ModbusManager: Closing connection...");
        // Сигнал не отправляем, дождемся UnconnectedState
        break;

    default:
        qWarning() << tr("ModbusManager: Unknown device state.");
        break;
    }
}

/**
 * @brief Слот-обработчик возникновения ошибок на уровне устройства (например, обрыв связи).
 * Игнорирует состояние NoError и транслирует остальные ошибки в сигнал.
 */
void ModbusManager::onErrorOccurred(QModbusDevice::Error error) {
    if (error == QModbusDevice::NoError) return;
    emit errorOccurred(m_modbusDevice->errorString());
}

/**
 * @brief Слот, вызываемый при завершении обработки запроса (успешно или с ошибкой).
 * Извлекает данные или формирует понятное сообщение об ошибке (включая Modbus Exception).
 */
void ModbusManager::onReplyFinished() {
    if (!m_currentReply) return;

    if (m_currentReply->error() == QModbusDevice::NoError) {
        // Запрос выполнен успешно, извлекаем блок данных и отправляем его в UI
        if (m_currentRequest.type == ModbusRequest::Type::Read) {
            const QModbusDataUnit unit = m_currentReply->result();
            emit dataReceived(m_currentRequest.serverAddress, unit);
        } else {
            emit writeCompleted();
        }
    } else if (m_currentReply->error() == QModbusDevice::ProtocolError) {
        // Специфическая ошибка: устройство ответило, но вернуло Modbus Exception (исключение).
        // Это означает, что запрос был получен, но отвергнут устройством (неверный адрес, функция и т.д.).
        QString exceptionInfo = tr("Unknown error");
        if (m_currentReply->rawResult().exceptionCode() == 0x01) {
            exceptionInfo = tr("Illegal Function (Function not supported by device)");
        } else if (m_currentReply->rawResult().exceptionCode() == 0x02) {
            exceptionInfo = tr("Illegal Data Address (Such register/coil address does not exist or is unavailable)");
        } else if (m_currentReply->rawResult().exceptionCode() == 0x03) {
            exceptionInfo = tr("Illegal Data Value (Value is out of allowed range)");
        } else if (m_currentReply->rawResult().exceptionCode() == 0x04) {
            exceptionInfo = tr("Slave Device Failure (Internal device error)");
        } else if (m_currentReply->rawResult().exceptionCode() == 0x06) {
            exceptionInfo = tr("Slave Device Busy (Device is busy)");
        }

        emit errorOccurred(
            tr("Device rejected the request (Modbus Exception).\n"
                    "Error code: %1\n"
                    "Description: %2\n"
                    "Device address (Slave ID): %3\n"
                    "Register address: %4\n\n"
                    "What to check:\n"
                    "1. Does this address exactly exist in the device manual?\n"
                    "2. Did you confuse the function (0x05 for coils, 0x06 for numeric registers)?")
                .arg(m_currentReply->rawResult().exceptionCode())
                .arg(exceptionInfo)
                .arg(m_currentRequest.serverAddress)
                .arg(m_currentRequest.startAddress)
            );
    } else if (m_currentReply->error() == QModbusDevice::TimeoutError) {
        // Ошибка таймаута: устройство не ответило в течение заданного времени (по умолчанию 1000 мс)
        emit errorOccurred(
            tr("Timeout waiting for device %1 response.").arg(m_currentRequest.serverAddress)
        );

        recordTimeout(m_currentRequest.serverAddress);
    } else {
        // Другие ошибки (например, физический обрыв линии, ошибка чтения/записи на уровне ОС)
        emit errorOccurred(
            tr("Modbus response error: %1\n"
                    "Device address (Slave ID): %2\n"
                    "Register address: %3")
                .arg(m_currentReply->errorString())
                .arg(m_currentRequest.serverAddress)
                .arg(m_currentRequest.startAddress)
            );
    }

    // Обязательно удаляем объект ответа, чтобы избежать утечки памяти
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
    m_isProcessing = false;

    // Запускаем обработку следующего запроса из очереди
    processNextRequest();
}


void ModbusManager::recordTimeout(quint8 serverAddress) {
    if (m_excludedDevices.contains(serverAddress)) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    m_timeoutHistory[serverAddress].enqueue(now);

    // Очищаем старые записи, которые вышли за пределы временного окна
    while (!m_timeoutHistory[serverAddress].isEmpty()) {
        QDateTime oldest = m_timeoutHistory[serverAddress].head();
        if (oldest.msecsTo(now) > TIMEOUT_WINDOW_MS) {
            m_timeoutHistory[serverAddress].dequeue();
        } else {
            break;
        }
    }

    // Проверяем, достигнут ли порог
    if (m_timeoutHistory[serverAddress].size() >= MAX_TIMEOUTS) {
        QString msg = tr("ModbusManager: Device %1 excluded from polling due to %2 timeouts within %3 ms")
                          .arg(serverAddress)
                          .arg(MAX_TIMEOUTS)
                          .arg(TIMEOUT_WINDOW_MS);
        m_excludedDevices.insert(serverAddress);
        m_timeoutHistory.remove(serverAddress); // Освобождаем память

        // Уведомляем главный поток
        emit deviceExcludedFromPolling(serverAddress, msg);
    }
}

void ModbusManager::resetDeviceExclusion(quint8 serverAddress) {
    m_excludedDevices.remove(serverAddress);
    m_timeoutHistory.remove(serverAddress);
}

void ModbusManager::clearAllExclusions() {
    m_excludedDevices.clear();
    m_timeoutHistory.clear();
}
