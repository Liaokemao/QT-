#ifndef SECONDPAGE_H
#define SECONDPAGE_H

#include <QMainWindow>
#include <QStringList>

namespace Ui {
class secondpage;
}

class secondpage : public QMainWindow
{
    Q_OBJECT

public:
    explicit secondpage(QWidget *parent = nullptr);
    ~secondpage();

signals:
    void backfirst();

private slots:
    void on_button5_clicked();

private:
    void readFileAndAddToList(const QString &filePath);

    Ui::secondpage *ui;
};

#endif // SECONDPAGE_H
