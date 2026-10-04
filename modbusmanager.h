#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QObject>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>

struct ModbusConnectionSettings {
    QString portName;
    int baudRate;
    int parity;
    int dataBits;
    int stopBits;
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

private slots:
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    void onReplyFinished();

private:
    QModbusClient *m_modbusDevice;
    QModbusReply *m_currentReply = nullptr;
};

#endif // MODBUSMANAGER_H
