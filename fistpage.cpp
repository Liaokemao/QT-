#include "fistpage.h"
#include "ui_fistpage.h"
#include "mainwindow.h"
#include "secondpage.h"
#include "qpushbutton.h"
fistpage::fistpage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::fistpage)
{
    ui->setupUi(this);

    // 加载原始背景图片
    QPixmap original(":/beijingtu.jpg");

    // 创建透明背景图片并填充为透明
    QPixmap transparent(original.size());
    transparent.fill(Qt::transparent);

    // 创建 painter 并在透明背景图片上绘制原始背景图片
    QPainter painter(&transparent);
    painter.setOpacity(0.6); // 设置透明度为60%
    painter.drawPixmap(0, 0, original);

    // 设置窗口的背景为处理后的透明背景图片
    QPalette palette;
    palette.setBrush(QPalette::Background, transparent);
    this->setPalette(palette);

    // 创建 MainWindow 对象并添加到当前窗口
    MainWindow *page2 = new MainWindow(this); // 使用 this 作为父对象
    connect(ui->button1, &QPushButton::clicked, [=](){
        this->hide();
        page2->show();
    });
    secondpage *page3 = new secondpage(this); // 使用 this 作为父对象
    connect(ui->button2, &QPushButton::clicked, [=](){
        this->hide();
        page3->show();
    });
    connect(page3, &secondpage::backfirst, [=]() {
        page3->hide();
        this->show();
    });
    connect(page2, &MainWindow::backone, [=]() {
        page2->hide();
        this->show();
    });
    // 设置按钮颜色为淡蓝色
    ui->button1->setStyleSheet("background-color: lightblue;");
    ui->button2->setStyleSheet("background-color: lightblue;");

}

fistpage::~fistpage()
{
    delete ui;
}

