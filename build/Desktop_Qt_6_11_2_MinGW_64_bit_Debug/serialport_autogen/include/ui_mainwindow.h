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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QComboBox *cbPorts;
    QPushButton *btnApply;
    QGroupBox *gbPortSettings;
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
    QGroupBox *gbCommand;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QComboBox *cbReadCode;
    QLabel *label_11;
    QLineEdit *leReadRegisterAddress;
    QLabel *label_9;
    QLineEdit *leReadDeviceAddress;
    QLineEdit *leReadRegistersQty;
    QLabel *label_10;
    QLabel *label_8;
    QPushButton *btnSendData;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox;
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_3;
    QLineEdit *leWriteDeviceAddress;
    QLabel *label_14;
    QComboBox *cbWriteCode;
    QLabel *label_13;
    QLabel *label_15;
    QLineEdit *leWriteRegisterAddress;
    QLabel *label_16;
    QLineEdit *leWriteRegistersQty;
    QPushButton *btnExecuteOnce;
    QLineEdit *leData;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QTableView *tvReceivedData;
    QLabel *label_12;
    QListView *lvLog;
    QPushButton *btnClearLogs;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1231, 807);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_5 = new QHBoxLayout(centralwidget);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(15);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(5, -1, -1, -1);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        cbPorts = new QComboBox(centralwidget);
        cbPorts->setObjectName("cbPorts");

        horizontalLayout->addWidget(cbPorts);

        btnApply = new QPushButton(centralwidget);
        btnApply->setObjectName("btnApply");

        horizontalLayout->addWidget(btnApply);

        horizontalLayout->setStretch(0, 2);

        verticalLayout->addLayout(horizontalLayout);

        gbPortSettings = new QGroupBox(centralwidget);
        gbPortSettings->setObjectName("gbPortSettings");
        gridLayoutWidget = new QWidget(gbPortSettings);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(10, 20, 371, 171));
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


        verticalLayout->addWidget(gbPortSettings);

        gbCommand = new QGroupBox(centralwidget);
        gbCommand->setObjectName("gbCommand");
        gridLayoutWidget_2 = new QWidget(gbCommand);
        gridLayoutWidget_2->setObjectName("gridLayoutWidget_2");
        gridLayoutWidget_2->setGeometry(QRect(10, 30, 371, 160));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        cbReadCode = new QComboBox(gridLayoutWidget_2);
        cbReadCode->setObjectName("cbReadCode");

        gridLayout_2->addWidget(cbReadCode, 2, 1, 1, 1);

        label_11 = new QLabel(gridLayoutWidget_2);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 7, 0, 1, 1);

        leReadRegisterAddress = new QLineEdit(gridLayoutWidget_2);
        leReadRegisterAddress->setObjectName("leReadRegisterAddress");

        gridLayout_2->addWidget(leReadRegisterAddress, 6, 1, 1, 1);

        label_9 = new QLabel(gridLayoutWidget_2);
        label_9->setObjectName("label_9");

        gridLayout_2->addWidget(label_9, 2, 0, 1, 1);

        leReadDeviceAddress = new QLineEdit(gridLayoutWidget_2);
        leReadDeviceAddress->setObjectName("leReadDeviceAddress");

        gridLayout_2->addWidget(leReadDeviceAddress, 0, 1, 1, 1);

        leReadRegistersQty = new QLineEdit(gridLayoutWidget_2);
        leReadRegistersQty->setObjectName("leReadRegistersQty");

        gridLayout_2->addWidget(leReadRegistersQty, 7, 1, 1, 1);

        label_10 = new QLabel(gridLayoutWidget_2);
        label_10->setObjectName("label_10");

        gridLayout_2->addWidget(label_10, 6, 0, 1, 1);

        label_8 = new QLabel(gridLayoutWidget_2);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 0, 0, 1, 1);

        btnSendData = new QPushButton(gbCommand);
        btnSendData->setObjectName("btnSendData");
        btnSendData->setGeometry(QRect(500, 30, 85, 27));

        verticalLayout->addWidget(gbCommand);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        gridLayoutWidget_3 = new QWidget(groupBox);
        gridLayoutWidget_3->setObjectName("gridLayoutWidget_3");
        gridLayoutWidget_3->setGeometry(QRect(10, 30, 361, 141));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_3);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        leWriteDeviceAddress = new QLineEdit(gridLayoutWidget_3);
        leWriteDeviceAddress->setObjectName("leWriteDeviceAddress");

        gridLayout_3->addWidget(leWriteDeviceAddress, 0, 1, 1, 1);

        label_14 = new QLabel(gridLayoutWidget_3);
        label_14->setObjectName("label_14");

        gridLayout_3->addWidget(label_14, 1, 0, 1, 1);

        cbWriteCode = new QComboBox(gridLayoutWidget_3);
        cbWriteCode->setObjectName("cbWriteCode");

        gridLayout_3->addWidget(cbWriteCode, 1, 1, 1, 1);

        label_13 = new QLabel(gridLayoutWidget_3);
        label_13->setObjectName("label_13");

        gridLayout_3->addWidget(label_13, 0, 0, 1, 1);

        label_15 = new QLabel(gridLayoutWidget_3);
        label_15->setObjectName("label_15");

        gridLayout_3->addWidget(label_15, 2, 0, 1, 1);

        leWriteRegisterAddress = new QLineEdit(gridLayoutWidget_3);
        leWriteRegisterAddress->setObjectName("leWriteRegisterAddress");

        gridLayout_3->addWidget(leWriteRegisterAddress, 2, 1, 1, 1);

        label_16 = new QLabel(gridLayoutWidget_3);
        label_16->setObjectName("label_16");

        gridLayout_3->addWidget(label_16, 3, 0, 1, 1);

        leWriteRegistersQty = new QLineEdit(gridLayoutWidget_3);
        leWriteRegistersQty->setObjectName("leWriteRegistersQty");

        gridLayout_3->addWidget(leWriteRegistersQty, 3, 1, 1, 1);

        btnExecuteOnce = new QPushButton(groupBox);
        btnExecuteOnce->setObjectName("btnExecuteOnce");
        btnExecuteOnce->setGeometry(QRect(500, 80, 80, 27));
        leData = new QLineEdit(groupBox);
        leData->setObjectName("leData");
        leData->setGeometry(QRect(380, 30, 201, 27));

        verticalLayout_4->addWidget(groupBox);


        verticalLayout->addLayout(verticalLayout_4);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        verticalLayout_3->addWidget(label_2);

        tvReceivedData = new QTableView(centralwidget);
        tvReceivedData->setObjectName("tvReceivedData");

        verticalLayout_3->addWidget(tvReceivedData);

        label_12 = new QLabel(centralwidget);
        label_12->setObjectName("label_12");

        verticalLayout_3->addWidget(label_12);

        lvLog = new QListView(centralwidget);
        lvLog->setObjectName("lvLog");

        verticalLayout_3->addWidget(lvLog);

        btnClearLogs = new QPushButton(centralwidget);
        btnClearLogs->setObjectName("btnClearLogs");

        verticalLayout_3->addWidget(btnClearLogs);


        horizontalLayout_3->addLayout(verticalLayout_3);


        horizontalLayout_5->addLayout(horizontalLayout_3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1231, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(btnApply, cbBaudRate);
        QWidget::setTabOrder(cbBaudRate, cbDataBits);
        QWidget::setTabOrder(cbDataBits, cbParity);
        QWidget::setTabOrder(cbParity, cbStopBits);
        QWidget::setTabOrder(cbStopBits, leReadDeviceAddress);
        QWidget::setTabOrder(leReadDeviceAddress, cbReadCode);
        QWidget::setTabOrder(cbReadCode, leReadRegisterAddress);
        QWidget::setTabOrder(leReadRegisterAddress, leReadRegistersQty);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnApply->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", nullptr));
        gbPortSettings->setTitle(QCoreApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \320\277\320\276\321\200\321\202\320\260", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\276\320\277-\320\261\320\270\321\202\321\213", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\320\247\320\265\321\202\320\275\320\276\321\201\321\202\321\214", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\241\320\272\320\276\321\200\320\276\321\201\321\202\321\214", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\320\221\320\270\321\202\321\213 \320\264\320\260\320\275\320\275\321\213\321\205", nullptr));
        gbCommand->setTitle(QCoreApplication::translate("MainWindow", "\320\247\321\202\320\265\320\275\320\270\320\265 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\276\320\262", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\276\320\262", nullptr));
        leReadRegisterAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\264 \321\204\321\203\320\275\320\272\321\206\320\270\320\270", nullptr));
        leReadDeviceAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x01", nullptr));
        leReadRegistersQty->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\260", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 SlaveID", nullptr));
        btnSendData->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\320\277\320\276\320\273\320\275\320\270\321\202\321\214", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\320\227\320\260\320\277\320\270\321\201\321\214 \320\262 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\321\213", nullptr));
        leWriteDeviceAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x01", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\264 \321\204\321\203\320\275\320\272\321\206\320\270\320\270", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 SlaveID", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\260", nullptr));
        leWriteRegisterAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\276\320\262", nullptr));
        leWriteRegistersQty->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        btnExecuteOnce->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\320\277\320\276\320\273\320\275\320\270\321\202\321\214", nullptr));
        leData->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\267\320\275\320\260\321\207\320\265\320\275\320\270\321\217 \321\207\320\265\321\200\320\265\320\267 \320\267\320\260\320\277\321\217\321\202\321\203\321\216 (\320\275\320\260\320\277\321\200\320\270\320\274\320\265\321\200:  10, 20, 30)", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\262\320\265\321\202 \320\276\321\202 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "\320\233\320\276\320\263\320\270", nullptr));
        btnClearLogs->setText(QCoreApplication::translate("MainWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 \320\273\320\276\320\263\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
