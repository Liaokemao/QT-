#include "secondpage.h"
#include "ui_secondpage.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

secondpage::secondpage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::secondpage)
{
    ui->setupUi(this);
    // 设置列表控件的样式表，添加表格网格线
    QString styleSheet = "\
        QListView {\
            border: 3px solid black;\
        }\
        QListView::item {\
            border-bottom: 2px solid black;\
            margin-bottom: 10px; /* 添加间距 */\
            font-size: 20px; /* 设置字体大小为14像素 */\
        }\
    ";

    ui->listWidget->setStyleSheet(styleSheet);
    connect(ui->button3, &QPushButton::clicked, [=](){
        emit this->backfirst();
    });

    connect(ui->button5, &QPushButton::clicked, this, &secondpage::on_button5_clicked);
}

secondpage::~secondpage()
{
    delete ui;
}

void secondpage::on_button5_clicked()
{
    qDebug() << "Button 5 clicked"; // 调试输出
    QString folderPath = "/home/passoni/qt_record"; // 指定文件夹路径

    // 打开文件选择对话框，让用户选择特定的 CSV 文件
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择CSV文件"), folderPath, tr("CSV 文件 (*.csv)"));

    // 如果用户取消选择文件，则返回
    if(filePath.isEmpty()) {
        return;
    }

    // 更新QLineEdit控件显示文件名
    QString fileName = QFileInfo(filePath).fileName();
    ui->lineEdit->setText(fileName);

    // 清空列表内容
    ui->listWidget->clear();

    // 将选择的 CSV 文件添加到列表中
    readFileAndAddToList(filePath);
}


void secondpage::readFileAndAddToList(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);

    QString crackDataX;
    QString crackDataY;
    QString statusData;
    QString posData;
    QStringList otherDataList;
    QString temperatureData; // 存储温度数据
    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.startsWith("crack:")) {
            QStringList parts = line.split(":");
            if (parts.length() == 2) {
                QString coordinatesString = parts[1].trimmed();
                QStringList coordinates = coordinatesString.split(";");
                for (int i = 0; i < coordinates.length(); ++i) {
                    QString coordinatePair = coordinates[i].trimmed();
                    QStringList xy = coordinatePair.split(",");
                    if (xy.length() == 2) {
                        crackDataX.append(xy[0]);
                        crackDataY.append(xy[1]);
                        if (i < coordinates.length() - 1) {
                            crackDataX.append(" ");
                            crackDataY.append(" ");
                        }
                    }
                }
            }
        }else if (line.startsWith("temperature")) {
            temperatureData.append(line + " ");
        }
        else if (line.startsWith("status")) {
            statusData.append(line + " ");
        }
        else if (line.startsWith("pos")) {
            posData.append(line + " ");
        }
        else {
            // 其他数据
            otherDataList.append(line);
        }
    }
    // 添加其他类型的数据
    QString specificData; // 存储特定类型的数据，放在同一行
    for (const QString &data : otherDataList) {
        // 检查特定类型的数据
        if (data.startsWith("裂缝长度:") || data.startsWith("裂缝宽度:") || data.startsWith("不开槽")) {
            specificData += data + " ";
        } else {
            // 添加其他类型的数据到列表中
            ui->listWidget->addItem(data);
        }
    }
    // 添加特定类型的数据到列表中
    if (!specificData.isEmpty()) {
        ui->listWidget->addItem(specificData);
    }
    ui->listWidget->addItem("温度记录(°C):");
    // 添加所有温度数据到列表中
    ui->listWidget->addItem(temperatureData);

    // 将整理后的数据添加到列表控件中
    ui->listWidget->addItem("裂缝X坐标:");
    QString formattedCrackDataX;
    for (const QString& coordinate : crackDataX.split(" ")) {
        formattedCrackDataX.append(coordinate).append(", ");
    }
    formattedCrackDataX.chop(2); // 移除最后多余的逗号和空格
    ui->listWidget->addItem("crack_x: " + formattedCrackDataX);

    ui->listWidget->addItem("裂缝Y坐标:");
    QString formattedCrackDataY;
    for (const QString& coordinate : crackDataY.split(" ")) {
        formattedCrackDataY.append(coordinate).append(", ");
    }
    formattedCrackDataY.chop(2); // 移除最后多余的逗号和空格
    ui->listWidget->addItem("crack_y: " + formattedCrackDataY);

    ui->listWidget->addItem("开槽/补缝(0/1)");
    ui->listWidget->addItem(statusData);

    ui->listWidget->addItem("作业进度(10*%):");
    ui->listWidget->addItem(posData);


}



