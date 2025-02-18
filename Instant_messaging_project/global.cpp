#include "global.h"

// 定义一个 std::function 对象 repolish，用于重新应用样式表到 QWidget
// 该函数接受一个 QWidget 指针作为参数
std::function<void(QWidget*)> repolish = [](QWidget* w) {
    // 1. 取消当前样式表的应用
    // unpolish() 会移除 QWidget 上当前应用的样式
    w->style()->unpolish(w);

    // 2. 重新应用样式表
    // polish() 会重新应用样式表到 QWidget
    w->style()->polish(w);
};
