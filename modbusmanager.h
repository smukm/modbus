#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QObject>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QQueue>
#include <QVector>

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

    // Методы для отправки запросов (скрывают сложность QModbusDataUnit)
    void sendReadRequest(quint8 serverAddress, quint8 funcCode, quint16 startAddress, quint16 count);
    void sendWriteRequest(quint8 serverAddress, quint8 funcCode, quint16 startAddress, const QVector<quint16> &values);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &errorString);
    void errorCriticalOccured(const QString &errorString);
    void dataReceived(const QModbusDataUnit &data);
    void writeCompleted(); // Новый сигнал для уведомления об успешной записи

private slots:
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    void onReplyFinished();

private:
    void processNextRequest(); // Метод для извлечения и отправки следующего запроса из очереди

    QModbusClient *m_modbusDevice;
    QModbusReply *m_currentReply = nullptr;

    QQueue<ModbusRequest> m_requestQueue; // Очередь запросов
    ModbusRequest m_currentRequest;       // Текущий выполняемый запрос
    bool m_isProcessing;                  // Флаг, указывающий, что запрос уже выполняется
};

#endif // MODBUSMANAGER_H
