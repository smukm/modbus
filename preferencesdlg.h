#ifndef PREFERENCESDLG_H
#define PREFERENCESDLG_H

#include "settingsmanager.h"
#include <QDialog>

namespace Ui {
class PreferencesDlg;
}

class PreferencesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDlg(QWidget *parent = nullptr);
    ~PreferencesDlg();

    // Загружает текущие настройки из SettingsManager в поля UI
    void loadSettings();

signals:
    // Сигнал о том, что настройки были успешно сохранены и их нужно применить
    void settingsApplied(const Preferences &newPreferences);

private slots:
    void onButtonBoxAccepted();

private:
    Ui::PreferencesDlg *ui;
};

#endif // PREFERENCESDLG_H
