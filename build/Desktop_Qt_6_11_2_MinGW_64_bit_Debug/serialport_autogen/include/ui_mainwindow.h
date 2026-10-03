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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QComboBox *cbPorts;
    QLabel *label;
    QPushButton *btnApply;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QComboBox *cbStopBits;
    QLabel *label_5;
    QLabel *label_6;
    QComboBox *cbDataBits;
    QComboBox *cbBaudRate;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *cbParity;
    QComboBox *cbFlowControl;
    QLabel *label_7;
    QLabel *label_2;
    QPushButton *btnSendData;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout;
    QLineEdit *leRegisterAddress1;
    QLineEdit *leRegisterAddress2;
    QLineEdit *leDeviceAddress;
    QLabel *label_11;
    QLabel *label_10;
    QLabel *label_9;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *leRegistersQty1;
    QLineEdit *leRegistersQty2;
    QComboBox *cbCode;
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
        cbPorts->setGeometry(QRect(30, 30, 131, 22));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 10, 41, 14));
        btnApply = new QPushButton(centralwidget);
        btnApply->setObjectName("btnApply");
        btnApply->setGeometry(QRect(170, 30, 80, 21));
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(30, 80, 221, 171));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        cbStopBits = new QComboBox(gridLayoutWidget);
        cbStopBits->setObjectName("cbStopBits");

        gridLayout->addWidget(cbStopBits, 3, 1, 1, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        cbDataBits = new QComboBox(gridLayoutWidget);
        cbDataBits->setObjectName("cbDataBits");

        gridLayout->addWidget(cbDataBits, 1, 1, 1, 1);

        cbBaudRate = new QComboBox(gridLayoutWidget);
        cbBaudRate->setObjectName("cbBaudRate");

        gridLayout->addWidget(cbBaudRate, 0, 1, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        cbParity = new QComboBox(gridLayoutWidget);
        cbParity->setObjectName("cbParity");

        gridLayout->addWidget(cbParity, 2, 1, 1, 1);

        cbFlowControl = new QComboBox(gridLayoutWidget);
        cbFlowControl->setObjectName("cbFlowControl");

        gridLayout->addWidget(cbFlowControl, 4, 1, 1, 1);

        label_7 = new QLabel(gridLayoutWidget);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 4, 0, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 60, 98, 22));
        btnSendData = new QPushButton(centralwidget);
        btnSendData->setObjectName("btnSendData");
        btnSendData->setGeometry(QRect(590, 30, 80, 21));
        gridLayoutWidget_2 = new QWidget(centralwidget);
        gridLayoutWidget_2->setObjectName("gridLayoutWidget_2");
        gridLayoutWidget_2->setGeometry(QRect(280, 80, 391, 201));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(gridLayoutWidget_2);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        leRegisterAddress1 = new QLineEdit(gridLayoutWidget_2);
        leRegisterAddress1->setObjectName("leRegisterAddress1");

        horizontalLayout->addWidget(leRegisterAddress1);

        leRegisterAddress2 = new QLineEdit(gridLayoutWidget_2);
        leRegisterAddress2->setObjectName("leRegisterAddress2");

        horizontalLayout->addWidget(leRegisterAddress2);


        gridLayout_2->addLayout(horizontalLayout, 6, 1, 1, 1);

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

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        leRegistersQty1 = new QLineEdit(gridLayoutWidget_2);
        leRegistersQty1->setObjectName("leRegistersQty1");

        horizontalLayout_2->addWidget(leRegistersQty1);

        leRegistersQty2 = new QLineEdit(gridLayoutWidget_2);
        leRegistersQty2->setObjectName("leRegistersQty2");

        horizontalLayout_2->addWidget(leRegistersQty2);


        gridLayout_2->addLayout(horizontalLayout_2, 7, 1, 1, 1);

        cbCode = new QComboBox(gridLayoutWidget_2);
        cbCode->setObjectName("cbCode");

        gridLayout_2->addWidget(cbCode, 2, 1, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 19));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\200\321\202\321\213:", nullptr));
        btnApply->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\320\247\320\265\321\202\320\275\320\276\321\201\321\202\321\214", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\276\320\277-\320\261\320\270\321\202\321\213", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\241\320\272\320\276\321\200\320\276\321\201\321\202\321\214:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\320\221\320\270\321\202\321\213 \320\264\320\260\320\275\320\275\321\213\321\205", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\320\243\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265 \320\277\320\276\321\202\320\276\320\272\320\276\320\274", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
        btnSendData->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\321\214", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 SlaveID", nullptr));
        leRegisterAddress1->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        leRegisterAddress2->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        leDeviceAddress->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x01", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\276\320\262", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\320\220\320\264\321\200\320\265\321\201 \321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\260", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\264 \321\204\321\203\320\275\320\272\321\206\320\270\320\270", nullptr));
        leRegistersQty1->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x00", nullptr));
        leRegistersQty2->setPlaceholderText(QCoreApplication::translate("MainWindow", "0x01", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
