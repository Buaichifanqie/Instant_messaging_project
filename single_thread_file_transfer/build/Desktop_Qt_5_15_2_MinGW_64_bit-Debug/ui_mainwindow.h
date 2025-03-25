/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *ipLb;
    QLineEdit *ipEd;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *portLb;
    QLineEdit *portEd;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *fileLb;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *fileBtn;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *conBtn;
    QPushButton *uploadBtn;
    QPushButton *testBtn;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_7;
    QProgressBar *progressBar;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(320, 479);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        ipLb = new QLabel(centralwidget);
        ipLb->setObjectName(QString::fromUtf8("ipLb"));

        horizontalLayout->addWidget(ipLb);

        ipEd = new QLineEdit(centralwidget);
        ipEd->setObjectName(QString::fromUtf8("ipEd"));

        horizontalLayout->addWidget(ipEd);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        portLb = new QLabel(centralwidget);
        portLb->setObjectName(QString::fromUtf8("portLb"));

        horizontalLayout_2->addWidget(portLb);

        portEd = new QLineEdit(centralwidget);
        portEd->setObjectName(QString::fromUtf8("portEd"));

        horizontalLayout_2->addWidget(portEd);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        fileLb = new QLabel(centralwidget);
        fileLb->setObjectName(QString::fromUtf8("fileLb"));

        horizontalLayout_3->addWidget(fileLb);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        fileBtn = new QPushButton(centralwidget);
        fileBtn->setObjectName(QString::fromUtf8("fileBtn"));

        horizontalLayout_4->addWidget(fileBtn);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        conBtn = new QPushButton(centralwidget);
        conBtn->setObjectName(QString::fromUtf8("conBtn"));

        horizontalLayout_5->addWidget(conBtn);

        uploadBtn = new QPushButton(centralwidget);
        uploadBtn->setObjectName(QString::fromUtf8("uploadBtn"));

        horizontalLayout_5->addWidget(uploadBtn);

        testBtn = new QPushButton(centralwidget);
        testBtn->setObjectName(QString::fromUtf8("testBtn"));

        horizontalLayout_5->addWidget(testBtn);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_6->addWidget(label_4);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        horizontalLayout_7->addWidget(progressBar);


        verticalLayout->addLayout(horizontalLayout_7);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 320, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        ipLb->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250ip:", nullptr));
        portLb->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243:", nullptr));
        fileLb->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\346\226\207\344\273\266\357\274\232", nullptr));
        fileBtn->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\351\234\200\350\246\201\345\217\221\351\200\201\347\232\204\346\226\207\344\273\266", nullptr));
        conBtn->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245", nullptr));
        uploadBtn->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\274\240", nullptr));
        testBtn->setText(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\274\240\350\277\233\345\272\246\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
