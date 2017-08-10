#include "spcomm.h"

spcomm::spcomm(QObject *parent) : QObject(parent)
{
    m_serialPort = new QSerialPort();
    m_baudRate = 9600;
    m_portName = "";
}
//析构的时候  删除 数据
spcomm::~spcomm()
{
    delete m_serialPort;
}

