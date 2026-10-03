#include "mainwindow.h"
#include "comport.h"
#include "./ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_comPort(nullptr)
{
    ui->setupUi(this);

    // Заполняем список доступных портов
    setPorts();

    setControlsForClosedPort();

    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::slotApplySettings);
    connect(ui->btnSendData, &QPushButton::clicked, this, &MainWindow::slotSendData);
}

/**
 * @brief Инициализирует и заполняет выпадающий список доступных COM-портов.
 * Также создает экземпляр класса Comport для первого найденного порта.
 */
void MainWindow::setPorts() {
    // Получаем список портов
    QList<QSerialPortInfo> const ports = Comport::getAvailablePorts();

    ui->cbPorts->clear();

    if (ports.isEmpty()) {
        ui->cbPorts->addItem("Нет доступных COM-портов");
        ui->cbPorts->setEnabled(false);
        ui->btnApply->setEnabled(false);
    } else {

        for (const QSerialPortInfo &info : ports) {
            QString displayText = info.portName();
            if (!info.description().isEmpty()) {
                displayText += QString(" (%1)").arg(info.description());
            }

            // addItem(Текст для отображения, Данные для внутреннего использования)
            ui->cbPorts->addItem(displayText, info.portName());
        }
        ui->cbPorts->setEnabled(true);
        ui->btnApply->setEnabled(true);

        // Создаем объект Comport для первого доступного порта
        QString firstPortName = ui->cbPorts->currentData().toString();
        m_comPort = new Comport(firstPortName, this); // this делает MainWindow родителем, удаление будет автоматическим

        fillSettings();
        setControlsForSendData();
        setControlsForClosedPort();

        connect(m_comPort, &Comport::portOpened, this, &MainWindow::slotPortOpened);
        connect(m_comPort, &Comport::portClosed, this, &MainWindow::slotPortClosed);
        connect(m_comPort, &Comport::modbusFrameReceived, this, &MainWindow::slotModbusResponse);
    }
}

/**
 * @brief Заполняет ComboBox-ы настройками порта.
 */
void MainWindow::fillSettings() {
    // Скорость порта
    ui->cbBaudRate->clear();
    ui->cbBaudRate->addItem("1200", QSerialPort::Baud1200);
    ui->cbBaudRate->addItem("2400", QSerialPort::Baud2400);
    ui->cbBaudRate->addItem("4800", QSerialPort::Baud4800);
    ui->cbBaudRate->addItem("9600", QSerialPort::Baud9600);
    ui->cbBaudRate->addItem("19200", QSerialPort::Baud19200);
    ui->cbBaudRate->addItem("38400", QSerialPort::Baud38400);
    ui->cbBaudRate->addItem("57600", QSerialPort::Baud57600);
    ui->cbBaudRate->addItem("115200", QSerialPort::Baud115200);
    ui->cbBaudRate->setCurrentText("9600"); // Выбираем по тексту

    // Биты данных
    ui->cbDataBits->clear();
    ui->cbDataBits->addItem("5 бит", QSerialPort::Data5);
    ui->cbDataBits->addItem("6 бит", QSerialPort::Data6);
    ui->cbDataBits->addItem("7 бит", QSerialPort::Data7);
    ui->cbDataBits->addItem("8 бит", QSerialPort::Data8);
    ui->cbDataBits->setCurrentText("8 бит");

    // Четность
    ui->cbParity->clear();
    ui->cbParity->addItem("Без контроля четности (None)", QSerialPort::NoParity);
    ui->cbParity->addItem("Четный (Even)", QSerialPort::EvenParity);
    ui->cbParity->addItem("Нечетный (Odd)", QSerialPort::OddParity);
    ui->cbParity->addItem("Space", QSerialPort::SpaceParity);
    ui->cbParity->addItem("Mark", QSerialPort::MarkParity);
    ui->cbParity->setCurrentText("Без контроля четности (None)");

    // Стоп-биты
    ui->cbStopBits->clear();
    ui->cbStopBits->addItem("1 стоп-бит", QSerialPort::OneStop);
    ui->cbStopBits->addItem("1.5 стоп-бита", QSerialPort::OneAndHalfStop);
    ui->cbStopBits->addItem("2 стоп-бита", QSerialPort::TwoStop);
    ui->cbStopBits->setCurrentText("1 стоп-бит");

    // Управление потоком
    ui->cbFlowControl->clear();
    ui->cbFlowControl->addItem("Нет (No Flow Control)", QSerialPort::NoFlowControl);
    ui->cbFlowControl->addItem("Аппаратный (Hardware RTS/CTS)", QSerialPort::HardwareControl);
    ui->cbFlowControl->addItem("Программный (Software XON/XOFF)", QSerialPort::SoftwareControl);
    ui->cbFlowControl->setCurrentText("Нет (No Flow Control)");
}

/**
 * @brief Слот-обработчик нажатия кнопки "Открыть/Закрыть" (btnApply).
 * Реализует логику переключения (toggle): если порт открыт — закрываем его,
 * если закрыт — считываем настройки из UI и открываем.
 */
void MainWindow::slotApplySettings() {
    if (!m_comPort) {
        return;
    }

    if (m_comPort->isOpen()) {
        m_comPort->closePort();

    } else {
        openPortWithSettings();
    }
}

/**
 * @brief Считывает текущие значения из всех ComboBox-ов,
 * формирует структуру PortSettings и пытается открыть порт.
 */
void MainWindow::openPortWithSettings() {
    QSerialPort::BaudRate selectedBaud = static_cast<QSerialPort::BaudRate>(ui->cbBaudRate->currentData().toInt());
    QSerialPort::DataBits selectedDataBits = static_cast<QSerialPort::DataBits>(ui->cbDataBits->currentData().toInt());
    QSerialPort::Parity selectedParity = static_cast<QSerialPort::Parity>(ui->cbParity->currentData().toInt());
    QSerialPort::StopBits selectedStopBits = static_cast<QSerialPort::StopBits>(ui->cbStopBits->currentData().toInt());
    QSerialPort::FlowControl selectedFlowControl = static_cast<QSerialPort::FlowControl>(ui->cbFlowControl->currentData().toInt());

    // Получаем системное имя порта (например, "COM3")
    QString selectedPortName = ui->cbPorts->currentData().toString();


    PortSettings portSettings = {
        .portName = selectedPortName,
        .baudRate =  selectedBaud,
        .dataBits = selectedDataBits,
        .parity = selectedParity,
        .stopBits = selectedStopBits,
        .flowControl = selectedFlowControl
    };
    qDebug() << portSettings.portName;

    // Применяем настройки к объекту Comport
    m_comPort->setPortSettings(portSettings);

    qDebug() << "Открываем порт";

    // Пытаемся открыть порт
    if (!m_comPort->openRW()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть порт с текущими настройками!\n");
    }
}

/**
 * @brief Обновляет состояние интерфейса при успешном открытии порта.
 * Блокирует изменение настроек (чтобы избежать конфликтов на лету)
 * и активирует функционал работы с данными.
 */
void MainWindow::slotPortOpened() {
    setControlsForOpenPort();
}

/**
 * @brief Обновляет состояние интерфейса при закрытии порта.
 * Возвращает возможность изменения настроек и меняет текст кнопки.
 */
void MainWindow::slotPortClosed() {
    setControlsForClosedPort();
}

/**
 * @brief Отправка данных в открытый порт
 */
void MainWindow::slotSendData() {
    if (!m_comPort || !m_comPort->isOpen()) {
        qDebug() << "Невозможно отправить: порт закрыт!";
        return;
    }

    // base = 0 позволяет автоматически определять систему счисления:
    // "0xFF" -> HEX, "0" -> OCT, "255" -> DEC
    auto parseByte = [](const QString &text, bool &ok) -> quint8 {
        int val = text.trimmed().toInt(&ok, 0);
        if (!ok || val < 0 || val > 255) {
            ok = false;
            return 0;
        }
        return static_cast<quint8>(val);
    };
    bool ok;

    // 1. Считываем и парсим поля из UI
    quint8 deviceAddr = parseByte(ui->leDeviceAddress->text(), ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный адрес устройства (ожидается 0-255 или 0x00-0xFF)!");
        return;
    }

    quint8 funcCode = static_cast<quint8>(ui->cbCode->currentData().toInt());

    quint8 regAddrHigh = parseByte(ui->leRegisterAddress1->text(), ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный старший байт адреса регистра!");
        return;
    }

    quint8 regAddrLow = parseByte(ui->leRegisterAddress2->text(), ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный младший байт адреса регистра!");
        return;
    }

    quint8 regQtyHigh = parseByte(ui->leRegistersQty1->text(), ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный старший байт количества регистров!");
        return;
    }

    quint8 regQtyLow = parseByte(ui->leRegistersQty2->text(), ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка ввода", "Некорректный младший байт количества регистров!");
        return;
    }

    // 1. Формируем кадр Modbus RTU (без CRC)
    QByteArray modbusFrame;
    modbusFrame.append(static_cast<char>(deviceAddr)); // Адрес устройства: 0x01
    modbusFrame.append(static_cast<char>(funcCode)); // Код функции: 0x03 (Read Holding Registers)
    modbusFrame.append(static_cast<char>(regAddrHigh)); // Старший байт адреса регистра: 0x00
    modbusFrame.append(static_cast<char>(regAddrLow)); // Младший байт адреса регистра: 0x00 (регистр 0)
    modbusFrame.append(static_cast<char>(regQtyHigh)); // Старший байт количества регистров: 0x00
    modbusFrame.append(static_cast<char>(regQtyLow)); // Младший байт количества регистров: 0x01 (читать 1 регистр)

    // 2. Рассчитываем CRC16
    quint16 crc = m_comPort->calculateModbusCRC16(modbusFrame);

    // 3. Добавляем CRC в конец кадра (Modbus использует Little Endian для CRC)
    modbusFrame.append(static_cast<char>(crc & 0xFF));        // Младший байт CRC
    modbusFrame.append(static_cast<char>((crc >> 8) & 0xFF)); // Старший байт CRC

    // 4. Отправляем бинарные данные
    qint64 bytesSent = m_comPort->sendData(modbusFrame);

    if (bytesSent > 0) {
        // Выводим в консоль отправленные байты в HEX-формате для наглядности
        qDebug() << "Отправлено байт:" << bytesSent
                 << "| Данные (HEX):" << modbusFrame.toHex(' ').toUpper();
    } else {
        qDebug() << "Ошибка отправки данных!";
    }
}

void MainWindow::slotModbusResponse(const QByteArray &frame) {
    // Здесь вы можете распарсить ответ.
    // Например, для функции 0x03 (чтение регистров):
    // frame[0] = адрес устройства
    // frame[1] = код функции (0x03)
    // frame[2] = количество байт данных
    // frame[3...N] = сами данные регистров
    // frame[N+1, N+2] = CRC (уже проверен, но физически там находится)

    qDebug() << "MainWindow получил полный кадр Modbus:" << frame.toHex(' ').toUpper();

    // Пример извлечения значения регистра (если это ответ на чтение 1 регистра)
    if (frame.size() >= 5 && frame[1] == 0x03) {
        quint8 byteCount = frame[2];
        if (byteCount >= 2) {
            quint16 registerValue = (static_cast<quint8>(frame[3]) << 8) | static_cast<quint8>(frame[4]);
            qDebug() << "Значение регистра:" << registerValue;
        }
    }
}

void MainWindow::setControlsForOpenPort() {
    ui->btnApply->setText("Закрыть");
    ui->cbPorts->setEnabled(false);
    ui->cbBaudRate->setEnabled(false);
    ui->cbDataBits->setEnabled(false);
    ui->cbParity->setEnabled(false);
    ui->cbStopBits->setEnabled(false);
    ui->cbFlowControl->setEnabled(false);

    ui->btnSendData->setEnabled(true);
    ui->leDeviceAddress->setEnabled(true);
    ui->cbCode->setEnabled(true);
    ui->leRegisterAddress1->setEnabled(true);
    ui->leRegisterAddress2->setEnabled(true);
    ui->leRegistersQty1->setEnabled(true);
    ui->leRegistersQty2->setEnabled(true);
}

void MainWindow::setControlsForClosedPort() {
    ui->btnApply->setEnabled(true);
    ui->btnApply->setText("Открыть");
    ui->cbPorts->setEnabled(true);
    ui->cbBaudRate->setEnabled(true);
    ui->cbDataBits->setEnabled(true);
    ui->cbParity->setEnabled(true);
    ui->cbStopBits->setEnabled(true);
    ui->cbFlowControl->setEnabled(true);

    ui->btnSendData->setEnabled(false);
    ui->leDeviceAddress->setEnabled(false);
    ui->cbCode->setEnabled(false);
    ui->leRegisterAddress1->setEnabled(false);
    ui->leRegisterAddress2->setEnabled(false);
    ui->leRegistersQty1->setEnabled(false);
    ui->leRegistersQty2->setEnabled(false);
}

/**
 * @brief Заполняет виджеты параметров отправки данных начальными значениями.
 */
void MainWindow::setControlsForSendData() {
    ui->leDeviceAddress->setText("0x01");
    ui->cbCode->clear();
    // addItem(Текст для отображения, Внутренние данные (int))
    ui->cbCode->addItem("01 (0x01) - Чтение дискретных выходов (Read Coils)", 0x01);
    ui->cbCode->addItem("02 (0x02) - Чтение дискретных входов (Read Discrete Inputs)", 0x02);
    ui->cbCode->addItem("03 (0x03) - Чтение регистров хранения (Read Holding Registers)", 0x03);
    ui->cbCode->addItem("04 (0x04) - Чтение входных регистров (Read Input Registers)", 0x04);
    ui->cbCode->addItem("05 (0x05) - Запись одного дискретного выхода (Write Single Coil)", 0x05);
    ui->cbCode->addItem("06 (0x06) - Запись одного регистра (Write Single Register)", 0x06);
    ui->cbCode->addItem("15 (0x0F) - Запись нескольких дискретных выходов (Write Multiple Coils)", 0x0F);
    ui->cbCode->addItem("16 (0x10) - Запись нескольких регистров (Write Multiple Registers)", 0x10);
    // Выбираем самую популярную функцию (0x03) по умолчанию
    ui->cbCode->setCurrentIndex(2);

    ui->leRegisterAddress1->setText("0x00");
    ui->leRegisterAddress2->setText("0x00");
    ui->leRegistersQty1->setText("0x00");
    ui->leRegistersQty2->setText("0x01");
}

MainWindow::~MainWindow()
{
    m_comPort->closePort();
    delete ui;
}
