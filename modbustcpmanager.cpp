#include "modbustcpmanager.h"

ModbusTcpManager::ModbusTcpManager(QObject *parent)
    : AbstractModbusManager(parent)
{
}

void ModbusTcpManager::connectToDevice(const ModbusConnectionSettings &settings) {
    if (m_modbusDevice) {
        disconnectFromDevice();
    }

    m_modbusDevice = new QModbusTcpClient(this);

    m_modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, settings.ipAddress);
    m_modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, static_cast<int>(settings.port));

    connectDeviceSignals();

    // Для TCP количество повторов лучше ставить 0 или 1.
    // TCP-стек сам гарантирует доставку, а лишние модбас-ретраи заблокируют очередь при обрыве сети.
    m_modbusDevice->setTimeout(1000);
    m_modbusDevice->setNumberOfRetries(1);

    if (!m_modbusDevice->connectDevice()) {
        emit errorCriticalOccurred(tr("Failed to initiate TCP connection: ") + m_modbusDevice->errorString());
    }
}

bool ModbusTcpManager::isConnected() const {
    return m_modbusDevice && m_modbusDevice->state() == QModbusDevice::ConnectedState;
}