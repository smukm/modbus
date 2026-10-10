#ifndef MODBUSTCPMANAGER_H
#define MODBUSTCPMANAGER_H

#include "abstractmodbusmanager.h"
#include <QModbusTcpClient>

class ModbusTcpManager : public AbstractModbusManager
{
    Q_OBJECT
public:
    explicit ModbusTcpManager(QObject *parent = nullptr);

    void connectToDevice(const ModbusConnectionSettings &settings) override;
    bool isConnected() const override;
};

#endif // MODBUSTCPMANAGER_H
