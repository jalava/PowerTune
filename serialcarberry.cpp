/*
 * OBD/CAN Communication with Carberry board
 * Original code can be found on https://github.com/cjtejada/QtDigitalInstrumentCluster
 *
*/

#include <QDebug>
#include "dashboard.h"
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
       // qDebug() << msg;
        QString dataVar = msg.split(' ')[1];
        QStringList data = dataVar.split('-');
        QString txID = data[0];
        bool ok;
//        if(CANProfile == 0) { // Default Adaptronic Modular CAN Profile
            uint offset = txID.toUInt(&ok, 16)-0x300; // Configurable base
            QString txMSG = data[1];
            short var1 = static_cast<int16_t>(txMSG.mid(0,4).toUShort(&ok, 16));
            short var2 = static_cast<int16_t>(txMSG.mid(4,4).toUShort(&ok, 16));
            short var3 = static_cast<int16_t>(txMSG.mid(8,4).toUShort(&ok, 16));
            short var4 = static_cast<int16_t>(txMSG.mid(16,4).toUShort(&ok, 16));
           // qDebug() << offset << ":" << var1 << " " << var2 << " "<< var3 << " " << var4;

            switch(offset) {
                case 4:
//4 ($004) 	16 - 19 	Ext 0-5V 2 input voltage 	12V supply voltage 	5V sensor supply voltage 	Sensor ground voltage

                    //qDebug() << msg;
                    //qDebug() << "Ext 0-5V 2 input: " << (float)var1/1000 << "V Bat 12V:" << (float)var2/1000 << " +5V:" << (float)var3/1000 << "V SGND:" << (float)var4/1000 << "V";
                    m_dashboard->setBatteryV((float)var2/1000);
                    break;
                case 16:
//                16 ($010)	64 - 67 	RPM 	(unused) 	(unused) 	(unused)
                    qDebug() << "RPM " << var1;
                    break;
                case 97:
//                97 ($061) 388 - 391 	IMAP 	IMAP 2 	EMAP 	EMAP 2
//                    qDebug() << "IMAP " <<  var1 / 10 << "IMAP2 " << var2 / 10 << "EMAP " << var3 / 10.0 << "EMAP2 " << var4 / 10;
                    m_dashboard->setMAP(var1 / 10.0);
                    m_dashboard->setIntakepress(var1 / 10.0);
                    break;
                case 99:
//                99 ($063) 396 - 399  TPS overall 	 TPS1 (electronic throttle)  TPS2 (electronic throttle) 	TPS3 (electronic throttle)
                    qDebug() << "TPS " <<  var1 / 100.0;
                    m_dashboard->setTPS(var1 / 100.0);
                    break;
                case 100:
//                100 ($064) 	400 - 403 	TPS4 (electronic throttle) 	Lambda 1 	Lambda 2 	ECT
                    m_dashboard->setAFR(14.7-14.7*var2/1000.0); // TODO: Select AFR Source and stoich metric (flex fuel possible too)
                    m_dashboard->setWatertemp(var4/10.0);
                    qDebug() << "AFR: " << 14.7-14.7*var2/1000.0;
                    // TODO: Set lambda
                    break;
                case 101:
//                101 ($065) 	404 - 407 	MAT 	Oil temp 	Fuel temp 	Oil pressure
                    // MAT
                    m_dashboard->setIntaketemp(var1/10.0);
                    // TODO: Support for:
                    // OilT
                    // FuelT
                    m_dashboard->setFueltemp(var3/10.0);
                    // OilP
                    break;
                case 102:
//                102 ($066) 	408 - 411 	Fuel gauge pressure 	Fuel differential pressure 	Servo position 	Ext 0-5V input 1 (calibrated)
                    //  	Fuel gauge pressure 	Fuel differential pressure 	Servo position 	Ext 0-5V input 1 (calibrated)
                    break;
                case 103:
//                103 ($067) 	412 - 415 	Ext 0-5V input 2 (calibrated) 	Sensor GND voltage with GND disconnected 	Ethanol percentage from sensor 	Vehicle speed
                    break;
                case 104:
//                104 ($068) 	416 - 419 	Gear 	Driven speed 	Ground (undriven) speed 	Slip speed
                    break;
                case 108:
//                108 ($06c) 	432 - 435 	Digital inputs (processed low) 	Digital inputs (processed high) 	Flags (low) 	Flags (high)
                    break;
                case 117:
//                117 ($075) 	468 - 471 	Lateral undriven wheel slip 	MGP 1 	MGP 2 	Knock max (inc bkg)
                    break;
                case 120:
//                120 ($078) 	480 - 483 	Injector duty cycle 1 	Injector duty cycle 2 	Injector duty cycle 3 	Injector duty cycle 4
                    break;
                case 129:
//                129 ($081) 	516 - 519 	Charge temp 1 	Charge temp 2 	Stoich ratio 	Target lambda
                    break;
                case 138:
//                138 ($08a) 	552 - 553 	Fuel mass cyl 15 	Fuel mass cyl 16 	Fuel inj duration 1 	Fuel inj duration 2
                    break;
                case 163:
//                 163 ($0a3) 	652 - 655 	Ignition timing (leading) 	Ignition timing (trailing) 	(unused) 	(unused)
                    break;
                case 165:
//                165 ($0a5) 	660 - 663 	Async 1 duration 	Async 1 strobe 	Async 2 duration 	Async 2 strobe
                    break;
                case 170:
//                 170 ($0aa) 	680 - 683 	Idle duty cycle from closed loop correction 	Idle controller status 	Unclipped idle effort 	Final idle effort
                    break;
                case 176:
//                176 ($0b0) 	704 - 707 	Cutting conditions 	Current RPM limit 	Pitlane RPM sampled 	Pitlane status
                    break;
                case 177:
//                177 ($0b1) 	708 - 711 	Cut percentage from antilag 	Cut percentage from traction control 	Final fuel cut percentage 	Final ignition cut percentage
                    break;
                case 186:
//                186 ($0ba) 	744 - 747 	Boost control duty cycle 1 unclipped 	Final boost control duty cycle 1 	Boost controller 1 status 	Closed loop correction for boost controller 2
                break;

                default:
                    break;
            }
//      }
    }
}
