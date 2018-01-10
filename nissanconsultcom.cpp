#include "nissanconsultcom.h"
#include "dashboard.h"
#include "serial.h"
#include "decodernissanconsult.h"
#include "serialport.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "QObject"



NissanconsultCom::NissanconsultCom(QObject *parent)
    : QObject(parent),
      m_serialconsult(Q_NULLPTR),
      m_decodernissanconsult(Q_NULLPTR)

{

}

NissanconsultCom::NissanconsultCom(DecoderNissanConsult *decodernissanconsult, QObject *parent)
    : QObject(parent)
    , m_decodernissanconsult(decodernissanconsult)
{
}


void NissanconsultCom::initSerialPort()
{
    if (m_serialconsult)
        delete m_serialconsult;
    m_serialconsult = new SerialPort(this);
    connect(this->m_serialconsult,SIGNAL(readyRead()),this,SLOT(readyToRead()));


}

//function for flushing all serial buffers
void NissanconsultCom::clear() const
{
    m_serialconsult->clear();
}


//function to open serial port
void NissanconsultCom::openConnection(const QString &portName)
{

    qDebug() <<("Opening Port");

    initSerialPort();
    m_serialconsult->setPortName(portName);
    m_serialconsult->setBaudRate(QSerialPort::Baud57600);
    m_serialconsult->setParity(QSerialPort::NoParity);
    m_serialconsult->setDataBits(QSerialPort::Data8);
    m_serialconsult->setStopBits(QSerialPort::OneStop);
    m_serialconsult->setFlowControl(QSerialPort::NoFlowControl);;

    if(m_serialconsult->open(QIODevice::ReadWrite) == false)
    {
       //m_dashBoard->setSerialStat(m_serialconsult->errorString());
    }
    else
    {
      // m_dashBoard->setSerialStat(QString("Connected to Serialport"));
    }


}

void NissanconsultCom::closeConnection()

{
    m_serialconsult->close();
}
