#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QTimer>

struct PortSettings {
    QString portName;
    QSerialPort::BaudRate baudRate; // Скорость
    QSerialPort::DataBits dataBits; // Биты данных
    QSerialPort::Parity parity; // Четность
    QSerialPort::StopBits stopBits; // Стоп-биты
    QSerialPort::FlowControl flowControl; // Управление потоком
};


class Comport : public QObject
{
    Q_OBJECT

private:
    QSerialPort *m_serialPort;
    PortSettings m_portSettings;
    QByteArray m_rxBuffer;
    QTimer *m_rxTimer;
    void initPort();

public:
    explicit Comport(const QString& portName = "COM1", QObject *parent = nullptr);
    ~Comport();

    bool openRW();
    bool isOpen();
    void closePort();
    static QList<QSerialPortInfo> getAvailablePorts();
    void setPortSettings(const PortSettings& settings);
    qint64 sendData(const QByteArray &data);
    quint16 calculateModbusCRC16(const QByteArray &data);
signals:
    void portOpened();
    void portClosed();
    void dataReceived(const QByteArray &data);
    void modbusFrameReceived(const QByteArray &frame); // Сигнал для готового кадра Modbus
private slots:
    void slotReadData();
    void processRxBuffer(); // Слот для обработки по таймауту
};

#endif // COMPORT_H
