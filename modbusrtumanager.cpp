#include "modbusrtumanager.h"

ModbusRtuManager::ModbusRtuManager(QObject *parent)
    : AbstractModbusManager(parent)
{
}

void ModbusRtuManager::connectToDevice(const ModbusConnectionSettings &settings) {
    if (m_modbusDevice) {
        disconnectFromDevice();
    }

    m_modbusDevice = new QModbusRtuSerialClient(this);

    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, settings.portName);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, settings.baudRate);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, settings.parity);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, settings.dataBits);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, settings.stopBits);

    connectDeviceSignals();

    // Для Serial-соединений допустимо больше попыток из-за шумов в линии
    m_modbusDevice->setTimeout(1000);
    m_modbusDevice->setNumberOfRetries(3);

    if (!m_modbusDevice->connectDevice()) {
        emit errorCriticalOccurred(tr("Failed to initiate Serial connection: ") + m_modbusDevice->errorString());
    }
}


bool ModbusRtuManager::isConnected() const {
    return m_modbusDevice && m_modbusDevice->state() == QModbusDevice::ConnectedState;
}