#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_spcomm = new spcomm();
    //m_serialPort.setBaudRate(9600);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_spcomm;
}

void MainWindow::on_pushButton_clicked()
{

}
