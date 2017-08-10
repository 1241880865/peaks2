#ifndef SPCOMM_H
#define SPCOMM_H

#include <QObject>
#include <QtSerialPort/QtSerialPort>

class spcomm : public QObject
{
    Q_OBJECT
public:
    explicit spcomm(QObject *parent = 0);
    ~spcomm();
    bool isOpen() const;

    /*
    void setPortName(const QString &name);
    QString portName() const;

    void setBaudRate(int baudRate);
    int baudRate() const;

    virtual bool open();
    virtual void close();
    virtual bool clear();

    int readData(char *buffer, int count, int timeout = 1000);
    int writeData(char *data, int size);
    int write(char ch);
    */

protected:
    QString m_portName;
    int m_baudRate;
    QSerialPort *m_serialPort;

signals:

public slots:
};

#endif // SPCOMM_H
