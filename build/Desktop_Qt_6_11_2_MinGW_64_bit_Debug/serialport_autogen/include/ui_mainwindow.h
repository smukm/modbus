/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QComboBox *cbPorts;
    QLabel *label;
    QPushButton *btnApply;
    QPushButton *btnSendData;
    QTextEdit *textEditLog;
    QLabel *label_12;
    QLineEdit *leData;
    QGroupBox *groupBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QComboBox *cbStopBits;
    QLabel *label_6;
    QComboBox *cbParity;
    QComboBox *cbBaudRate;
    QLabel *label_5;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *cbDataBits;
    QGroupBox *groupBox_2;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QLabel *label_8;
    QLineEdit *leDeviceAddress;
    QLabel *label_11;
    QLabel *label_10;
    QLabel *label_9;
    QComboBox *cbCode;
    QLabel *label_7;
    QTextEdit *textEditRecievedData;
    QLineEdit *leRegisterAddress;
    QLineEdit *leRegistersQty;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        cbPorts = new QComboBox(centralwidget);
        cbPorts->setObjectName("cbPorts");
        cbPorts->setGeometry(QRect(30, 30, 191, 22));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 10, 91, 16));
        btnApply = new QPushButton(centralwidget);
        btnApply->setObjectName("btnApply");
        btnApply->setGeometry(QRect(230, 30, 80, 21));
        btnSendData = new QPushButton(centralwidget);
        btnSendData->setObjectName("btnSendData");
        btnSendData->setGeometry(QRect(690, 30, 80, 21));
        textEditLog = new QTextEdit(centralwidget);
        textEditLog->setObjectName("textEditLog");
        textEditLog->setGeometry(QRect(30, 340, 741, 221));
        label_12 = new QLabel(centralwidget);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(30, 320, 41, 14));
        leData = new QLineEdit(centralwidget);
        leData->setObjectName("leData");
        leData->setGeometry(QRect(340, 30, 341, 22));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(30, 60, 281, 201));
        gridLayoutWidget = new QWidget(groupBox);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(10, 20, 261, 171));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        cbStopBits = new QComboBox(gridLayoutWidget);
        cbStopBits->setObjectName("cbStopBits");

        gridLayout->addWidget(cbStopBits, 3, 1, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        cbParity = new QComboBox(gridLayoutWidget);
        cbParity->setObjectName("cbParity");

        gridLayout->addWidget(cbParity, 2, 1, 1, 1);

        cbBaudRate = new QComboBox(gridLayoutWidget);
        cbBaudRate->setObjectName("cbBaudRate");

        gridLayout->addWidget(cbBaudRate, 0, 1, 1, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        cbDataBits = new QComboBox(gridLayoutWidget);
        cbDataBits->setObjectName("cbDataBits");

        gridLayout->addWidget(cbDataBits, 1, 1, 1, 1);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(340, 60, 431, 231));
        gridLayoutWidget_2 = new QWidget(groupBox_2);
        gridLayoutWidget_2->setObjectName("gridLayoutWidget_2");
        gridLayoutWidget_2->setGeometry(QRect(10, 20, 411, 201));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(gridLayoutWidget_2);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 0, 0, 1, 1);

        leDeviceAddress = new QLineEdit(gridLayoutWidget_2);
        leDeviceAddress->setObjectName("leDeviceAddress");

        gridLayout_2->addWidget(leDeviceAddress, 0, 1, 1, 1);

        label_11 = new QLabel(gridLayoutWidget_2);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 7, 0, 1, 1);

        label_10 = new QLabel(gridLayoutWidget_2);
        label_10->setObjectName("label_10");

        gridLayout_2->addWidget(label_10, 6, 0, 1, 1);

        label_9 = new QLabel(gridLayoutWidget_2);
        label_9->setObjectName("label_9");

        gridLayout_2->addWidget(label_9, 2, 0, 1, 1);

        cbCode = new QComboBox(gridLayoutWidget_2);
        cbCode->setObjectName("cbCode");

        gridLayout_2->addWidget(cbCode, 2, 1, 1, 1);

        label_7 = new QLabel(gridLayoutWidget_2);
        label_7->setObjectName("label_7");

        gridLayout_2->addWidget(label_7, 8, 0, 1, 1);

        textEditRecievedData = new QTextEdit(gridLayoutWidget_2);
        textEditRecievedData->setObjectName("textEditRecievedData");

        gridLayout_2->addWidget(textEditRecievedData, 8, 1, 1, 1);

        leRegisterAddress = new QLineEdit(gridLayoutWidget_2);
        leRegisterAddress->setObjectName("leRegisterAddress");

        gridLayout_2->addWidget(leRegisterAddress, 6, 1, 1, 1);

        leRegistersQty = new QLineEdit(gridLayoutWidget_2);
        leRegistersQty->setObjectName("leRegistersQty");

        gridLayout_2->addWidget(leRegistersQty, 7, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 19));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(cbPorts, btnApply);
        QWidget::setTabOrder(btnApply, cbBaudRate);
        QWidget::setTabOrder(cbBaudRate, cbDataBits);
        QWidget::setTabOrder(cbDataBits, cbParity);
        QWidget::setTabOrder(cbParity, cbStopBits);
        QWidget::setTabOrder(cbStopBits, leDeviceAddress);
        QWidget::setTabOrder(leDeviceAddress, cbCode);
        QWidget::setTabOrder(cbCode, leRegisterAddress);
        QWidget::setTabOrder(leRegisterAddress, leRegistersQty);
        QWidget::setTabOrder(leRegistersQty, leData);
        QWidget::setTabOrder(leData, btnSendData);
        QWidget::setTabOrder(btnSendData, textEditLog);
        QWidget::setTabOrder(textEditLog, textEditRecievedData);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\321\201\321\202\321\203\320\277\320\275\321\213\320\265 \320\277\320\276\321\200\321\202\321\213", nullptr));
        btnApply->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", nullptr));
        btnSendData->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\321\214", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "\320\233\320\276\320\263\320\270", nullptr));
        leData->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\267\320\275\320\260\321\207\320\265\320\275\320\270\321\217 \321\207\320\265\321\200\320\265\320\267 \320\267\320\260\320\277\321\217\321\202\321\203\321\216 (\320\275\320\260\320\277\321\200\320\270\320\274\320\265\321\200:  10, 20, 30)", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \320\277\320\276\321\200\321\202\320\260", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\276\320\277-\320\261\320\270\321\202\321\213", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\320\247\320\265\321\202\320\275\320\276\321\201\321\202\321\214", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\241\320\272\320\276\321\200\320\276\321\201\321\202\321\214", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\320\221\320\270\321\202\321\213 \320\264\320\260\320\275\320\275\321\213\321\205", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\274\320\260\320\275\320\264\320\260", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 SlaveID", nullptr));
        leDeviceAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x01", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\276\320\262", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\260", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\264 \321\204\321\203\320\275\320\272\321\206\320\270\320\270", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\273\321\203\321\207\320\265\320\275\320\275\321\213\320\265 \320\264\320\260\320\275\320\275\321\213\320\265:", nullptr));
        leRegisterAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        leRegistersQty->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
