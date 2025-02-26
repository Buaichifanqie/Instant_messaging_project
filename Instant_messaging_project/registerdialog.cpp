#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog),m_countdown(5)
{
    ui->setupUi(this);
    ui->pass_edit1->setEchoMode(QLineEdit::Password);
    ui->confirm_pass_edit->setEchoMode(QLineEdit::Password);
    //设置属性
    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);

    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_reg_mod_finish,this,&RegisterDialog::slot_reg_mod_finish);

    initHttpHandlers();
    ui->err_tip->clear();

    connect(ui->user_edit1,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->pass_edit1, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });
    connect(ui->confirm_pass_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });
    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this](){
        checkVarifyValid();
    });
    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);

    ui->pass_visible->SetState("unvisible","unvisible_hover","","visible",
                               "visible_hover","");
    ui->confirm_visible->SetState("unvisible","unvisible_hover","","visible",
                                  "visible_hover","");


    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->pass_edit1->setEchoMode(QLineEdit::Password);
        }else{
            ui->pass_edit1->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });
    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->confirm_pass_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->confirm_pass_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    // 创建定时器
    m_countdown_timer = new QTimer(this);
    // 连接信号和槽
    connect(m_countdown_timer, &QTimer::timeout, [this](){
        if(m_countdown==0){
            m_countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        m_countdown--;
        auto str = QString("注册成功，%1 s后返回登录").arg(m_countdown);
        ui->tip_lb->setText(str);

    });
}



RegisterDialog::~RegisterDialog()
{
    qDebug()<<"destruct RegisterDialog";
    delete ui;
}

void RegisterDialog::on_get_code_clicked()
{
    auto email=ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match=regex.match(email).hasMatch();
    if(match)
    {
        //发送http验证码
        QJsonObject json_obj;
        json_obj["email"]=email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);
    }
    else
    {
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err!=ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"),false);
        return;
    }
    //解析json 字符串，res 转化为QByteArray
    QJsonDocument jsonDoc=QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull())
    {
        showTip(tr("json解析失败"),false);
        return;
    }
    //json解析错误
    if(!jsonDoc.isObject())
    {
        showTip(tr("json解析失败"),false);
        return;
    }

    //文档转对象
    m_handlers[id](jsonDoc.object());
}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包逻辑
    m_handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"),false);
            return;
        }
        auto email=jsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱请注意查收"),true);
        qDebug()<<"email is"<<email;
    });

    //注册 注册用户回包逻辑
    m_handlers.insert(ReqId::ID_REG_USER,[this](QJsonObject jsonObj){
        int error=jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"),false);
            return;
        }
        auto email=jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug()<<"user uuid is"<<jsonObj["uid"].toString();
        qDebug()<< "email is " << email ;
        ChangeTipPage();
    });
}

void RegisterDialog::showTip(QString str,bool b_ok)
{
    if(b_ok)
    {
        ui->err_tip->setProperty("state","normal");
    }
    else
    {
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}


void RegisterDialog::on_confirm_btn_clicked()
{

    if(ui->user_edit1->text() == ""){
        showTip(tr("用户名不能为空"), false);
        return;
    }
    if(ui->email_edit->text() == ""){
        showTip(tr("邮箱不能为空"), false);
        return;
    }
    if(ui->pass_edit1->text() == ""){
        showTip(tr("密码不能为空"), false);
        return;
    }
    if(ui->confirm_pass_edit->text() == ""){
        showTip(tr("确认密码不能为空"), false);
        return;
    }
    if(ui->confirm_pass_edit->text() != ui->pass_edit1->text()){
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }
    if(ui->varify_edit->text() == ""){
        showTip(tr("验证码不能为空"), false);
        return;
    }
    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit1->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->pass_edit1->text());
    json_obj["confirm"] = xorString(ui->confirm_pass_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    m_tip_errs[te] = tips;
    showTip(tips, false);
}
void RegisterDialog::DelTipErr(TipErr te)
{
    m_tip_errs.remove(te);
    if(m_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }
    showTip(m_tip_errs.first(), false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit1->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->pass_edit1->text();
    auto confirm = ui->confirm_pass_edit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::checkVarifyValid()
{
    auto pass = ui->varify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void RegisterDialog::ChangeTipPage()
{
    m_countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
}

bool RegisterDialog::checkConfirmValid()
{
    auto pass = ui->pass_edit1->text();
    auto confirm = ui->confirm_pass_edit->text();

    if(confirm.length() < 6 || confirm.length() > 15 ){
        //提示长度不准确
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(confirm).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_CONFIRM_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码和密码不匹配"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}



void RegisterDialog::on_pushButton_clicked()
{
    m_countdown_timer->stop();
    emit sigSwitchLogin();
}

