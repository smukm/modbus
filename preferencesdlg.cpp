#include "preferencesdlg.h"
#include "ui_preferencesdlg.h"
#include <QMessageBox>
#include <QPushButton>

PreferencesDlg::PreferencesDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDlg)
{
    ui->setupUi(this);

    // Перехватываем стандартное поведение кнопки OK, чтобы сначала выполнить валидацию
    ui->buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDlg::onButtonBoxAccepted);
}

PreferencesDlg::~PreferencesDlg()
{
    delete ui;
}


void PreferencesDlg::loadSettings()
{
    Preferences prefs = SettingsManager::loadPreferences();
    ui->lePollInterval->setText(QString::number(prefs.pollInterval));
    ui->leModbusRtuTimeout->setText(QString::number(prefs.modbusRtuTimeout));
    ui->leModbusRtuRetries->setText(QString::number(prefs.modbusRtuRetries));
    ui->leModbusTcpTimeout->setText(QString::number(prefs.modbusTcpTimeout));
    ui->leModbusTcpRetries->setText(QString::number(prefs.modbusTcpRetries));
}


void PreferencesDlg::onButtonBoxAccepted()
{
    bool ok1, ok2, ok3, ok4, ok5;

    // Используем toUInt для безопасного парсинга положительных чисел
    quint32 valPoll = ui->lePollInterval->text().toUInt(&ok1);
    quint32 valRtuTime = ui->leModbusRtuTimeout->text().toUInt(&ok2);
    quint32 valRtuRet = ui->leModbusRtuRetries->text().toUInt(&ok3);
    quint32 valTcpTime = ui->leModbusTcpTimeout->text().toUInt(&ok4);
    quint32 valTcpRet = ui->leModbusTcpRetries->text().toUInt(&ok5);

    if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5) {
        QMessageBox::warning(this, tr("Input Error"), tr("All fields must contain valid positive numbers."));
        return;
    }

    if (valPoll < 100) {
        QMessageBox::warning(this, tr("Input Error"), tr("Poll interval must be at least 100 ms to prevent UI freezing."));
        return;
    }

    Preferences newPrefs;
    newPrefs.pollInterval = static_cast<quint16>(valPoll);
    newPrefs.modbusRtuTimeout = static_cast<quint16>(valRtuTime);
    newPrefs.modbusRtuRetries = static_cast<quint8>(valRtuRet);
    newPrefs.modbusTcpTimeout = static_cast<quint16>(valTcpTime);
    newPrefs.modbusTcpRetries = static_cast<quint8>(valTcpRet);

    if (SettingsManager::savePreferences(newPrefs)) {
        emit settingsApplied(newPrefs);
        accept(); // Закрываем диалог с кодом успеха
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save preferences to file."));
    }
}