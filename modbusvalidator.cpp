#include "modbusvalidator.h"

ValidationResult ModbusValidator::validateBasicParams(quint8 deviceAddr, quint8 funcCode, quint16 startAddr, quint16 count) {
    ValidationResult result;
    result.isValid = true;
    result.errorMessage = "";

    // Проверка кода функции
    bool isRead = (funcCode == 0x01 || funcCode == 0x02 || funcCode == 0x03 || funcCode == 0x04);
    bool isWrite = (funcCode == 0x05 || funcCode == 0x06 || funcCode == 0x0F || funcCode == 0x10);

    if (!isRead && !isWrite) {
        result.isValid = false;
        result.errorMessage = QString("Неподдерживаемый код функции: 0x%1.\nПоддерживаются: 0x01-0x04 (чтение) и 0x05, 0x06, 0x0F, 0x10 (запись).")
                                  .arg(funcCode, 2, 16, QChar('0')).toUpper();
        return result;
    }

    // Проверка диапазонов адресов и количества
    // (deviceAddr уже ограничен типом quint8 (0-255), но можно добавить проверку на широковещательный адрес, если нужно)

    if (startAddr > 65535) {
        result.isValid = false;
        result.errorMessage = "Некорректный адрес регистра (должен быть в диапазоне 0-65535).";
        return result;
    }

    if (count < 1 || count > 65535) {
        result.isValid = false;
        result.errorMessage = "Некорректное количество регистров (должно быть в диапазоне 1-65535).";
        return result;
    }

    return result;
}

ValidationResult ModbusValidator::validateWriteData(quint8 funcCode, const QString &dataText, QVector<quint16> &outValues) {
    ValidationResult result;
    result.isValid = true;
    result.errorMessage = "";

    // Очищаем выходной вектор перед заполнением
    outValues.clear();

    if (dataText.trimmed().isEmpty()) {
        result.isValid = false;
        result.errorMessage = "Введите значение для записи!";
        return result;
    }

    const QStringList parts = dataText.split(',', Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        result.isValid = false;
        result.errorMessage = "Некорректный формат данных.";
        return result;
    }

    for (const QString &part : parts) {
        bool valOk = false;
        // base = 0 автоматически распознает "0xFF" как HEX, "0" как OCT, остальные как DEC
        int val = part.trimmed().toInt(&valOk, 0);

        if (!valOk || val < 0 || val > 0xFFFF) {
            result.isValid = false;
            result.errorMessage = QString("Некорректное значение для записи: '%1'\nОжидается число от 0 до 65535 (или 0x0000-0xFFFF).").arg(part);
            return result;
        }

        // Специальная проверка для дискретных выходов (COILS)
        if (funcCode == 0x05 || funcCode == 0x0F) {
            if (val != 0 && val != 1 && val != 0xFF00 && val != 65280) {
                result.isValid = false;
                result.errorMessage = QString("Для функций 0x05/0x0F (Coils) допустимы только значения:\n"
                                              "0 (или 0x00) — для выключения (OFF)\n"
                                              "65280 (или 0xFF00) — для включения (ON)\n"
                                              "Вы ввели: %1").arg(val);
                return result;
            }
            // Приводим 1 к стандартному 0xFF00
            if (val == 1) {
                val = 0xFF00;
            }
        }

        outValues.append(static_cast<quint16>(val));

    }

    return result;
}