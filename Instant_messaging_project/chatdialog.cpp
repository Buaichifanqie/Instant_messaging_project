#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    ui->add_btn->SetState("normal","hover","press");
    ui->search_edit->SetMaxLength(15);

    //实现快速删除的小图标
    QAction* searchAction=new QAction(ui->search_edit);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
