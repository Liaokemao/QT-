// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QSlider>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void handlePositionData(int statusData);
    void handleStatusData(int statusData);
    //void updateTemperature(double temperature0);
    void updatezhongliang(double zhongliang0);
    void handleCrackData(Ui::MainWindow *ui, const QVector<QPointF> &crackDataPoints);
    void updateCoordinates();
    void resetAngleSlider();
    Ui::MainWindow *ui;
private slots:
    void on_connectionButton_clicked();       // 打开串口按钮点击槽函数
    void on_disconnectionButton_clicked();    // 关闭串口按钮点击槽函数
    void readSerialData();                    // 读取串口数据槽函数
    void on_modeshiftComboBox_currentIndexChanged(const QString &mode);  // modeshiftComboBox模式切换槽函数
    void onAccButtonClicked(); // 增加按钮点击时的槽函数声明
    void onDecButtonClicked(); // 减少按钮点击时的槽函数声明
    void recordbuttonclicked();
    void recordbutton_2clicked();
    void recordbutton_3clicked();
    void recordbutton_4clicked();
    void recordbutton_5clicked();
    void on_pushButton_clicked();
    void Init();
    void onComboBox1IndexChanged(int index);
    void onComboBox2IndexChanged(int index);
    void onComboBox3IndexChanged(int index);
    void onComboBox4IndexChanged(int index);
    void onComboBox5IndexChanged(int index);
    void onComboBox6IndexChanged(int index);

signals:
    void backone();
private:
    QSerialPort *serial;                      // 串口对象
    QGraphicsScene *scene; // 声明 scene 成员
    QPoint MapRemov_Old;
    QString currentMode; // 当前模式变量
    QVector<QPointF> crackDataPoints;
    int drawnPointCount = 0;
};

#endif // MAINWINDOW_H
