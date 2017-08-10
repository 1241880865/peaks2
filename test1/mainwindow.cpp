#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_spcomm = new SPComm();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_spcomm;
}

void MainWindow::on_pushButton_clicked()
{
    if(m_spcomm->open())
        return;
    m_spcomm->setBaudRate(9600);
    m_spcomm->setPortName("com3");
    if(m_spcomm->open())
        qDebug() << "打开串口成功";
}


void MainWindow::on_pushButton_2_clicked()
{
    if(m_spcomm->open())
        m_spcomm->close();
        qDebug() << "关闭串口成功";
}
