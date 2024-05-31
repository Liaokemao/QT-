#include "mainwindow.h"
#include "ui_mainwindow.h" // 包含 Ui::MainWindow 的定义
#include "fistpage.h"
#include <QApplication>
#include "secondpage.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fistpage v; // 创建 fistpage 对象
    v.show();   // 显示 fistpage 界面
    return a.exec();
}
