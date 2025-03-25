#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QLinearGradient>
#include "mainwindow.h"

// class GradientWidget : public QWidget {
// public:
//     GradientWidget(QWidget *parent = nullptr) : QWidget(parent) {}

// protected:
//     void paintEvent(QPaintEvent *event) override {
//         Q_UNUSED(event);

//         QPainter painter(this);

//         // 创建线性渐变
//         QLinearGradient gradient(0, 0, width(), height());
//         gradient.setColorAt(0, Qt::black);  // 起始颜色
//         gradient.setColorAt(1, Qt::white); // 结束颜色

//         // //径向渐变
//         // QRadialGradient gradient(width() / 2, height() / 2, width() / 2);
//         // gradient.setColorAt(0, Qt::blue);
//         // gradient.setColorAt(1, Qt::green);
//         // painter.fillRect(rect(), gradient);

//         // 设置渐变色为背景
//         painter.fillRect(rect(), gradient);
//     }

// };

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     GradientWidget widget;
//     widget.resize(400, 300);
//     widget.show();

//     return app.exec();
// }


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
