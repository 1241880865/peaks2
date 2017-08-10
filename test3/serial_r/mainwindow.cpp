#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initSeialPort();
}

MainWindow::~MainWindow()
{
    delete ui;
    serial.close();
}

void MainWindow::initSeialPort()
{

    connect(&serial,SIGNAL(readyRead()),this,SLOT(serialRead()));   //连接槽

    //get name for choose
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
        ui->comboBox->addItem("无效");
        return;
    }
    ui->comboBox->addItem("串口");
    foreach (QSerialPortInfo info, infos) {
        ui->comboBox->addItem(info.portName());
    }
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    QSerialPortInfo info;
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    int i = 0;
    foreach (info, infos) {
        if(info.portName() == arg1) break;
        i++;
    }
    if(i != infos.size ()){//can find
        ui->label->setText("[已开启]");
        serial.close();
        serial.setPort(info);
        serial.open(QIODevice::ReadWrite);         //读写打开
    }
    else
    {
        serial.close();
        ui->label->setText("[出错]");
    }
}

void MainWindow::serialRead()
{
    //ui->textEdit->append(serial.readAll());

    QByteArray temp;
    temp = serial.readAll();

    /*
    int disp;
    QByteArray byteArrayTempInfo;
    disp = byteArrayTempInfo.toInt(temp);
    */
    ui->textEdit->clear();
    ui->textEdit->append(temp.toHex());

    QFile myfile("d:/test.txt");
    myfile.open(QIODevice::Append);
    myfile.write(temp.toHex()+"\r\n");
    myfile.close();

    /*
    QByteArray buf;
    buf = serial.readAll();
    if(!buf.isEmpty())
    {
        QString str = ui->textEdit->toPlainText();
        str+=tr(buf);
        ui->textEdit->clear();
        ui->textEdit->append(str);
    }
    buf.clear();
    */
}

void MainWindow::on_pushButton_2_clicked()
{
    if(ui->lineEdit->text().isEmpty())
        return;
    serial.write(ui->lineEdit->text().toLatin1());
}
