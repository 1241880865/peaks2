#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/********************************************************************************
********************************************************************************/
void modbusMaster::on_connectButton_clicked()//连接按钮点击槽方法
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    QString comPortName = ui->lineEdit->text();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {//如果处在非连接状态，进行连接
        //进行串口连接
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
            comPortName);//获取串口端口,下面设置各种参数
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
            QSerialPort::EvenParity);//偶较验
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
            QSerialPort::Baud19200);//波特率为19200
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
            QSerialPort::Data8);//数据位为8位
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
            QSerialPort::OneStop);//停止位为1位
        modbusDevice->setTimeout(1000);//连接超时1S
        modbusDevice->setNumberOfRetries(3);//连接失败重试三次连接
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    }else{//处在连接状态进行断开连接的操作
        modbusDevice->disconnectDevice();
    }
}

void modbusMaster::on_sendButton_clicked()
{
    if (!modbusDevice)//如果设备没有被创建就返回
        return;
    QString strDis;
    QString str1 = ui->textEdit->toPlainText();
    qDebug()<<str1;
    QByteArray str2 = QByteArray::fromHex (str1.toLatin1().data());//按十六进制编码接入文本
    //qDebug()<<str2;
    QString str3 = str2.toHex().data();//以十六进制显示
    statusBar()->clearMessage();//清除状态栏显示的信息
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,0,10);//发送的数据信息（数据类型 ，起始地址，个数）
//    QModbusDataUnit::RegisterType table = writeUnit.registerType();//得到寄存器类型传给table
    for (uint i = 0; i < writeUnit.valueCount(); i++) {
        int j = 2*i;
        QString st = str3.mid (j,2);
        bool ok;
        int hex =st.toInt(&ok,16);//将textedit中读取到的数据转换为16进制发送
        qDebug()<<hex;
        writeUnit.setValue(i,hex);//设置发送数据
    }
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) {// 1是server address   sendWriteRequest是向服务器写数据
        if (!reply->isFinished()) {   //reply Returns true when the reply has finished or was aborted.
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}
void modbusMaster::on_readButton_clicked()//从服务器读数据的读按钮槽方法
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,0,10);
    if (auto *reply = modbusDevice->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &modbusMaster::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}


void modbusMaster::readReady()//在这里读的数据
{
    auto reply = qobject_cast<QModbusReply *>(sender());//QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
    if (!reply)
        return;
//数据从QModbusReply这个类的resuil方法中获取,也就是本程序中的reply->result()
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            ui->textEdit_2->append(entry);
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }
    reply->deleteLater();
}
