#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GString.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QScrollBar>
#include "dialog.h"
#include "searchbox.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    connect(ui->action, SIGNAL(triggered()), this, SLOT(Find()));
    connect(ui->action_2, SIGNAL(triggered()), this, SLOT(Close()));

    int retVal = m_satan.Init();
    if( retVal != 0 ) return ;
    m_satan.getGuruList( m_hostArray );

    ///talbe1
    ui->tableWidget->setRowCount( m_hostArray.Count() );
    ui->tableWidget->setColumnCount(2);
    //QDebug qdebug;
    for( int i = 0 ; i < m_hostArray.Count() ; i++ )
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr(m_hostArray.getStr0(i)));
        ui->tableWidget->setItem(i, 0, newItem);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(tr(m_hostArray.getStr1(i)));
        ui->tableWidget->setItem(i, 1, newItem2);
    }


    m_satan.getFrontList( m_frontArray );
    ///talbe2
    ui->tableWidget_2->setRowCount( m_frontArray.Count() );
    ui->tableWidget_2->setColumnCount(2);
    //QDebug qdebug;
    for( int i = 0 ; i < m_frontArray.Count() ; i++ )
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr(m_frontArray.getStr0(i)));
        ui->tableWidget_2->setItem(i, 0, newItem);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(tr(m_frontArray.getStr1(i)));
        ui->tableWidget_2->setItem(i, 1, newItem2);
    }

    /*
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Log()));
    timer->interval();
    timer->setInterval( 1000 );
    timer->start(5000);
    */

    searchBox = new SearchBox();
    searchBox->setHost( m_hostArray );
    connect(searchBox, SIGNAL(searchFinish()), this, SLOT(Finded()));
}

MainWindow::~MainWindow()
{
    if( searchBox ) delete searchBox;
    delete ui;
}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    /*
    const QAbstractItemModel * model = index.model();
    QVariant value = index.data();

    QMessageBox msgBox;
    msgBox.setText( value.toString() );
    msgBox.exec();
    */

    int idx = index.row();
    GString ssh;
    ssh.Format("/usr/bin/gnome-terminal -e 'ssh folder@%s'", m_hostArray.getStr1(idx) );
    system( (const char*)ssh );
    //const QAbstractItemModel * model = index.model();
    //QVariant value = index.data();

  // QMessageBox msgBox;
  // msgBox.setText( (const char*)ssh );
  // msgBox.exec();


}

void MainWindow::Find()
{
    //searchBox.setWindowModality( Qt::ApplicationModal );
    searchBox->show();
    /*

    Dialog findDial;
    findDial.exec();

    QInputDialog inputBox;
    inputBox.exec();


//    QMessageBox msgBox;
//   msgBox.setText( inputBox.textValue() );
//    msgBox.exec();
    QString key = inputBox.textValue();
    int findFlag = 0;
    for( int i = 0 ; i < m_hostArray.Count() ; i++ )
    {
        if( key == m_hostArray.getStr0(i) || key == m_hostArray.getStr1(i) )
        {
            QTableWidgetItem *item = ui->tableWidget->item(i,0);
            item->setSelected(true);
            ui->tableWidget->scrollToItem( item );
            findFlag = 1;
            break;
        }
    }
    if( !findFlag )
    {
        QMessageBox msgBox;
        msgBox.setText( "NOT FOUND" );
        msgBox.exec();
    }
    */
}

void MainWindow::Finded()
{

    QString key = searchBox->text();
    fprintf( stderr, "FINDED %s\n", key.toStdString().c_str() );

    int findFlag = 0;
    ui->tableWidget->clearSelection();
    for( int i = 0 ; i < m_hostArray.Count() ; i++ )
    {
        if( key == m_hostArray.getStr0(i) || key == m_hostArray.getStr1(i) )
        {
            QTableWidgetItem *item = ui->tableWidget->item(i,0);
            item->setSelected(true);
            ui->tableWidget->scrollToItem( item );
            findFlag = 1;
            break;
        }
    }
    if( !findFlag )
    {
        QMessageBox msgBox;
        msgBox.setText( "NOT FOUND" );
        msgBox.exec();
    }
}

void MainWindow::Close()
{
    exit(0);
}

void MainWindow::Log()
{
    ui->listWidget->addItem(tr("LOG"));
    QScrollBar *scroll = ui->listWidget->verticalScrollBar();
    scroll->setValue( scroll->maximum() );
    m_satan.CheckServer(ESERVER_TYPE_GURU);
    //v->setValue(v->maxValue());
}

void MainWindow::on_tableWidget_2_doubleClicked(const QModelIndex &index)
{
    int idx = index.row();
    GString ssh;
    ssh.Format("/usr/bin/gnome-terminal -e 'ssh folder@%s'", m_frontArray.getStr1(idx) );
    system( (const char*)ssh );
}
