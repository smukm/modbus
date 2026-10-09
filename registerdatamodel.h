#ifndef REGISTERDATAMODEL_H
#define REGISTERDATAMODEL_H

#include <QStandardItemModel>
#include <QHash>
#include <QModbusDataUnit>

class RegisterDataModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit RegisterDataModel(QObject* parent = nullptr);

    // Главный метод: принимает сырые данные и сам решает, обновить строку или создать новую
    void updateData(quint8 serverAddress, const QModbusDataUnit &unit);

    // Визуально помечает все строки устройства как исключенные (офлайн)
    void setExcludeDevice(quint8 serverAddress);

    // Сбрасывает визуальные пометки (призводит строки в нормальный вид)
    void resetExcludeDevice(quint8 serverAddress);

    // Сбрасывает пометки для всех устройств
    void clearAllExclusions();

private:
    // Кэш для быстрого поиска: {адрес, тип} -> номер строки
    QHash<QString, int> m_addressMap;

    QString getRegisterTypeName(QModbusDataUnit::RegisterType type) const;
};

#endif // REGISTERDATAMODEL_H
