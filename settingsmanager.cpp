#include "settingsmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QDebug>
#include <QObject>

// Имя файла конфигурации
static const QString CONFIG_FILE_NAME = "modbus_settings.json";

// Ключи верхнего уровня (ветки)
static const QString KEY_PORT_SETTINGS  = "portSettings";
static const QString KEY_READ_SETTINGS  = "readSettings";
static const QString KEY_WRITE_SETTINGS = "writeSettings";

// Ключи внутри ветки portSettings
static const QString KEY_PORT_NAME  = "portName";
static const QString KEY_BAUD_RATE  = "baudRate";
static const QString KEY_PARITY     = "parity";
static const QString KEY_DATA_BITS  = "dataBits";
static const QString KEY_STOP_BITS  = "stopBits";

// Ключи внутри ветки readSettings
static const QString KEY_READ_DEV_ADDR   = "deviceAddr";
static const QString KEY_READ_FUNC_CODE  = "funcCode";
static const QString KEY_READ_START_ADDR = "startAddr";
static const QString KEY_READ_COUNT      = "count";

// Ключи внутри ветки writeSettings
static const QString KEY_WRITE_DEV_ADDR   = "deviceAddr";
static const QString KEY_WRITE_FUNC_CODE  = "funcCode";
static const QString KEY_WRITE_START_ADDR = "startAddr";
static const QString KEY_WRITE_COUNT      = "count";
static const QString KEY_WRITE_DATA       = "data";


QString SettingsManager::configFilePath()
{
    // Используем стандартную папку для конфигураций приложения
    // Windows: C:/Users/<User>/AppData/Local/<AppName>/
    // Linux:   ~/.local/share/<AppName>/
    // macOS:   ~/Library/Application Support/<AppName>/
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    QDir dir(configDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    return dir.filePath(CONFIG_FILE_NAME);
}

// =========================================================================
// Вспомогательные методы для безопасной работы с ветками JSON
// =========================================================================

QJsonObject SettingsManager::loadRootObject()
{
    const QString path = configFilePath();
    QFile file(path);

    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            return doc.object();
        }
        qWarning() << QObject::tr("SettingsManager: JSON parsing error, using empty object:") << parseError.errorString();
    }
    return QJsonObject(); // Возвращаем пустой объект, если файла нет или он битый
}

bool SettingsManager::saveRootObject(const QJsonObject &rootObj)
{
    QJsonDocument doc(rootObj);
    QFile file(configFilePath());

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << QObject::tr("SettingsManager: Failed to open file for writing:") << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out << doc.toJson(QJsonDocument::Indented);
    file.close();
    return true;
}

// =========================================================================
// Методы для настроек порта
// =========================================================================

bool SettingsManager::savePortSettings(const ModbusConnectionSettings &settings)
{
    QJsonObject rootObj = loadRootObject();

    QJsonObject portObj;
    portObj[KEY_PORT_NAME] = settings.portName;
    portObj[KEY_BAUD_RATE] = settings.baudRate;
    portObj[KEY_PARITY]    = settings.parity;
    portObj[KEY_DATA_BITS] = settings.dataBits;
    portObj[KEY_STOP_BITS] = settings.stopBits;

    rootObj[KEY_PORT_SETTINGS] = portObj;

    return saveRootObject(rootObj);
}

ModbusConnectionSettings SettingsManager::loadPortSettings()
{
    ModbusConnectionSettings defaults;
    defaults.portName  = "";
    defaults.baudRate  = 9600;
    defaults.parity    = 0; // QSerialPort::NoParity
    defaults.dataBits  = 8; // QSerialPort::Data8
    defaults.stopBits  = 1; // QSerialPort::OneStop

    QJsonObject rootObj = loadRootObject();
    if (!rootObj.contains(KEY_PORT_SETTINGS)) {
        return defaults;
    }

    QJsonObject portObj = rootObj[KEY_PORT_SETTINGS].toObject();
    ModbusConnectionSettings settings;
    settings.portName = portObj[KEY_PORT_NAME].toString();
    settings.baudRate = portObj[KEY_BAUD_RATE].toInt(defaults.baudRate);
    settings.parity   = portObj[KEY_PARITY].toInt(defaults.parity);
    settings.dataBits = portObj[KEY_DATA_BITS].toInt(defaults.dataBits);
    settings.stopBits = portObj[KEY_STOP_BITS].toInt(defaults.stopBits);

    return settings;
}

bool SettingsManager::settingsExist()
{
    return QFile::exists(configFilePath());
}

// =========================================================================
// Методы для последних параметров команд
// =========================================================================

bool SettingsManager::saveLastParams(const ModbusLastParams &params)
{
    QJsonObject rootObj = loadRootObject();

    // Формируем ветку чтения
    QJsonObject readObj;
    readObj[KEY_READ_DEV_ADDR]   = params.readDeviceAddrs;
    readObj[KEY_READ_FUNC_CODE]  = static_cast<int>(params.readFuncCode);
    readObj[KEY_READ_START_ADDR] = static_cast<int>(params.readStartAddr);
    readObj[KEY_READ_COUNT]      = static_cast<int>(params.readCount);

    // Формируем ветку записи
    QJsonObject writeObj;
    writeObj[KEY_WRITE_DEV_ADDR]   = static_cast<int>(params.writeDeviceAddr);
    writeObj[KEY_WRITE_FUNC_CODE]  = static_cast<int>(params.writeFuncCode);
    writeObj[KEY_WRITE_START_ADDR] = static_cast<int>(params.writeStartAddr);
    writeObj[KEY_WRITE_COUNT]      = static_cast<int>(params.writeCount);
    writeObj[KEY_WRITE_DATA]       = params.writeData;

    // Внедряем ветки в корневой объект (это не затронет portSettings)
    rootObj[KEY_READ_SETTINGS]  = readObj;
    rootObj[KEY_WRITE_SETTINGS] = writeObj;

    return saveRootObject(rootObj);
}

ModbusLastParams SettingsManager::loadLastParams()
{
    ModbusLastParams defaults; // Значения по умолчанию уже заданы в объявлении структуры
    QJsonObject rootObj = loadRootObject();

    // Загружаем ветку чтения, если она существует
    if (rootObj.contains(KEY_READ_SETTINGS)) {
        QJsonObject readObj = rootObj[KEY_READ_SETTINGS].toObject();
        defaults.readDeviceAddrs = readObj[KEY_READ_DEV_ADDR].toString(defaults.readDeviceAddrs);
        defaults.readFuncCode   = static_cast<quint8>(readObj[KEY_READ_FUNC_CODE].toInt(defaults.readFuncCode));
        defaults.readStartAddr  = static_cast<quint16>(readObj[KEY_READ_START_ADDR].toInt(defaults.readStartAddr));
        defaults.readCount      = static_cast<quint16>(readObj[KEY_READ_COUNT].toInt(defaults.readCount));
    }

    // Загружаем ветку записи, если она существует
    if (rootObj.contains(KEY_WRITE_SETTINGS)) {
        QJsonObject writeObj = rootObj[KEY_WRITE_SETTINGS].toObject();
        defaults.writeDeviceAddr = static_cast<quint8>(writeObj[KEY_WRITE_DEV_ADDR].toInt(defaults.writeDeviceAddr));
        defaults.writeFuncCode   = static_cast<quint8>(writeObj[KEY_WRITE_FUNC_CODE].toInt(defaults.writeFuncCode));
        defaults.writeStartAddr  = static_cast<quint16>(writeObj[KEY_WRITE_START_ADDR].toInt(defaults.writeStartAddr));
        defaults.writeCount      = static_cast<quint16>(writeObj[KEY_WRITE_COUNT].toInt(defaults.writeCount));
        defaults.writeData       = writeObj[KEY_WRITE_DATA].toString(defaults.writeData);
    }

    return defaults;
}