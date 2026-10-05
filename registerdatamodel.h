#ifndef REGISTERDATAMODEL_H
#define REGISTERDATAMODEL_H
#include <QStandardItemModel>
#include <QHash>
#include <QPair>
#include <QModbusDataUnit>

class RegisterDataModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit RegisterDataModel(QObject* parent = nullptr);

    // Главный метод: принимает сырые данные и сам решает, обновить строку или создать новую
    void updateData(const QModbusDataUnit &unit);

private:
    // Кэш для быстрого поиска: {адрес, тип} -> номер строки
    QHash<QPair<int, QString>, int> m_addressMap;

    QString getRegisterTypeName(QModbusDataUnit::RegisterType type) const;
};

#endif // REGISTERDATAMODEL_H
