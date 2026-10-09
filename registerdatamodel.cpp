#include "registerdatamodel.h"
#include <QFont>
#include <QColor>
#include <QBrush>

// Определяем кастомную роль для хранения исходного цвета
constexpr int OriginalColorRole = Qt::UserRole + 1;

RegisterDataModel::RegisterDataModel(QObject* parent) : QStandardItemModel(parent) {
    setColumnCount(4);
    setHorizontalHeaderLabels({
        tr("SlaveId"),
        tr("Register Address"),
        tr("Value"),
        tr("Register Type")
    });
}

void RegisterDataModel::updateData(quint8 serverAddress, const QModbusDataUnit &unit) {
    for (uint i = 0; i < unit.valueCount(); ++i) {
        int regAddress = unit.startAddress() + static_cast<int>(i);
        quint16 regValue = unit.value(i);
        QString valueStr = QString("%1 (0x%2)")
                               .arg(regValue)
                               .arg(regValue, 4, 16, QChar('0')).toUpper();

        QString regType = getRegisterTypeName(unit.registerType());
        QString key = QString("%1_%2_%3").arg(serverAddress).arg(regAddress).arg(regType);
        auto it = m_addressMap.find(key);

        if (it != m_addressMap.end()) {
            QStandardItem* valueItem = item(it.value(), 2);
            if (valueItem) {
                // Обновляем значение
                valueItem->setText(valueStr);

                // Проверяем, находится ли строка в состоянии "офлайн"
                QStandardItem* slaveIdItem = item(it.value(), 0);
                bool isOffline = slaveIdItem && slaveIdItem->text().contains(" [OFFLINE]");

                // Восстанавливаем вид только если строка была в офлайне
                if (isOffline) {
                    for (int col = 0; col < columnCount(); ++col) {
                        QStandardItem* colItem = item(it.value(), col);
                        if (colItem) {
                            // Восстанавливаем исходное состояние цвета из кэша
                            QVariant originalFg = colItem->data(OriginalColorRole);

                            // Если originalFg не валиден, Qt автоматически использует цвет текста по умолчанию для текущей темы!
                            colItem->setData(originalFg, Qt::ForegroundRole);

                            // Очищаем кэш
                            colItem->setData(QVariant(), OriginalColorRole);

                            // Убираем курсив
                            QFont font = colItem->font();
                            font.setItalic(false);
                            colItem->setFont(font);

                            // Убираем текстовый маркер
                            if (col == 0) {
                                QString text = colItem->text();
                                text.replace(" [OFFLINE]", "");
                                colItem->setText(text);
                            }
                        }
                    }
                }
            }
        } else {
            // Добавляем новую строку
            int newRow = rowCount();
            QString slaveId = QString::number(serverAddress);
            QString regAddressStr = QString("%1 (0x%2)")
                                        .arg(regAddress)
                                        .arg(regAddress, 4, 16, QChar('0')).toUpper();

            QStandardItem* slaveIdItem = new QStandardItem(slaveId);
            QStandardItem* addrItem = new QStandardItem(regAddressStr);
            addrItem->setData(regAddress, Qt::UserRole);

            QStandardItem* valueItem = new QStandardItem(valueStr);
            QStandardItem* typeItem = new QStandardItem(regType);

            appendRow({slaveIdItem, addrItem, valueItem, typeItem});
            m_addressMap.insert(key, newRow);
        }
    }
}

void RegisterDataModel::setExcludeDevice(quint8 serverAddress) {
    QString targetSlaveId = QString::number(serverAddress);

    for (int row = 0; row < rowCount(); ++row) {
        QStandardItem* slaveIdItem = item(row, 0);
        if (slaveIdItem && slaveIdItem->text().startsWith(targetSlaveId)) {
            for (int col = 0; col < columnCount(); ++col) {
                QStandardItem* colItem = item(row, col);
                if (colItem) {
                    QVariant originalFg = colItem->data(Qt::ForegroundRole);
                    colItem->setData(originalFg, OriginalColorRole);

                    // Применяем стиль "офлайн"
                    colItem->setForeground(Qt::darkGray);

                    QFont font = colItem->font();
                    font.setItalic(true);
                    colItem->setFont(font);

                    if (col == 0) {
                        QString currentText = slaveIdItem->text();
                        if (!currentText.contains(" [OFFLINE]")) {
                            slaveIdItem->setText(currentText + " [OFFLINE]");
                        }
                    }
                }
            }
        }
    }
}

void RegisterDataModel::resetExcludeDevice(quint8 serverAddress) {
    QString targetSlaveId = QString::number(serverAddress);

    for (int row = 0; row < rowCount(); ++row) {
        QStandardItem* slaveIdItem = item(row, 0);
        if (slaveIdItem && slaveIdItem->text().startsWith(targetSlaveId) && slaveIdItem->text().contains(" [OFFLINE]")) {
            for (int col = 0; col < columnCount(); ++col) {
                QStandardItem* colItem = item(row, col);
                if (colItem) {
                    // Восстанавливаем исходное состояние (валидное или нет)
                    QVariant originalFg = colItem->data(OriginalColorRole);
                    colItem->setData(originalFg, Qt::ForegroundRole);
                    colItem->setData(QVariant(), OriginalColorRole);

                    QFont font = colItem->font();
                    font.setItalic(false);
                    colItem->setFont(font);

                    if (col == 0) {
                        QString text = slaveIdItem->text();
                        text.replace(" [OFFLINE]", "");
                        slaveIdItem->setText(text);
                    }
                }
            }
        }
    }
}

void RegisterDataModel::clearAllExclusions() {
    for (int row = 0; row < rowCount(); ++row) {
        QStandardItem* slaveIdItem = item(row, 0);
        if (slaveIdItem && slaveIdItem->text().contains(" [OFFLINE]")) {
            for (int col = 0; col < columnCount(); ++col) {
                QStandardItem* colItem = item(row, col);
                if (colItem) {
                    // Восстанавливаем исходное состояние (валидное или нет)
                    QVariant originalFg = colItem->data(OriginalColorRole);
                    colItem->setData(originalFg, Qt::ForegroundRole);
                    colItem->setData(QVariant(), OriginalColorRole);

                    QFont font = colItem->font();
                    font.setItalic(false);
                    colItem->setFont(font);

                    if (col == 0) {
                        QString text = slaveIdItem->text();
                        text.replace(" [OFFLINE]", "");
                        slaveIdItem->setText(text);
                    }
                }
            }
        }
    }
}

QString RegisterDataModel::getRegisterTypeName(QModbusDataUnit::RegisterType type) const {
    QString regType;
    switch (type) {
    case QModbusDataUnit::RegisterType::HoldingRegisters:
        regType = tr("Holding Registers");
        break;
    case QModbusDataUnit::RegisterType::InputRegisters:
        regType = tr("Input Registers");
        break;
    case QModbusDataUnit::RegisterType::DiscreteInputs:
        regType = tr("Discrete Inputs");
        break;
    case QModbusDataUnit::RegisterType::Coils:
        regType = tr("Coils");
        break;
    default:
        regType = tr("Unknown type");
        break;
    }

    return regType;
}