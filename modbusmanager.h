#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QObject>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QQueue>
#include <QVector>
#include <QHash>
#include <QSet>
#include <QDateTime>

struct ModbusConnectionSettings {
    QString portName;
    int baudRate;
    int parity;
    int dataBits;
    int stopBits;
};

// Структура для хранения параметров одного запроса в очереди
struct ModbusRequest {
    enum class Type { Read, Write };
    Type type;
    quint8 serverAddress;
    quint8 funcCode;
    quint16 startAddress;
    quint16 count;
    QVector<quint16> values; // Используется только для записи
};

class ModbusManager : public QObject
{
    Q_OBJECT
public:
    explicit ModbusManager(QObject *parent = nullptr);
    ~ModbusManager();

    void connectToDevice(const ModbusConnectionSettings &settings);
    void disconnectFromDevice();
    bool isConnected() const;
    bool isProcessing() const { return m_isProcessing; }

    // Методы для отправки запросов
    void sendReadRequest(quint8 serverAddress, quint8 funcCode, quint16 startAddress, quint16 count);
    void sendWriteRequest(quint8 serverAddress, quint8 funcCode, quint16 startAddress, const QVector<quint16> &values);

    // Принудительный сброс статуса исключения (например, при ручном перезапуске)
    void resetDeviceExclusion(quint8 serverAddress);
    void clearAllExclusions();

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &errorString);
    void errorCriticalOccurred(const QString &errorString);
    void dataReceived(quint8 serverAddress, const QModbusDataUnit &data);
    void writeCompleted();
    // Уведомление UI о том, что устройство нужно исключить из опроса
    void deviceExcludedFromPolling(quint8 serverAddress, const QString& msg);

private slots:
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    void onReplyFinished();

private:
    void processNextRequest(); // Метод для извлечения и отправки следующего запроса из очереди
    void recordTimeout(quint8 serverAddress);

    QModbusClient *m_modbusDevice;
    QModbusReply *m_currentReply = nullptr;

    QQueue<ModbusRequest> m_requestQueue; // Очередь запросов
    ModbusRequest m_currentRequest;       // Текущий выполняемый запрос
    bool m_isProcessing;                  // Флаг, указывающий, что запрос уже выполняется

    QHash<quint8, QQueue<QDateTime>> m_timeoutHistory; // История таймаутов по адресам
    QSet<quint8> m_excludedDevices;                    // "Черный список" устройств

    static constexpr int MAX_TIMEOUTS = 3;             // Максимум таймаутов
    static constexpr int TIMEOUT_WINDOW_MS = 30000;    // Временное окно в мс (30 секунд)

};

#endif // MODBUSMANAGER_H
