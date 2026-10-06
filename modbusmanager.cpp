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
    // Создаем экземпляр RTU-клиента. Родителем является сам ModbusManager,
    // что гарантирует автоматическое удаление при уничтожении менеджера.
    //m_modbusDevice = new QModbusRtuSerialClient(this);

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
        emit errorCriticalOccured("Не удалось начать подключение: " + m_modbusDevice->errorString());
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
    if (m_currentReply) {
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
        emit errorOccurred("Порт не открыт!");
        return;
    };

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
        emit errorOccurred("Порт не открыт!");
        return;
    }

    if (values.isEmpty()) {
        emit errorOccurred("Нет данных для записи!");
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
            emit errorOccurred("Неподдерживаемый код функции для чтения");
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
            emit errorOccurred("Неподдерживаемый код функции для записи (поддерживаются 0x05, 0x06, 0x0F, 0x10)");
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
        emit errorCriticalOccured(m_modbusDevice->errorString());
        m_isProcessing = false;
        processNextRequest(); // Продолжаем обработку очереди даже при ошибке отправки
    }
}

/**
 * @brief Слот-обработчик изменения состояния подключения.
 * Транслирует внутренние состояния QModbusDevice в понятные сигналы для UI.
 */
void ModbusManager::onStateChanged(QModbusDevice::State state) {
    switch (state) {
    case QModbusDevice::UnconnectedState:
        qDebug() << "ModbusManager: Устройство отключено.";
        emit disconnected();
        break;

    case QModbusDevice::ConnectedState:
        qDebug() << "ModbusManager: Устройство успешно подключено.";
        emit connected();
        break;

    case QModbusDevice::ConnectingState:
        qDebug() << "ModbusManager: Попытка подключения...";
        // Сигнал не отправляем, так как процесс еще идет
        break;

    case QModbusDevice::ClosingState:
        qDebug() << "ModbusManager: Закрытие соединения...";
        // Сигнал не отправляем, дождемся UnconnectedState
        break;

    default:
        qWarning() << "ModbusManager: Неизвестное состояние устройства.";
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
            emit dataReceived(unit);
        } else {
            emit writeCompleted();
        }
    } else if (m_currentReply->error() == QModbusDevice::ProtocolError) {
        // Специфическая ошибка: устройство ответило, но вернуло Modbus Exception (исключение).
        // Это означает, что запрос был получен, но отвергнут устройством (неверный адрес, функция и т.д.).
        QString exceptionInfo = "Неизвестная ошибка";
        if (m_currentReply->rawResult().exceptionCode() == 0x01) {
            exceptionInfo = "Illegal Function (Функция не поддерживается устройством)";
        } else if (m_currentReply->rawResult().exceptionCode() == 0x02) {
            exceptionInfo = "Illegal Data Address (Такого адреса регистра/катушки не существует или он недоступен)";
        } else if (m_currentReply->rawResult().exceptionCode() == 0x03) {
            exceptionInfo = "Illegal Data Value (Значение выходит за допустимые пределы)";
        } else if (m_currentReply->rawResult().exceptionCode() == 0x04) {
            exceptionInfo = "Slave Device Failure (Внутренняя ошибка устройства)";
        } else if (m_currentReply->rawResult().exceptionCode() == 0x06) {
            exceptionInfo = "Slave Device Busy (Устройство занято)";
        }

        emit errorOccurred(
            QString("Устройство отвергло запрос (Modbus Exception).\n"
                    "Код ошибки: %1\n"
                    "Расшифровка: %2\n\n"
                    "Что проверить:\n"
                    "1. Точно ли этот адрес существует в паспорте устройства?\n"
                    "2. Не перепутали ли вы функцию (0x05 для катушек, 0x06 для числовых регистров)?")
                .arg(m_currentReply->rawResult().exceptionCode())
                .arg(exceptionInfo)
            );
    } else {
        // Другие ошибки (например, таймаут ожидания ответа или физический обрыв линии)
        emit errorOccurred("Ошибка ответа Modbus:" + m_currentReply->errorString());
    }

    // Обязательно удаляем объект ответа, чтобы избежать утечки памяти
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
    m_isProcessing = false;

    // Запускаем обработку следующего запроса из очереди
    processNextRequest();
}