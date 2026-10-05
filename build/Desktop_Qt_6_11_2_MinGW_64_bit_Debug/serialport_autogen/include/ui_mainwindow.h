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
#include <QtWidgets/QCheckBox>
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
    QWidget *horizontalLayoutWidget_2;
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
    QComboBox *cbCode;
    QLabel *label_11;
    QLineEdit *leRegistersQty;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_8;
    QLineEdit *leRegisterAddress;
    QLineEdit *leDeviceAddress;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *leData;
    QPushButton *btnSendData;
    QCheckBox *cbPolling;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QTableView *tvReceivedData;
    QLabel *label_12;
    QListView *lvLog;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayoutWidget_2 = new QWidget(centralwidget);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(0, 0, 781, 541));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_3->setSpacing(15);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(5, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        cbPorts = new QComboBox(horizontalLayoutWidget_2);
        cbPorts->setObjectName("cbPorts");

        horizontalLayout->addWidget(cbPorts);

        btnApply = new QPushButton(horizontalLayoutWidget_2);
        btnApply->setObjectName("btnApply");

        horizontalLayout->addWidget(btnApply);

        horizontalLayout->setStretch(0, 2);

        verticalLayout->addLayout(horizontalLayout);

        gbPortSettings = new QGroupBox(horizontalLayoutWidget_2);
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

        gbCommand = new QGroupBox(horizontalLayoutWidget_2);
        gbCommand->setObjectName("gbCommand");
        gridLayoutWidget_2 = new QWidget(gbCommand);
        gridLayoutWidget_2->setObjectName("gridLayoutWidget_2");
        gridLayoutWidget_2->setGeometry(QRect(10, 20, 371, 160));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        cbCode = new QComboBox(gridLayoutWidget_2);
        cbCode->setObjectName("cbCode");

        gridLayout_2->addWidget(cbCode, 2, 1, 1, 1);

        label_11 = new QLabel(gridLayoutWidget_2);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 7, 0, 1, 1);

        leRegistersQty = new QLineEdit(gridLayoutWidget_2);
        leRegistersQty->setObjectName("leRegistersQty");

        gridLayout_2->addWidget(leRegistersQty, 7, 1, 1, 1);

        label_9 = new QLabel(gridLayoutWidget_2);
        label_9->setObjectName("label_9");

        gridLayout_2->addWidget(label_9, 2, 0, 1, 1);

        label_10 = new QLabel(gridLayoutWidget_2);
        label_10->setObjectName("label_10");

        gridLayout_2->addWidget(label_10, 6, 0, 1, 1);

        label_8 = new QLabel(gridLayoutWidget_2);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 0, 0, 1, 1);

        leRegisterAddress = new QLineEdit(gridLayoutWidget_2);
        leRegisterAddress->setObjectName("leRegisterAddress");

        gridLayout_2->addWidget(leRegisterAddress, 6, 1, 1, 1);

        leDeviceAddress = new QLineEdit(gridLayoutWidget_2);
        leDeviceAddress->setObjectName("leDeviceAddress");

        gridLayout_2->addWidget(leDeviceAddress, 0, 1, 1, 1);


        verticalLayout->addWidget(gbCommand);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        leData = new QLineEdit(horizontalLayoutWidget_2);
        leData->setObjectName("leData");

        horizontalLayout_4->addWidget(leData);

        btnSendData = new QPushButton(horizontalLayoutWidget_2);
        btnSendData->setObjectName("btnSendData");

        horizontalLayout_4->addWidget(btnSendData);

        horizontalLayout_4->setStretch(0, 1);

        verticalLayout_4->addLayout(horizontalLayout_4);

        cbPolling = new QCheckBox(horizontalLayoutWidget_2);
        cbPolling->setObjectName("cbPolling");

        verticalLayout_4->addWidget(cbPolling);


        verticalLayout->addLayout(verticalLayout_4);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_2 = new QLabel(horizontalLayoutWidget_2);
        label_2->setObjectName("label_2");

        verticalLayout_3->addWidget(label_2);

        tvReceivedData = new QTableView(horizontalLayoutWidget_2);
        tvReceivedData->setObjectName("tvReceivedData");

        verticalLayout_3->addWidget(tvReceivedData);

        label_12 = new QLabel(horizontalLayoutWidget_2);
        label_12->setObjectName("label_12");

        verticalLayout_3->addWidget(label_12);

        lvLog = new QListView(horizontalLayoutWidget_2);
        lvLog->setObjectName("lvLog");

        verticalLayout_3->addWidget(lvLog);


        horizontalLayout_3->addLayout(verticalLayout_3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 19));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(btnApply, cbBaudRate);
        QWidget::setTabOrder(cbBaudRate, cbDataBits);
        QWidget::setTabOrder(cbDataBits, cbParity);
        QWidget::setTabOrder(cbParity, cbStopBits);
        QWidget::setTabOrder(cbStopBits, leDeviceAddress);
        QWidget::setTabOrder(leDeviceAddress, cbCode);
        QWidget::setTabOrder(cbCode, leRegisterAddress);
        QWidget::setTabOrder(leRegisterAddress, leRegistersQty);
        QWidget::setTabOrder(leRegistersQty, leData);

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
        gbCommand->setTitle(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\274\320\260\320\275\320\264\320\260", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\276\320\262", nullptr));
        leRegistersQty->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\264 \321\204\321\203\320\275\320\272\321\206\320\270\320\270", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\260", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 SlaveID", nullptr));
        leRegisterAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        leDeviceAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x01", nullptr));
        leData->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\267\320\275\320\260\321\207\320\265\320\275\320\270\321\217 \321\207\320\265\321\200\320\265\320\267 \320\267\320\260\320\277\321\217\321\202\321\203\321\216 (\320\275\320\260\320\277\321\200\320\270\320\274\320\265\321\200:  10, 20, 30)", nullptr));
        btnSendData->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\320\277\320\276\320\273\320\275\320\270\321\202\321\214", nullptr));
        cbPolling->setText(QCoreApplication::translate("MainWindow", "\320\237\320\265\321\200\320\270\320\276\320\264\320\270\321\207\320\265\321\201\320\272\320\270\320\271 \320\277\320\276\320\262\321\202\320\276\321\200", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\262\320\265\321\202 \320\276\321\202 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "\320\233\320\276\320\263\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
