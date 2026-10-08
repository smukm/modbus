#include "registerdatamodel.h"

RegisterDataModel::RegisterDataModel(QObject* parent) : QStandardItemModel(parent) {
    setColumnCount(4);
    setHorizontalHeaderLabels({
        "SlaveId",
        "Адрес регистра",
        "Значение",
        "Тип регистра"
    });
}


void RegisterDataModel::updateData(quint8 serverAddress, const QModbusDataUnit &unit) {
    for (uint i = 0; i < unit.valueCount(); ++i) {
        int regAddress = unit.startAddress() + static_cast<int>(i);
        quint16 regValue = unit.value(i);
        QString valueStr = QString("%1 (0x%2)")
                               .arg(regValue)
                               .arg(regValue, 4, 16, QChar('0')).toUpper();

        // Ищем существующую строку с тем же адресом и типом
        QString regType = getRegisterTypeName(unit.registerType());

        QString key = QString("%1_%2_%3").arg(serverAddress).arg(regAddress).arg(regType);
        auto it = m_addressMap.find(key);

        if (it != m_addressMap.end()) {
            // Строка найдена — обновляем только значение (столбец 2)
            item(it.value(), 2)->setText(valueStr);
        } else {
            // Добавляем новую строку
            int newRow = rowCount();
            QString slaveId = QString("%1").arg(serverAddress);
            QString regAddressStr = QString("%1 (0x%2)")
                                        .arg(regAddress)
                                        .arg(regAddress, 4, 16, QChar('0')).toUpper();

            QStandardItem* slaveIdItem = new QStandardItem(slaveId);

            QStandardItem* addrItem = new QStandardItem(regAddressStr);
            addrItem->setData(regAddress, Qt::UserRole); // Сохраняем адрес как данные

            QStandardItem* valueItem = new QStandardItem(valueStr);
            QStandardItem* typeItem = new QStandardItem(regType);

            appendRow({slaveIdItem, addrItem, valueItem, typeItem});
            m_addressMap.insert(key, newRow); // Кэшируем
        }
    }
}

QString RegisterDataModel::getRegisterTypeName(QModbusDataUnit::RegisterType type) const {
    QString regType;
    switch (type) {
    case QModbusDataUnit::RegisterType::HoldingRegisters:
        regType = "Holding Registers";
        break;
    case QModbusDataUnit::RegisterType::InputRegisters:
        regType = "Input Registers";
        break;
    case QModbusDataUnit::RegisterType::DiscreteInputs:
        regType = "Discrete Inputs";
        break;
    case QModbusDataUnit::RegisterType::Coils:
        regType = "Coils";
        break;
    default:
        regType = "Unknown type";
        break;
    }

    return regType;
}