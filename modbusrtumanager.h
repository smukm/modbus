#ifndef MODBUSRTUMANAGER_H
#define MODBUSRTUMANAGER_H

#include "abstractmodbusmanager.h"
#include <QModbusRtuSerialClient>

class ModbusRtuManager : public AbstractModbusManager
{
    Q_OBJECT
public:
    explicit ModbusRtuManager(QObject *parent = nullptr);

    void connectToDevice(const ModbusConnectionSettings &settings) override;
    bool isConnected() const override;
};

#endif // MODBUSRTUMANAGER_H
