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
QByteArray buffer;

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
//    m_serialcarberry->write(QString("+++").toUtf8()); // Pause datastream
    m_serialcarberry->write(QString("CAN MODE USER\r\n").toUtf8()); // Set CAN to User mode
    m_serialcarberry->write(QString("CAN USER OPEN CH1 500k\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER ALIGN RIGHT\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER FILTER CH1 0 0300\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER MASK CH1 0F00\r\n").toUtf8());
    m_serialcarberry->write(QString("CAN USER STATUS CH1\r\n").toUtf8());
    m_serialcarberry->write(QString("ATO\r\n").toUtf8());
}

void SerialCarberry::readyToRead()
{
    m_readDataCarberry = m_serialcarberry->readAll();
//    QString rawMsg = QString::fromUtf8(m_readDataCarberry);

    // TODO: Collect buffers until \r\n and process
//    QStringList msgs = rawMsg.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
 //ebug() << rawMsg;
//    qDebug() <<("Process Raw Message");
    SerialCarberry::ProcessRawMessage(m_readDataCarberry);
}

QString EOL = QString("\r\n").toUtf8();

void SerialCarberry::closeConnection()
{
    m_serialcarberry->write(QString("CAN USER CLOSE CH1\r\n").toUtf8());
    m_serialcarberry->close();
}

void SerialCarberry::ProcessRawMessage(const QByteArray &buffer)
{
    m_buffer.append(buffer);
    int i = 0;

    while (m_buffer.contains(QString("\r\n").toUtf8()) && i < 100)
    {
        // Pop message
        // Cut message and process it
        int posend = m_buffer.indexOf(EOL);
        QByteArray msg = m_buffer;
//        qDebug() << "A1:" << m_buffer;
//        qDebug() << "A2:" << msg;
//        qDebug() << "A3:" << posend;
        int CurrentLenght = m_buffer.length();
        msg.remove(posend,CurrentLenght-posend);
//        qDebug() << "B1:" << m_buffer;
//        qDebug() << "B2:" << msg;
        m_buffer.remove(0, posend+2);
        SerialCarberry::ProcessMessage(msg);

    }
       // qDebug() <<("Buffer")<<m_buffer;
}

void SerialCarberry::ProcessMessage(const QString &msg) {
    if(msg.startsWith("RX1")) {
        QString dataVar = msg.split(' ')[1];
        QStringList data = dataVar.split('-');
        QString txID = data[0];
        bool ok;
        uint offset = txID.toUInt(&ok, 16)-0x300; // Configurable base
        QString txMSG = data[1];
        int var1 = txMSG.mid(0,4).toShort(&ok, 16);
        int var2 = txMSG.mid(4,4).toShort(&ok, 16);
        int var3 = txMSG.mid(8,4).toShort(&ok, 16);
        int var4 = txMSG.mid(16,4).toShort(&ok, 16);
       // qDebug() << offset << ":" << var1 << " " << var2 << " "<< var3 << " " << var4;

        switch(offset) {
            case 4:
                qDebug() << msg;
                qDebug() << "Ext 0-5V 2 input: " << (float)var1/1000 << "V Bat 12V:" << (float)var2/1000 << " +5V:" << (float)var3/1000 << "V SGND:" << (float)var4/1000 << "V";
                break;
            default:
                break;
        }
    }
}
