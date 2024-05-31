#ifndef FISTPAGE_H
#define FISTPAGE_H

#include <QMainWindow>
#include "mainwindow.h"
#include "secondpage.h"

namespace Ui {
class fistpage;
}

class fistpage : public QMainWindow
{
    Q_OBJECT

public:
    explicit fistpage(QWidget *parent = nullptr);
    ~fistpage();

    MainWindow *page2 = nullptr;
    secondpage *page3 = nullptr;
private:
    Ui::fistpage *ui;
};

#endif // FISTPAGE_H
