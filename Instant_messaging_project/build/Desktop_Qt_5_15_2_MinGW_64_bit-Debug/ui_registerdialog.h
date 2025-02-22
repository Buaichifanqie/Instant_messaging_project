/********************************************************************************
** Form generated from reading UI file 'registerdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERDIALOG_H
#define UI_REGISTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegisterDialog
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_7;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_2;
    QLabel *err_tip;
    QSpacerItem *horizontalSpacer;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_4;
    QWidget *horizontalWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *user_label1;
    QLineEdit *user_edit1;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *email_label;
    QLineEdit *email_edit;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *pass_label1;
    QLineEdit *pass_edit1;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *confirm_pass_label;
    QLineEdit *confirm_pass_edit;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *varify_label;
    QLineEdit *varify_edit;
    QPushButton *get_code;
    QSpacerItem *verticalSpacer_5;
    QWidget *widget_6;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *confirm_btn;
    QPushButton *cancel_btn;
    QSpacerItem *verticalSpacer_3;
    QWidget *page_2;

    void setupUi(QDialog *RegisterDialog)
    {
        if (RegisterDialog->objectName().isEmpty())
            RegisterDialog->setObjectName(QString::fromUtf8("RegisterDialog"));
        RegisterDialog->resize(300, 500);
        RegisterDialog->setMinimumSize(QSize(300, 500));
        RegisterDialog->setMaximumSize(QSize(300, 500));
        verticalLayout = new QVBoxLayout(RegisterDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        widget = new QWidget(RegisterDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        widget_7 = new QWidget(widget);
        widget_7->setObjectName(QString::fromUtf8("widget_7"));
        horizontalLayout_7 = new QHBoxLayout(widget_7);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalSpacer_2 = new QSpacerItem(77, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_2);

        err_tip = new QLabel(widget_7);
        err_tip->setObjectName(QString::fromUtf8("err_tip"));
        err_tip->setAlignment(Qt::AlignCenter);

        horizontalLayout_7->addWidget(err_tip);

        horizontalSpacer = new QSpacerItem(77, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);


        verticalLayout_2->addWidget(widget_7);

        stackedWidget = new QStackedWidget(widget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        verticalLayout_3 = new QVBoxLayout(page);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        horizontalWidget = new QWidget(page);
        horizontalWidget->setObjectName(QString::fromUtf8("horizontalWidget"));
        horizontalLayout = new QHBoxLayout(horizontalWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        user_label1 = new QLabel(horizontalWidget);
        user_label1->setObjectName(QString::fromUtf8("user_label1"));
        user_label1->setMinimumSize(QSize(0, 25));
        user_label1->setMaximumSize(QSize(16777215, 25));

        horizontalLayout->addWidget(user_label1);

        user_edit1 = new QLineEdit(horizontalWidget);
        user_edit1->setObjectName(QString::fromUtf8("user_edit1"));
        user_edit1->setMinimumSize(QSize(0, 25));
        user_edit1->setMaximumSize(QSize(16777215, 25));

        horizontalLayout->addWidget(user_edit1);


        verticalLayout_3->addWidget(horizontalWidget);

        widget_2 = new QWidget(page);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        email_label = new QLabel(widget_2);
        email_label->setObjectName(QString::fromUtf8("email_label"));
        email_label->setMinimumSize(QSize(0, 25));
        email_label->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_2->addWidget(email_label);

        email_edit = new QLineEdit(widget_2);
        email_edit->setObjectName(QString::fromUtf8("email_edit"));
        email_edit->setMinimumSize(QSize(0, 25));
        email_edit->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_2->addWidget(email_edit);


        verticalLayout_3->addWidget(widget_2);

        widget_3 = new QWidget(page);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pass_label1 = new QLabel(widget_3);
        pass_label1->setObjectName(QString::fromUtf8("pass_label1"));
        pass_label1->setMinimumSize(QSize(0, 25));
        pass_label1->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_3->addWidget(pass_label1);

        pass_edit1 = new QLineEdit(widget_3);
        pass_edit1->setObjectName(QString::fromUtf8("pass_edit1"));
        pass_edit1->setMinimumSize(QSize(0, 25));
        pass_edit1->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_3->addWidget(pass_edit1);


        verticalLayout_3->addWidget(widget_3);

        widget_4 = new QWidget(page);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        horizontalLayout_4 = new QHBoxLayout(widget_4);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        confirm_pass_label = new QLabel(widget_4);
        confirm_pass_label->setObjectName(QString::fromUtf8("confirm_pass_label"));
        confirm_pass_label->setMinimumSize(QSize(0, 25));
        confirm_pass_label->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_4->addWidget(confirm_pass_label);

        confirm_pass_edit = new QLineEdit(widget_4);
        confirm_pass_edit->setObjectName(QString::fromUtf8("confirm_pass_edit"));
        confirm_pass_edit->setMinimumSize(QSize(0, 25));
        confirm_pass_edit->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_4->addWidget(confirm_pass_edit);


        verticalLayout_3->addWidget(widget_4);

        widget_5 = new QWidget(page);
        widget_5->setObjectName(QString::fromUtf8("widget_5"));
        horizontalLayout_5 = new QHBoxLayout(widget_5);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        varify_label = new QLabel(widget_5);
        varify_label->setObjectName(QString::fromUtf8("varify_label"));
        varify_label->setMinimumSize(QSize(0, 25));
        varify_label->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_5->addWidget(varify_label);

        varify_edit = new QLineEdit(widget_5);
        varify_edit->setObjectName(QString::fromUtf8("varify_edit"));
        varify_edit->setMinimumSize(QSize(0, 25));
        varify_edit->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_5->addWidget(varify_edit);

        get_code = new QPushButton(widget_5);
        get_code->setObjectName(QString::fromUtf8("get_code"));
        get_code->setMinimumSize(QSize(0, 25));
        get_code->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_5->addWidget(get_code);


        verticalLayout_3->addWidget(widget_5);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_5);

        widget_6 = new QWidget(page);
        widget_6->setObjectName(QString::fromUtf8("widget_6"));
        horizontalLayout_6 = new QHBoxLayout(widget_6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        confirm_btn = new QPushButton(widget_6);
        confirm_btn->setObjectName(QString::fromUtf8("confirm_btn"));
        confirm_btn->setMinimumSize(QSize(0, 25));
        confirm_btn->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_6->addWidget(confirm_btn);

        cancel_btn = new QPushButton(widget_6);
        cancel_btn->setObjectName(QString::fromUtf8("cancel_btn"));
        cancel_btn->setMinimumSize(QSize(0, 25));
        cancel_btn->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_6->addWidget(cancel_btn);


        verticalLayout_3->addWidget(widget_6);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        stackedWidget->addWidget(page_2);

        verticalLayout_2->addWidget(stackedWidget);


        verticalLayout->addWidget(widget);


        retranslateUi(RegisterDialog);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(RegisterDialog);
    } // setupUi

    void retranslateUi(QDialog *RegisterDialog)
    {
        RegisterDialog->setWindowTitle(QCoreApplication::translate("RegisterDialog", "Dialog", nullptr));
        err_tip->setText(QCoreApplication::translate("RegisterDialog", "\351\224\231\350\257\257\346\217\220\347\244\272", nullptr));
        user_label1->setText(QCoreApplication::translate("RegisterDialog", "\347\224\250\346\210\267:", nullptr));
        email_label->setText(QCoreApplication::translate("RegisterDialog", "\351\202\256\347\256\261:", nullptr));
        pass_label1->setText(QCoreApplication::translate("RegisterDialog", "\345\257\206\347\240\201:", nullptr));
        confirm_pass_label->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\350\256\244\345\257\206\347\240\201:", nullptr));
        varify_label->setText(QCoreApplication::translate("RegisterDialog", "\351\252\214\350\257\201\347\240\201:", nullptr));
        get_code->setText(QCoreApplication::translate("RegisterDialog", "\350\216\267\345\217\226\351\252\214\350\257\201\347\240\201", nullptr));
        confirm_btn->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\350\256\244", nullptr));
        cancel_btn->setText(QCoreApplication::translate("RegisterDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegisterDialog: public Ui_RegisterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERDIALOG_H
