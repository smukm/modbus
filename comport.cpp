#include "comport.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QIODevice>

Comport::Comport(const QString& portName, QObject *parent)
    : QObject(parent),
    m_serialPort(new QSerialPort(this))
{
    m_portSettings.portName = portName;
    m_portSettings.baudRate = QSerialPort::Baud9600;
    m_portSettings.dataBits = QSerialPort::Data8;
    m_portSettings.parity = QSerialPort::NoParity;
    m_portSettings.stopBits = QSerialPort::OneStop;
    m_portSettings.flowControl = QSerialPort::NoFlowControl;

    // Инициализация настроек порта
    initPort();

    // Подключаем сигнал готовности данных к слоту чтения
    connect(m_serialPort, &QSerialPort::readyRead, this, &Comport::slotReadData);

    // Инициализация таймера для определения конца кадра Modbus
    m_rxTimer = new QTimer(this);
    m_rxTimer->setSingleShot(true); // Таймер срабатывает только один раз после запуска
    m_rxTimer->setInterval(10);     // 10 миллисекунд (безопасно для 9600 и выше)
    connect(m_rxTimer, &QTimer::timeout, this, &Comport::processRxBuffer);
}

/**
 * @brief Применяет текущие настройки из структуры m_portSettings к объекту QSerialPort.
 * Если порт уже открыт, Qt применит изменения "на лету".
 */
void Comport::initPort() {
    m_serialPort->setPortName(m_portSettings.portName);
    m_serialPort->setBaudRate(m_portSettings.baudRate);
    m_serialPort->setDataBits(m_portSettings.dataBits);
    m_serialPort->setParity(m_portSettings.parity);
    m_serialPort->setStopBits(m_portSettings.stopBits);
    m_serialPort->setFlowControl(m_portSettings.flowControl);
}

/**
 * @brief Открывает порт в режиме чтения и записи (ReadWrite).
 * @return true, если порт успешно открыт; false в случае ошибки.
 */
bool Comport::openRW() {
    bool isOpened = m_serialPort->open(QIODevice::ReadWrite);

    if (!isOpened) {
        qWarning() << "Не удалось открыть порт: " << m_serialPort->errorString();

        return false;
    }

    // Уведомляем слушателей об успешном открытии
    emit portOpened();

    return isOpened;
}

/**
 * @brief Закрывает порт, останавливает таймер приема и очищает буфер.
 */
void Comport::closePort() {
    if (m_rxTimer) {
        m_rxTimer->stop();
    }
    m_rxBuffer.clear();

    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->close();
        qDebug() << "Порт" << m_portSettings.portName << "закрыт.";

        // Уведомляем слушателей о закрытии
        emit portClosed();
    }
}

/**
 * @brief Проверяет, открыт ли порт в данный момент.
 * @return true, если порт открыт; иначе false.
 */
bool Comport::isOpen() {
    return m_serialPort->isOpen();
}


/**
 * @brief Отправляет массив байтов в открытый COM-порт.
 * @param data QByteArray с данными для отправки.
 * @return Количество успешно записанных байт, или -1 в случае ошибки.
 */
qint64 Comport::sendData(const QByteArray &data) {
    if (!isOpen()) {
        qWarning() << "Ошибка отправки: Порт не открыт!";
        return -1;
    }

    if (data.isEmpty()) {
        qWarning() << "Ошибка отправки: Пустые данные!";
        return 0;
    }

    // Записываем данные в порт
    qint64 bytesWritten = m_serialPort->write(data);

    if (bytesWritten == -1) {
        qWarning() << "Ошибка записи в порт:" << m_serialPort->errorString();
        return -1;
    }

    // Принудительно сбрасываем буфер записи ОС, чтобы данные ушли в порт немедленно
    m_serialPort->flush();

    return bytesWritten;
}

/**
 * @brief Рассчитывает контрольную сумму CRC16 по алгоритму Modbus.
 * @param data Массив данных, для которых нужно рассчитать CRC (без самих байтов CRC).
 * @return Вычисленное 16-битное значение CRC.
 */
quint16 Comport::calculateModbusCRC16(const QByteArray &data) {
    quint16 crc = 0xFFFF; // Начальное значение регистра CRC
    for (int pos = 0; pos < data.size(); pos++) {
        crc ^= (quint8)data[pos]; // XOR с очередным байтом данных

        // 8 сдвигов для каждого бита в байте
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001; // Полином для Modbus CRC16
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Слот, вызываемый при наличии новых данных в порту (сигнал readyRead).
 * Накапливает данные в буфере и перезапускает таймер тишины.
 */
void Comport::slotReadData() {
    if (!m_serialPort || !m_serialPort->isOpen()) {
        return;
    }

    // Читаем все доступные байты и добавляем их в буфер
    QByteArray newData = m_serialPort->readAll();
    if (!newData.isEmpty()) {
        m_rxBuffer.append(newData);

        // Перезапускаем таймер.
        // Если новые данные придут раньше, чем через 10 мс, таймер сбросится и начнет отсчет заново.
        // Это позволяет корректно собрать кадр, даже если он пришел частями.
        m_rxTimer->start();
    }
}

/**
 * @brief Слот, вызываемый по таймауту таймера m_rxTimer.
 * Означает, что передача данных закончилась и кадр Modbus RTU, вероятно, завершен.
 * Проверяет целостность кадра с помощью CRC и отправляет его дальше.
 */
void Comport::processRxBuffer() {
    // Минимальная длина кадра Modbus RTU: 1 байт адреса + 1 байт функции + 2 байта CRC = 4 байта
    if (m_rxBuffer.size() < 4) {
        qDebug() << "Modbus: Получен слишком короткий кадр, очистка буфера.";
        m_rxBuffer.clear();
        return;
    }

    // Извлекаем полученную CRC из последних двух байтов.
    // В протоколе Modbus CRC передается в формате Little Endian (младший байт первый).
    quint8 crcLow = static_cast<quint8>(m_rxBuffer[m_rxBuffer.size() - 2]);
    quint8 crcHigh = static_cast<quint8>(m_rxBuffer[m_rxBuffer.size() - 1]);
    quint16 receivedCrc = (static_cast<quint16>(crcHigh) << 8) | crcLow;

    // Вычисляем CRC для всех данных, кроме последних двух байтов
    QByteArray payload = m_rxBuffer.left(m_rxBuffer.size() - 2);
    quint16 calculatedCrc = calculateModbusCRC16(payload);

    // Сравниваем полученную и вычисленную контрольные суммы
    if (receivedCrc == calculatedCrc) {
        qDebug() << "Modbus: Успешно принят валидный кадр:" << m_rxBuffer.toHex(' ').toUpper();

        // Отправляем готовый и проверенный кадр в основной поток программы
        emit modbusFrameReceived(m_rxBuffer);

        // Очищаем буфер для следующего кадра
        m_rxBuffer.clear();
    } else {
        qDebug() << "Modbus: Ошибка CRC! Получено:" << QString::number(receivedCrc, 16).toUpper()
                 << "Вычислено:" << QString::number(calculatedCrc, 16).toUpper();
        qDebug() << "Сырые данные:" << m_rxBuffer.toHex(' ').toUpper();

        // В случае ошибки очищаем буфер, чтобы рассинхронизация не ломала следующие кадры
        // (В продвинутых реализациях здесь пытаются найти границу следующего кадра)
        m_rxBuffer.clear();
    }
}

/**
 * @brief Получает список всех доступных в системе последовательных портов.
 * Также выводит отладочную информацию о каждом порте.
 * @return Список объектов QSerialPortInfo.
 */
QList<QSerialPortInfo> Comport::getAvailablePorts() {

    qDebug() << "--- Доступные COM-порты ---";
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        qDebug() << "Имя порта:" << info.portName();
        qDebug() << "Описание:" << info.description();
        qDebug() << "Производитель:" << info.manufacturer();
        qDebug() << "-------------------------";
    }

    return QSerialPortInfo::availablePorts();
}

/**
 * @brief Обновляет структуру настроек порта и применяет их.
 * @param settings Новая структура PortSettings.
 */
void Comport::setPortSettings(const PortSettings& settings) {
    m_portSettings = settings;
    // Применяем новые настройки к объекту порта.
    // Если порт открыт, Qt применит их немедленно.
    initPort();
}

Comport::~Comport() {
    if (m_serialPort) {
        closePort(); // Гарантируем, что порт закрыт и таймер остановлен
    }
}