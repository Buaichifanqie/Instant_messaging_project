#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initUI();
    void initSlots();
    void startDelay(int msecs);
private slots:
    void on_fileBtn_clicked();

    void on_testBtn_clicked();

    void on_conBtn_clicked();

    void on_uploadBtn_clicked();

private:
    Ui::MainWindow *ui;
    QString _file_name;
    QString _file_md5;
};

#endif // MAINWINDOW_H
