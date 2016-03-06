#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gen/ssArray.h"
#include "csatan.h"
#include "searchbox.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CSsArray m_hostArray;
    CSsArray m_frontArray;

private slots:

    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void Find();
    void Close();
    void Log();
    void Finded();

    void on_tableWidget_2_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    CSatan m_satan;
    SearchBox* searchBox;
};

#endif // MAINWINDOW_H
