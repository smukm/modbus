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
 * @param funcCode Код функции Modbus (0x05 или 0x06).
 * @param startAddress Адрес регистра/коилла для записи.
 * @param value Значение, которое необходимо записать.
 */
void ModbusManager::sendWriteRequest(quint8 serverAddress, quint8 funcCode, quint16 startAddress, quint16 value) {
    if (!isConnected()) {
        emit errorOccurred("Порт не открыт!");
        return;
    }

    QModbusDataUnit::RegisterType type;
    switch (funcCode) {
    case 0x05: type = QModbusDataUnit::Coils; break;             // Запись одного дискретного выхода (Coil)
    case 0x06: type = QModbusDataUnit::HoldingRegisters; break;  // Запись одного регистра хранения
    // Примечание: функции 0x0F и 0x10 (запись нескольких) требуют передачи массива значений,
    // в рамках простого UI мы их пока не поддерживаем.
    default:
        emit errorOccurred("Неподдерживаемый код функции для записи (поддерживаются только 0x05 и 0x06)");
        return;
    }

    // Для записи одного значения количество (valueCount) всегда равно 1
    QModbusDataUnit request(type, startAddress, 1);
    request.setValue(0, value); // Записываем значение в нулевой (и единственный) элемент

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
        emit errorOccurred(
            "Устройство отвергло запрос (Modbus Exception).\n"
            "Проверьте:\n"
            "1. Адрес устройства (Slave ID)\n"
            "2. Код функции (0x03 или 0x04?)\n"
            "3. Адрес регистра (в Modbus он начинается с 0, а не с 40001)"
            );
    } else {
        // Другие ошибки (например, таймаут ожидания ответа или физический обрыв линии)
        emit errorOccurred("Ошибка ответа Modbus:" + m_currentReply->errorString());
    }

    // Обязательно удаляем объект ответа, чтобы избежать утечки памяти
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}