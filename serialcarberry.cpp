/*
 * OBD/CAN Communication with Carberry board
 * Original code can be found on https://github.com/cjtejada/QtDigitalInstrumentCluster
 *
*/

#include <QDebug>
#include "serialcarberry.h"
#include "serial.h"
#include "serialport.h"

QString CarberryCANSelect;
QString CarberryPortnameselect;

int CANinitialized;

SerialCarberry::SerialCarberry(QObject *parent)
    : QObject(parent)
    , m_dashboard(Q_NULLPTR)
{

}
SerialCarberry::SerialCarberry(DashBoard *dashboard, QObject *parent)
    : QObject(parent)
    , m_dashboard(dashboard)
{

}

void SerialCarberry::SelectCANPort(const QString &portName)
{
    CarberryCANSelect = portName;
    qDebug() << "CAN Port "<< portName << "Selected!";
}


void SerialCarberry::initSerialPort()
{
    if (m_serialcarberry)
        delete m_serialcarberry;
    m_serialcarberry = new SerialPort(this);
    connect(this->m_serialcarberry,SIGNAL(readyRead()),this,SLOT(readyToRead()));
}

//function to open serial port
void SerialCarberry::openConnection(const QString &portName)
{

    qDebug() <<("Open Carberry")<<portName;
    initSerialPort();
    m_serialcarberry->setPortName(portName);
    m_serialcarberry->setBaudRate(QSerialPort::Baud115200);
    m_serialcarberry->setParity(QSerialPort::NoParity);
    m_serialcarberry->setDataBits(QSerialPort::Data8);
    m_serialcarberry->setStopBits(QSerialPort::OneStop);
    m_serialcarberry->setFlowControl(QSerialPort::NoFlowControl);;
    if(m_serialcarberry->open(QIODevice::ReadWrite) == false)
    {
        qDebug() << QString(m_serialcarberry->errorString());
    }
    else
    {
        qDebug() << QString("Connected to Carberry Serialport");
    }

    CANinitialized = 0;
    SerialCarberry::InitCANStream();
}

void SerialCarberry::InitCANStream()
{
    CANinitialized = 0;
    m_serialcarberry->write(QString("+++\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN MODE USER\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER OPEN CH1 500k\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER FILTER CH1 0 0300\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER MASK CH1 0F00\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER STATUS CH1\r\n").toUtf8());
}

void SerialCarberry::readyToRead()
{
    m_readDataCarberry = m_serialcarberry->readAll();
    QString rawMsg = QString::fromUtf8(m_readDataCarberry);

    // TODO: Collect buffers until \r\n and process
    QStringList msgs = rawMsg.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);


    qDebug() <<("Initstate ")<< CANinitialized ;
    if(CANinitialized == 0) {
        bool cool = true;
        for (QStringList::iterator it = msgs.begin(); it != msgs.end(); ++it) {
            QString current = *it;
            if(cool && current == "OPEN") {
                m_serialcarberry->write(QString("ATO\r\n").toUtf8());
                CANinitialized = 1;
            } else if(current != "OK") {
                qDebug() << "!!" << current << "]]";
                cool = false;
                break;
            } else {
                qDebug() << "[[" << current << "]]";
            }
        }
        if(!cool) {
            qWarning() << "Failed to initialize CAN on Carberry!";
        }
    } else {
        qDebug() <<("Received Data ")<< msgs;
    }
}

void SerialCarberry::closeConnection()

{
    m_serialcarberry->close();
}
