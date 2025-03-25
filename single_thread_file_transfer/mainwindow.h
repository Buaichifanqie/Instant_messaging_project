#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initUi();
    void initSlots();
    //启动延时操作
    void startDelay(int msecs);

private slots:
    void on_fileBtn_clicked();

    void on_testBtn_clicked();

    void on_conBtn_clicked();

    void on_uploadBtn_clicked();

private:
    Ui::MainWindow *ui;
    QString m_file_name;
    QString m_file_md5;
};
#endif // MAINWINDOW_H
