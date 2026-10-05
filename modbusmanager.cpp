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
{
    // Создаем экземпляр RTU-клиента. Родителем является сам ModbusManager,
    // что гарантирует автоматическое удаление при уничтожении менеджера.
    m_modbusDevice = new QModbusRtuSerialClient(this);

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
    if (m_modbusDevice) m_modbusDevice->disconnectDevice();
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

    if (m_currentReply) {
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    QModbusDataUnit::RegisterType type;
    switch (funcCode) {
    case 0x01: type = QModbusDataUnit::Coils; break;
    case 0x02: type = QModbusDataUnit::DiscreteInputs; break;
    case 0x03: type = QModbusDataUnit::HoldingRegisters; break;
    case 0x04: type = QModbusDataUnit::InputRegisters; break;
    default: emit errorOccurred("Неподдерживаемый код функции для чтения"); return;
    }

    // Формируем единицу данных (Data Unit) для запроса
    QModbusDataUnit request(type, startAddress, count);
    // Асинхронная отправка запроса. Возвращает объект QModbusReply для отслеживания результата.
    m_currentReply = m_modbusDevice->sendReadRequest(request, serverAddress);

    if (m_currentReply) {
        // Если ответ еще не получен (стандартное асинхронное поведение), ждем сигнал finished
        if (!m_currentReply->isFinished()) {
            connect(m_currentReply, &QModbusReply::finished, this, &ModbusManager::onReplyFinished);
        } else {
            // Редкий случай: ответ пришел синхронно (мгновенно), обрабатываем сразу
            onReplyFinished();
        }
    } else {
        // Ошибка на этапе формирования или постановки запроса в очередь
        emit errorCriticalOccured(m_modbusDevice->errorString());
    }
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

    if (m_currentReply) {
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    QModbusDataUnit::RegisterType type;
    quint16 count = static_cast<quint16>(values.size());
    if (funcCode == 0x05 || funcCode == 0x0F) {
        type = QModbusDataUnit::Coils;
    } else if (funcCode == 0x06 || funcCode == 0x10) {
        type = QModbusDataUnit::HoldingRegisters;
    } else {
        emit errorOccurred("Неподдерживаемый код функции для записи (поддерживаются 0x05, 0x06, 0x0F, 0x10)");
        return;
    }

    // Формируем запрос. count берется из размера переданного вектора
    QModbusDataUnit request(type, startAddress, count);
    for (int i = 0; i < count; ++i) {
        request.setValue(i, values[i]);
    }

    m_currentReply = m_modbusDevice->sendWriteRequest(request, serverAddress);

    if (m_currentReply) {
        if (!m_currentReply->isFinished()) {
            connect(m_currentReply, &QModbusReply::finished, this, &ModbusManager::onReplyFinished);
        } else {
            onReplyFinished();
        }
    } else {
        emit errorCriticalOccured(m_modbusDevice->errorString());
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
        const QModbusDataUnit unit = m_currentReply->result();

        emit dataReceived(unit);
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
}