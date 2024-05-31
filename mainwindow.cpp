// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <vector>
#include <QCoreApplication>
#include <cmath>
#include <QMessageBox>
#include <QInputDialog>
#include <QRegularExpression>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QTimer>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      serial(new QSerialPort(this))
{
    ui->setupUi(this);
    // 找到速度和角度滑块控件
    QSlider *speedSlider = ui->verticalSlider;
    QSlider *angleSlider = ui->horizontalSlider;

    // 设置速度和角度滑块的属性
    if (speedSlider && angleSlider) {
        speedSlider->setRange(-10, 10);

        angleSlider->setRange(-10, 10);
        // 滑块的样式表
        speedSlider->setStyleSheet("QSlider::groove:vertical {"
                                   "border: 1px solid #999999;"
                                   "background: white;"
                                   "width: 20px; /* 设置宽度 */"
                                   "margin: 0px 2px;"
                                   "}"
                                   "QSlider::handle:vertical {"
                                   "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                                   "stop:0 #eeeeee, stop:1 #cccccc);"
                                   "border: 1px solid #5c5c5c;"
                                   "width: 20px; /* 设置宽度 */"
                                   "border-radius: 9px;"
                                   "margin: -10px -4px; /* 设置位置 */"
                                   "}");

        angleSlider->setStyleSheet("QSlider::groove:horizontal {"
                                   "border: 1px solid #999999;"
                                   "background: white;"
                                   "height: 20px; /* 设置高度 */"
                                   "margin: 2px 0px;"
                                   "}"
                                   "QSlider::handle:horizontal {"
                                   "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                                   "stop:0 #eeeeee, stop:1 #cccccc);"
                                   "border: 1px solid #5c5c5c;"
                                   "height: 20px; /* 设置高度 */"
                                   "border-radius: 9px;"
                                   "margin: -4px -10px; /* 设置位置 */"
                                   "}");

        // 连接滑块数值变化的信号到槽函数
        connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::updateCoordinates);
        connect(angleSlider, &QSlider::valueChanged, this, &MainWindow::updateCoordinates);

        // 连接滑块松开事件到槽函数
        //connect(angleSlider, &QSlider::sliderReleased, this, &MainWindow::resetAngleSlider);
    }
    // 在这里设置 w.ui-> 的属性
    ui->connectionButton->setStyleSheet("QPushButton {""background-color: #FAF0E6;""color: black;""text-align: center;""}");
    ui->disconnectionButton->setStyleSheet("QPushButton {""background-color: #FAF0E6;""color: black;""text-align: center;""}");
    ui->currentcondition_status->setStyleSheet("background-color: #FAF0E6; color: black;");
    ui->currentcondition_label->setStyleSheet("background-color: #FAF0E6; color: black;");
    ui->graphicsView->setStyleSheet("background-color: #FAF0E6;");
    Init();

    ui->velprogressBar->setValue(20);
    ui->velprogressBar_2->setValue(0);
    // 创建一个 QPalette 对象，并获取当前窗口的调色板
    QPalette palette(this->palette());

    // 使用整数参数设置淡蓝色背景
    palette.setColor(QPalette::Background, QColor(173, 213, 224)); // 使用 RGB 值设置淡蓝色背景

    // 将修改后的调色板应用到窗口
    this->setPalette(palette);

    // 连接按钮的点击事件到槽函数
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
    connect(ui->modeshiftComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_modeshiftComboBox_currentIndexChanged(QString)));
    // 连接accButton按钮的clicked()信号到自定义的槽函数
    connect(ui->accButton, &QPushButton::clicked, this, &MainWindow::onAccButtonClicked);
    // 连接decButton按钮的clicked()信号到自定义的槽函数
    connect(ui->decButton, &QPushButton::clicked, this, &MainWindow::onDecButtonClicked);
    connect(ui->recordbutton, &QPushButton::clicked, this, &MainWindow::recordbuttonclicked);
    connect(ui->recordbutton_2, &QPushButton::clicked, this, &MainWindow::recordbutton_2clicked);
    connect(ui->recordbutton_3, &QPushButton::clicked, this, &MainWindow::recordbutton_3clicked);
    connect(ui->recordbutton_4, &QPushButton::clicked, this, &MainWindow::recordbutton_4clicked);
    connect(ui->recordbutton_5, &QPushButton::clicked, this, &MainWindow::recordbutton_5clicked);
    connect(ui->button4, &QPushButton::clicked, [=](){emit this->backone();});
    // 连接 ComboBox_1 的 currentIndexChanged 信号到槽函数
    connect(ui->ComboBox_1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBox1IndexChanged);
    connect(ui->ComboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBox2IndexChanged);
    connect(ui->ComboBox_3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBox3IndexChanged);
    connect(ui->ComboBox_4, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBox4IndexChanged);
    connect(ui->ComboBox_5, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBox5IndexChanged);
    connect(ui->ComboBox_6, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBox6IndexChanged);
    // 创建图形场景
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    // 从LoRa接收到的数据
    std::vector<QPointF> crackData; // 曲线的点
    std::vector<double> positionData; // 当前位置数据
    //int statusData = 0; // 当前状态数据
    // 调用 Init 函数进行初始化
    Init();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::Init()
{

    //设置窗口固定大小
    this->setFixedSize(1280,800);


}
void MainWindow::on_connectionButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "连接串口", "确定要连接串口吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
            if (!serial->isOpen()) {
                serial->setPortName("/dev/ttyUSB0");
                serial->setBaudRate(QSerialPort::Baud9600);  // 替换为波特率
                if (serial->open(QIODevice::ReadWrite)) {
                    ui->connection_status->setText("串口已连接");
                } else {
                    qDebug() << "打开串口失败!";
                    ui->connection_status->setText("串口连接失败");
                }
        } else {
            QMessageBox::critical(this, "错误", "密码错误！");
        }
    }
}


void MainWindow::on_disconnectionButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "断开连接", "确定要断开通讯吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (serial->isOpen()) {
            serial->close();  // 如果串口已打开，则关闭串口
            ui->connection_status->setText("串口已断开");
        }
    }
}

void MainWindow::on_modeshiftComboBox_currentIndexChanged(const QString &mode)
{
    // 断开当前模式选择框的信号连接，以防止循环调用
    disconnect(ui->modeshiftComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(on_modeshiftComboBox_currentIndexChanged(const QString &)));

    // 弹出对话框询问用户是否要切换模式
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "切换模式", "确定要切换到 " + mode + " 模式吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 用户选择切换模式，更新当前模式，并发送相应数据到串口
        currentMode = mode;
        ui->currentmode_status->setText(mode);
        qDebug() << "模式切换到：" << mode;
        if (mode == "手动模式") {
            QByteArray data = "1111";
            serial->write(data);
            ui->ComboBox_1->setEnabled(true);
            ui->ComboBox_2->setEnabled(true);
            ui->ComboBox_3->setEnabled(true);
            ui->ComboBox_4->setEnabled(true);
            ui->ComboBox_5->setEnabled(true);
            ui->ComboBox_6->setEnabled(true);
        } else if (mode == "自动模式") {
            QByteArray data = "2222";
            serial->write(data);
            ui->ComboBox_1->setEnabled(false);
            ui->ComboBox_2->setEnabled(false);
            ui->ComboBox_3->setEnabled(false);
            ui->ComboBox_4->setEnabled(false);
            ui->ComboBox_5->setEnabled(false);
            ui->ComboBox_6->setEnabled(false);
        } else if (mode == "随动模式") {
            QByteArray data = "3333";
            serial->write(data);
            ui->ComboBox_1->setEnabled(false);
            ui->ComboBox_2->setEnabled(false);
            ui->ComboBox_3->setEnabled(false);
            ui->ComboBox_4->setEnabled(false);
            ui->ComboBox_5->setEnabled(false);
            ui->ComboBox_6->setEnabled(false);
        }else if (mode == "仅补缝模式") {
            QByteArray data = "4444";
            serial->write(data);
            ui->ComboBox_1->setEnabled(false);
            ui->ComboBox_2->setEnabled(false);
            ui->ComboBox_3->setEnabled(false);
            ui->ComboBox_4->setEnabled(false);
            ui->ComboBox_5->setEnabled(false);
            ui->ComboBox_6->setEnabled(false);
        }
    } else {
        // 用户选择不切换模式，将下拉框重新设置为之前的选项
        ui->modeshiftComboBox->setCurrentText(ui->currentmode_status->text());
    }

    // 重新连接当前模式选择框的信号连接
    connect(ui->modeshiftComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_modeshiftComboBox_currentIndexChanged(QString)));
}


void MainWindow::readSerialData()
{

    static QString receivedBuffer; // 用于缓存未完整接收的数据
    static QFile file; // 文件对象
    // 清空接收缓冲区
    receivedBuffer.clear();
    // 检查文件是否已打开，若未打开则打开文件
    if (!file.isOpen()) {
        // 获取当前日期时间
        QDateTime currentDateTime = QDateTime::currentDateTime();
        // 根据当前日期时间构建文件名
       QString fileName = QString("/home/passoni/qt_record/%1.csv").arg(currentDateTime.toString("yyyy-MM-dd-hh-mm"));
        file.setFileName(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qDebug() << "Failed to open CSV file for writing:" << file.errorString();
            return;
        }
        qDebug() << "CSV file opened for writing.";
    }
    QByteArray data = serial->readAll(); // 从串口读取所有可用数据
    QString receivedData = QString::fromUtf8(data);
    qDebug() << "Received serial data:" << receivedData;
    QTextStream out(&file);
    out << data << "\n"; // 将接收到的原始数据写入文件，每条数据一行

    // 刷新文件流
    out.flush();
    // 将新接收到的数据与缓存中的数据拼接
    receivedBuffer += receivedData;
    // 检查是否接收到完整的消息
    if (!receivedBuffer.endsWith(';')) {
        return; // 没有接收到完整的消息，等待下一次数据到来
    }
    // 解析接收到的字符串数据
    QStringList dataList = receivedBuffer.split(";");
    QMap<QString, QString> valuesMap;

    for (const QString &item : dataList) {
        QStringList parts = item.split(":");
        if (parts.size() == 2) {
            QString key = parts[0].trimmed();
            QString value = parts[1].trimmed();
            valuesMap[key] = value;
        }
    }

    // 在接收数据的地方直接使用QVector<QPointF>存储数据
    QStringList crackDataValues;
    QVector<QPointF> crackDataPoints;
    // 处理接收到的数据并添加到 crackDataPoints 容器中
    if (valuesMap.contains("crack")) {
        QString crackData = valuesMap["crack"];

        // 解析数据点
        QStringList point = crackData.split(",");
        if (point.size() == 2) {
            qreal x = point[0].toDouble() / 2; // 将 x 坐标除以 10
            qreal y = point[1].toDouble() / 2; // 将 y 坐标除以 10
            crackDataPoints.push_back(QPointF(x, y));
        }
    }
    handleCrackData(ui, crackDataPoints);
    qDebug() << "Crack data points:" << crackDataPoints;
    // 处理 status_data 数据
    if (valuesMap.contains("status")) {
        int statusData = valuesMap["status"].toInt();
        qDebug() << "Status data:" << statusData;
        handleStatusData(statusData);
    }
    /*
    // 处理 temperature 数据
    if (valuesMap.contains("temperature")) {
        double temperature = valuesMap["temperature"].toDouble();
        qDebug() << "Temperature:" << temperature;
        updateTemperature(temperature);
    }
    */
    // 处理 zhongliang 数据
    if (valuesMap.contains("zhongliang")) {
        double zhongliang = valuesMap["zhongliang"].toDouble();
        qDebug() << "zhongliang:" << zhongliang;
        updatezhongliang(zhongliang);
    }
    // 处理 position_data 数据
    if (valuesMap.contains("pos")) {
        int positionData = valuesMap["pos"].toInt();
        qDebug() << "Position data:" << positionData;
        handlePositionData(positionData);
    }
    // 处理 chaochufanwei 数据
    if (valuesMap.contains("chaochufanwei")) {
        // 检查超出范围的值
        int chaochufanwei = valuesMap["chaochufanwei"].toInt();

        if (chaochufanwei == 1) {
            // 发出警告提示
            QMessageBox::warning(this, "警告", "当前位置已经超出电子围栏范围，请回手动引导小车到安全区域！");
        }
    }
    if (valuesMap.contains("wuliefeng")) {
        // 检查超出范围的值
        int wuliefeng = valuesMap["wuliefeng"].toInt();

        if (wuliefeng == 1) {
            // 发出提示
            QMessageBox::warning(this, "提示", "当前范围内寻找不到裂缝，请人工引导到摄像头可视区域");
        }
    }
    if (valuesMap.contains("zhaodaoliefeng")) {
        // 判断值是否为 1
        int zhaodaoliefeng = valuesMap["zhaodaoliefeng"].toInt();
        if (zhaodaoliefeng == 1) {
            // 发出提示
            QMessageBox::warning(this, "提示", "找到裂缝，可切换到自动导航模式");
        }
    }

    // 清空接收缓冲区
    receivedBuffer.clear();
}
int positionData;
static QVector<QPointF> allCrackDataPoints;
// 在 handleCrackData 函数中设置 crackData 数据
void MainWindow::handleCrackData(Ui::MainWindow *ui, const QVector<QPointF> &crackDataPoints)
{
    // 创建一个持久的容器来存储所有的数据点
    static QVector<QPointF> allCrackDataPoints;
    // 将新的数据点与之前的数据点合并
    allCrackDataPoints += crackDataPoints;

    // 当数据点数量达到阈值时再进行绘制
    if (allCrackDataPoints.size() == 30) {
        // 创建一个 QGraphicsScene 对象
        QGraphicsScene *scene = new QGraphicsScene;
        // 设置点的大小
        int pointSize = 7;
        // 添加所有数据点作为矩形显示
        for (const QPointF &point : allCrackDataPoints) {
            scene->addRect(point.x() - pointSize / 2, point.y() - pointSize / 2, pointSize, pointSize, QPen(Qt::black), QBrush(Qt::black));
        }
        // 绘制连接所有数据点的线条
        for (int i = 1; i < allCrackDataPoints.size(); ++i) {
            scene->addLine(allCrackDataPoints[i - 1].x(), allCrackDataPoints[i - 1].y(), allCrackDataPoints[i].x(), allCrackDataPoints[i].y(), QPen(Qt::blue));
        }

        // 保存上一次的场景
        QGraphicsScene *previousScene = ui->graphicsView->scene();
        // 设置新场景到 graphicsView 中
        ui->graphicsView->setScene(scene);
        // 删除上一次的场景
        if (previousScene && previousScene != scene) {
            delete previousScene;
        }
    }

    if (allCrackDataPoints.size() > 30) {
        ui->graphicsView->scene()->clear(); // 清空 graphicsView
        allCrackDataPoints.clear(); // 清空数据点容器
        allCrackDataPoints += crackDataPoints;
        //return; // 终止函数执行，避免在添加新场景时继续执行下面的代码
    }
}

// 在 handlePositionData 函数中使用 positionData 成员变量
void MainWindow::handlePositionData(int positionData)
{
    int progressBarValue = (positionData * 10000) / 1000;
    // 更新进度条的值
    ui->velprogressBar_2->setValue(progressBarValue);
}
void MainWindow::handleStatusData(int statusData)
{
    Q_UNUSED(statusData);

    // 检查状态数据是否为无效值
    if (statusData != 0 && statusData != 1) {
        // 如果收到的状态数据无效，显示 "Invalid status data"
        qDebug() << "Invalid status data:" << statusData;
        ui->currentcondition_status->setText("Invalid status data");
    } else {
        // 更新状态文本
        if (statusData == 1) {
            ui->currentcondition_status->setText("正在进行补缝...");
        } else {
            ui->currentcondition_status->setText("正在进行开槽...");
        }
    }

}
/*
void MainWindow::updateTemperature(double temperature)
{
    // 将温度值转换为带有"°C"单位的字符串
    QString temperatureText = QString::number(temperature) + "°C";

    // 将带有单位的温度字符串设置为文本框的显示内容
    ui->status00->setText(temperatureText);
}
*/
void MainWindow::updatezhongliang(double zhongliang)
{
    // 将温度值转换为带有"°C"单位的字符串
    QString zhongliangText = QString::number(zhongliang) + "KG";

    // 将带有单位的温度字符串设置为文本框的显示内容
    ui->status00_2->setText(zhongliangText);
}
// 自定义的槽函数，当accButton按钮点击时调用
void MainWindow::onAccButtonClicked()
{
    if (ui->modeshiftComboBox->currentText() == "手动模式")
    {
        // 增加进度条的值，每次增加10%
        int newValue = ui->velprogressBar->value() + 20;
        // 确保不超过最大值
        newValue = qMin(newValue, ui->velprogressBar->maximum());
        // 更新进度条的值
        ui->velprogressBar->setValue(newValue);
        // 如果达到最大值，禁用accButton按钮
        if (newValue == ui->velprogressBar->maximum())
        {
            ui->accButton->setEnabled(false);
        }
        // 启用decButton按钮
        ui->decButton->setEnabled(true);
        // 向串口发送数据 "77"
        QString dataString = QString("%1").arg(7777);
        QByteArray data = dataString.toUtf8();
        serial->write(data);
    }
}

// 自定义的槽函数，当decButton按钮点击时调用
void MainWindow::onDecButtonClicked()
{
    if (ui->modeshiftComboBox->currentText() == "手动模式")
    {
        // 减少进度条的值，每次减少10%
        int newValue = ui->velprogressBar->value() - 20;
        // 确保不低于最小值
        newValue = qMax(newValue, ui->velprogressBar->minimum());
        // 更新进度条的值
        ui->velprogressBar->setValue(newValue);
        // 启用accButton按钮
        ui->accButton->setEnabled(true);
        // 如果到达了最小值，禁用decButton按钮
        if (newValue == ui->velprogressBar->minimum())
        {
            ui->decButton->setEnabled(false);
        }
        else
        {
            // 启用decButton按钮
            ui->decButton->setEnabled(true);
        }
        // 向串口发送数据 "99"
        QString dataString = "9999";
        QByteArray data = dataString.toUtf8();
        serial->write(data);
    }
}
void MainWindow::recordbuttonclicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "保存记录", "确定要保存记录吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString dataString = "8888"; // 这里是你要发送的消息
        QByteArray data = dataString.toUtf8();
        serial->write(data); // 发送消息给串口

        // 处理串口数据并生成 CSV 文件
        //processSerialData(dataString);
    }
}
void MainWindow::recordbutton_2clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "关机", "确定要关机吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString dataString = "5555"; // 这里是你要发送的消息
        QByteArray data = dataString.toUtf8();
        serial->write(data); // 发送消息给串口

        // 处理串口数据并生成 CSV 文件
        //processSerialData(dataString);
    }
}
void MainWindow::recordbutton_3clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "顺时针旋转", "确定要顺时针旋转吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString dataString = "shunshizhen"; // 这里是你要发送的消息
        QByteArray data = dataString.toUtf8();
        serial->write(data); // 发送消息给串口
    }
}
void MainWindow::recordbutton_4clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "逆时针旋转", "确定要逆时针旋转吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString dataString = "nishizhen"; // 这里是你要发送的消息
        QByteArray data = dataString.toUtf8();
        serial->write(data); // 发送消息给串口
    }
}
void MainWindow::recordbutton_5clicked()
{
        QString dataString = "tingzhi"; // 这里是你要发送的消息
        QByteArray data = dataString.toUtf8();
        serial->write(data); // 发送消息给串口
}




void MainWindow::onComboBox1IndexChanged(int index) {
if (ui->modeshiftComboBox->currentText() == "手动模式")
{
    QString action;
    if (index == 0) {
        action = "等待";
    } else if (index == 1) {
        action = "打开";
    } else if (index == 2) {
        action = "关闭";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作", "确定要" + action + "吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 用户确认操作，执行相应的逻辑
        QLabel* label = ui->label11; // 获取 label11 指针
        ui->label11->setFixedSize(50, 50);

        // 根据 ComboBox 的选择更新 QLabel 的颜色
        switch(index) {
            case 0: // "等待"
                label->setStyleSheet("background-color: yellow; border-radius: 25px;"); // 设置背景
                break;
            case 1: // "打开"
                label->setStyleSheet("background-color: green; border-radius: 25px;"); // 设置绿色背景
                if (serial->isOpen()) {
                    QByteArray data("1001"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据1001！";
                }
                break;
            case 2: // "关闭"
                label->setStyleSheet("background-color: red; border-radius: 25px;"); // 设置红色背景
                if (serial->isOpen()) {
                    QByteArray data("1000"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据1000！";
                }
                break;
        }
    } else {
        // 用户取消操作，将 ComboBox 的当前选择重新设置为之前的选项
        ui->ComboBox_1->blockSignals(true); // 阻止信号槽循环调用
        ui->ComboBox_1->setCurrentIndex(index); // 设置为之前的选项
        ui->ComboBox_1->blockSignals(false); // 解除信号槽阻止
    }
}
}
void MainWindow::onComboBox2IndexChanged(int index) {
if (ui->modeshiftComboBox->currentText() == "手动模式")
{
    QString action;
    if (index == 0) {
        action = "等待";
    } else if (index == 1) {
        action = "打开";
    } else if (index == 2) {
        action = "关闭";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作", "确定要" + action + "吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 用户确认操作，执行相应的逻辑
        QLabel* label = ui->label22; // 获取 label22 指针
        ui->label22->setFixedSize(50, 50);

        // 根据 ComboBox 的选择更新 QLabel 的颜色
        switch(index) {
            case 0: // "等待"
                label->setStyleSheet("background-color: yellow; border-radius: 25px;"); // 设置背景
                break;
            case 1: // "打开"
                label->setStyleSheet("background-color: green; border-radius: 25px;"); // 设置绿色背景
                if (serial->isOpen()) {
                    QByteArray data("2001"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据2001！";
                }
                break;
            case 2: // "关闭"
                label->setStyleSheet("background-color: red; border-radius: 25px;"); // 设置红色背景
                if (serial->isOpen()) {
                    QByteArray data("2000"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据2000！";
                }
                break;
        }
    } else {
        // 用户取消操作，将 ComboBox 的当前选择重新设置为之前的选项
        ui->ComboBox_2->blockSignals(true); // 阻止信号槽循环调用
        ui->ComboBox_2->setCurrentIndex(index); // 设置为之前的选项
        ui->ComboBox_2->blockSignals(false); // 解除信号槽阻止
    }
}
}
void MainWindow::onComboBox3IndexChanged(int index) {
if (ui->modeshiftComboBox->currentText() == "手动模式")
{
    QString action;
    if (index == 0) {
        action = "等待";
    } else if (index == 1) {
        action = "打开";
    } else if (index == 2) {
        action = "关闭";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作", "确定要" + action + "吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 用户确认操作，执行相应的逻辑
        QLabel* label = ui->label33; // 获取 label33 指针
        ui->label33->setFixedSize(50, 50);

        // 根据 ComboBox 的选择更新 QLabel 的颜色
        switch(index) {
            case 0: // "等待"
                label->setStyleSheet("background-color: yellow; border-radius: 25px;"); // 设置背景
                break;
            case 1: // "打开"
                label->setStyleSheet("background-color: green; border-radius: 25px;"); // 设置绿色背景
                if (serial->isOpen()) {
                    QByteArray data("3001"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据3001！";
                }
                break;
            case 2: // "关闭"
                label->setStyleSheet("background-color: red; border-radius: 25px;"); // 设置红色背景
                if (serial->isOpen()) {
                    QByteArray data("3000"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据3000！";
                }
                break;
        }
    } else {
        // 用户取消操作，将 ComboBox 的当前选择重新设置为之前的选项
        ui->ComboBox_3->blockSignals(true); // 阻止信号槽循环调用
        ui->ComboBox_3->setCurrentIndex(index); // 设置为之前的选项
        ui->ComboBox_3->blockSignals(false); // 解除信号槽阻止
    }
}
}
void MainWindow::onComboBox4IndexChanged(int index) {
if (ui->modeshiftComboBox->currentText() == "手动模式")
{
    QString action;
    if (index == 0) {
        action = "等待";
    } else if (index == 1) {
        action = "打开";
    } else if (index == 2) {
        action = "关闭";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作", "确定要" + action + "吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 用户确认操作，执行相应的逻辑
        QLabel* label = ui->label44; // 获取 label44 指针
        ui->label44->setFixedSize(50, 50);

        // 根据 ComboBox 的选择更新 QLabel 的颜色
        switch(index) {
            case 0: // "等待"
                label->setStyleSheet("background-color: yellow; border-radius: 25px;"); // 设置背景
                break;
            case 1: // "打开"
                label->setStyleSheet("background-color: green; border-radius: 25px;"); // 设置绿色背景
                if (serial->isOpen()) {
                    QByteArray data("4001"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据4001！";
                }
                break;
            case 2: // "关闭"
                label->setStyleSheet("background-color: red; border-radius: 25px;"); // 设置红色背景
                if (serial->isOpen()) {
                    QByteArray data("4000"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据4000！";
                }
                break;
        }
    } else {
        // 用户取消操作，将 ComboBox 的当前选择重新设置为之前的选项
        ui->ComboBox_4->blockSignals(true); // 阻止信号槽循环调用
        ui->ComboBox_4->setCurrentIndex(index); // 设置为之前的选项
        ui->ComboBox_4->blockSignals(false); // 解除信号槽阻止
    }
}
}
void MainWindow::onComboBox5IndexChanged(int index) {
if (ui->modeshiftComboBox->currentText() == "手动模式")
{
    QString action;
    if (index == 0) {
        action = "等待";
    } else if (index == 1) {
        action = "打开";
    } else if (index == 2) {
        action = "关闭";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作", "确定要" + action + "吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 用户确认操作，执行相应的逻辑
        QLabel* label = ui->label55; // 获取 label55 指针
        ui->label55->setFixedSize(50, 50);

        // 根据 ComboBox 的选择更新 QLabel 的颜色
        switch(index) {
            case 0: // "等待"
                label->setStyleSheet("background-color: yellow; border-radius: 25px;"); // 设置背景
                break;
            case 1: // "打开"
                label->setStyleSheet("background-color: green; border-radius: 25px;"); // 设置绿色背景
                if (serial->isOpen()) {
                    QByteArray data("5001"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据5001！";
                }
                break;
            case 2: // "关闭"
                label->setStyleSheet("background-color: red; border-radius: 25px;"); // 设置红色背景
                if (serial->isOpen()) {
                    QByteArray data("5000"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据5000！";
                }
                break;
        }
    } else {
        // 用户取消操作，将 ComboBox 的当前选择重新设置为之前的选项
        ui->ComboBox_5->blockSignals(true); // 阻止信号槽循环调用
        ui->ComboBox_5->setCurrentIndex(index); // 设置为之前的选项
        ui->ComboBox_5->blockSignals(false); // 解除信号槽阻止
    }
}
}
void MainWindow::onComboBox6IndexChanged(int index) {
if (ui->modeshiftComboBox->currentText() == "手动模式")
{
    QString action;
    if (index == 0) {
        action = "等待";
    } else if (index == 1) {
        action = "打开";
    } else if (index == 2) {
        action = "关闭";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作", "确定要" + action + "吗？", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 用户确认操作，执行相应的逻辑
        QLabel* label = ui->label66; // 获取 label66 指针
        ui->label66->setFixedSize(50, 50);

        // 根据 ComboBox 的选择更新 QLabel 的颜色
        switch(index) {
            case 0: // "等待"
                label->setStyleSheet("background-color: yellow; border-radius: 25px;"); // 设置背景
                break;
            case 1: // "打开"
                label->setStyleSheet("background-color: green; border-radius: 25px;"); // 设置绿色背景
                if (serial->isOpen()) {
                    QByteArray data("6001"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据5001！";
                }
                break;
            case 2: // "关闭"
                label->setStyleSheet("background-color: red; border-radius: 25px;"); // 设置红色背景
                if (serial->isOpen()) {
                    QByteArray data("6000"); // 准备要发送的数据
                    serial->write(data); // 发送数据到串口
                } else {
                    qDebug() << "串口未打开，无法发送数据5000！";
                }
                break;
        }
    } else {
        // 用户取消操作，将 ComboBox 的当前选择重新设置为之前的选项
        ui->ComboBox_6->blockSignals(true); // 阻止信号槽循环调用
        ui->ComboBox_6->setCurrentIndex(index); // 设置为之前的选项
        ui->ComboBox_6->blockSignals(false); // 解除信号槽阻止
    }
}
}
void MainWindow::updateCoordinates()
{
if (currentMode == "手动模式")
{
    QString xyString;
    // 获取当前速度和角度滑块的值
    int speed = ui->verticalSlider->value();
    int angle = ui->horizontalSlider->value();


    // 将速度和角度映射到坐标系中
    double x = speed;
    double y = angle;
    // 构建带有"x"和"y"标签的字符串
    xyString = "x:" + QString::number(x, 'f', 1) + ", y:" + QString::number(y, 'f', 1) + ";";

    // 发送数据到串口
    if (serial->isOpen())
    {
        QByteArray xyData = xyString.toUtf8();
        serial->write(xyData); // 发送数据到串口
        QTimer::singleShot(100, this, [=]() {
            // 延时 0.05 秒后执行的代码
            // 例如：执行其他操作等
        });
    }
    else
    {
        qDebug() << "串口未打开，无法发送数据！";
    }

    qDebug() << "Sent data:" << xyString;
}
}

void MainWindow::resetAngleSlider()
{
    // 将角度滑块的值设置为0
    ui->horizontalSlider->setValue(0);
}
void MainWindow::on_pushButton_clicked()
{
    QString text = ui->lineEdit->text();
    QString text_1 = ui->lineEdit_2->text();
    QString text_2 = ui->lineEdit_3->text();
    QString text_3 = ui->lineEdit_4->text();

    if (!text.isEmpty() && !text_1.isEmpty() && !text_2.isEmpty() && !text_3.isEmpty()) {
        // 当四个lineEdit都有数据时才能发送数据
        if (serial->isOpen()) {
            QString data("dianziweilan:%1;%2;%3;%4。"); // 添加前缀文本，并指定格式
            data = data.arg(text).arg(text_1).arg(text_2).arg(text_3); // 根据格式添加lineEdit的内容
            QByteArray sendData = data.toUtf8(); // 将QString类型的数据转换为QByteArray类型的数据
            serial->write(sendData); // 发送数据到串口
            qDebug() << "Sent data: " << data; // 打印发送的数据
        }else {
            qDebug() << "串口未打开，无法发送数据dianziweilan！";
        }
    } else {
        qDebug() << "四个LineEdit均不能为空！";
    }
    ui->lineEdit->activateWindow();
    ui->lineEdit->setFocus();
}

