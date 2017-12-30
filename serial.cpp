/*
* Copyright (C) 2017 Bastian Gschrey & Markus Ippy
*
* Digital Gauges for Apexi Power FC for RX7 on Raspberry Pi
*
*
* This software comes under the GPL (GNU Public License)
* You may freely copy,distribute etc. this as long as the source code
* is made available for FREE.
*
* No warranty is made or implied. You use this program at your own risk.
*/

/*!
  \file serial.cpp
  \brief Raspexi Viewer Power FC related functions
  \author Bastian Gschrey & Markus Ippy
*/


#include "serial.h"
#include "serialobd.h"
#include "decoder.h"
#include "dashboard.h"
#include "serialport.h"
#include "appsettings.h"
#include "gopro.h"
#include "gps.h"
#include "apexicom.h"
#include "adaptroniccom.h"
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QModbusRtuSerialMaster>
#include <QFile>
#include <QTextStream>
#include <QByteArrayMatcher>
#include <QProcess>



int requestIndex = 0; //ID for requested data type Power FC
int ecu; //0=apex, 1=adaptronic;2= OBD; 3= Dicktator ECU
int interface; // 0=fcHako, 1=fc-datalogIt
int logging; // 0 Logging off , 1 Logging to file
int loggingstatus;
int Bytesexpected;
int Bytes;
QString Logfilename;
QByteArray checksumhex;
QByteArray recvchecksumhex;

//reply = new QModbusReply;




Serial::~Serial()
{

    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;

}

Serial::Serial(QObject *parent) :
    QObject(parent),
    m_serialport(Q_NULLPTR),
    m_decoder(Q_NULLPTR),
    m_dashBoard(Q_NULLPTR),
    m_gopro(Q_NULLPTR),
    m_gps(Q_NULLPTR),
    m_obd(Q_NULLPTR),
    m_apexicom(Q_NULLPTR),
    m_adaptroniccom(Q_NULLPTR),
    m_bytesWritten(0)
{

    getPorts();
    m_dashBoard = new DashBoard(this);
    m_decoder = new Decoder(m_dashBoard, this);
    m_appSettings = new AppSettings(this);
    m_gopro = new GoPro(this);
    m_gps = new GPS(m_dashBoard, this);
    m_obd = new SerialOBD(m_dashBoard, this);
    m_apexicom = new ApexiCom(m_decoder,this);
    m_adaptroniccom = new AdaptronicCom(m_decoder,this);
    QQmlApplicationEngine *engine = dynamic_cast<QQmlApplicationEngine*>( parent );
    if (engine == Q_NULLPTR)
        return;
    engine->rootContext()->setContextProperty("Dashboard", m_dashBoard);
    engine->rootContext()->setContextProperty("Decoder", m_decoder);
    engine->rootContext()->setContextProperty("AppSettings", m_appSettings);
    engine->rootContext()->setContextProperty("GoPro", m_gopro);
    engine->rootContext()->setContextProperty("GPS", m_gps);
    engine->rootContext()->setContextProperty("OBD", m_obd);
    engine->rootContext()->setContextProperty("ApexiCom", m_apexicom);
    engine->rootContext()->setContextProperty("AdaptronicCom", m_adaptroniccom);
}

void Serial::initSerialPort()
{
    if (m_serialport)
        delete m_serialport;
    m_serialport = new SerialPort(this);
    connect(this->m_serialport,SIGNAL(readyRead()),this,SLOT(readyToRead()));
    m_readData.clear();
    //m_timer.start(5000);


}
void Serial::getEcus()
{
    QStringList EcuList;
    EcuList.append("PowerFC");
    EcuList.append("Adaptronic");
}


/*void Serial::setEcus(QStringList ECUList)
{

}*/

void Serial::getPorts()
{
    QStringList PortList;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        PortList.append(info.portName());
    }
    setPortsNames(PortList);
    // Check available ports evry 1000 ms
    QTimer::singleShot(1000, this, SLOT(getPorts()));
}
//function for flushing all serial buffers
void Serial::clear() const
{
    m_serialport->clear();
}


//function to open serial port
void Serial::openConnection(const QString &portName, const int &ecuSelect, const int &interfaceSelect, const int &loggingSelect)
{

    ecu = ecuSelect;
    interface = interfaceSelect;
    logging = loggingSelect;


    //Apexi
    if (ecuSelect == 0)
    {

        m_apexicom->openConnection(portName);

    }


    //Adaptronic
    if (ecuSelect == 1)
    {
        m_adaptroniccom->openConnection(portName);

    }
    //OBD
    if (ecuSelect == 2)
    {
    m_obd->SelectPort(portName);
    }
    //Dicktator
    if (ecuSelect == 3)
    {

        initSerialPort();
        m_serialport->setPortName(portName);
        m_serialport->setBaudRate(QSerialPort::Baud19200);
        m_serialport->setParity(QSerialPort::NoParity);
        m_serialport->setDataBits(QSerialPort::Data8);
        m_serialport->setStopBits(QSerialPort::OneStop);
        m_serialport->setFlowControl(QSerialPort::NoFlowControl);

        if(m_serialport->open(QIODevice::ReadWrite) == false)
        {
            m_dashBoard->setSerialStat(m_serialport->errorString());
        }
        else
        {
            m_dashBoard->setSerialStat(QString("Connected to Serialport"));
        }
    }

}
void Serial::closeConnection()
{
    if(ecu == 0){
        m_apexicom->closeConnection();
    }

    if(ecu == 1){
        m_adaptroniccom->closeConnection();
    }
    if(ecu == 3){
        m_serialport->close();
    }
}

void Serial::update()
{


    bool bStatus = false;

    QStringList args;
    qint64      pid = 0;

    args << "&";
    bStatus = QProcess::startDetached("/home/pi/update.sh", args, ".", &pid);

}



void Serial::readyToRead()
{

    if(ecu == 3) //Dicktator ECU
    {
        m_readData = m_serialport->readAll();
        Serial::dicktatorECU(m_readData);
        m_readData.clear();
    }

}


void Serial::dicktatorECU(const QByteArray &buffer)
{
    //Appending the message until the patterns Start and End Are found , then removing all bytes before and after the message
    m_buffer.append(buffer);
    QByteArray startpattern("START");
    QByteArrayMatcher startmatcher(startpattern);
    QByteArray endpattern("END");
    QByteArrayMatcher endmatcher(endpattern);
    int pos = 0;
    while((pos = startmatcher.indexIn(m_buffer, pos)) != -1)
    {

        if (pos !=0)
        {
            m_buffer.remove(0, pos);
        }
        if (pos == 0 ) break;
    }
    int pos2 = 0;
    while((pos2 = endmatcher.indexIn(m_buffer, pos2)) != -1)
    {


    if (pos2 > 30)
        {
            m_buffer.remove(0,pos2-30);
        }

        if (pos2 == 30 )
        {
            m_dicktatorMsg = m_buffer;
            m_buffer.clear();
            m_decoder->decodeDicktator(m_dicktatorMsg);
            break;
        }
    }

}



//function for Start Logging
void Serial::startLogging(const QString &logfilenameSelect, const int &loggeron)
{
    loggingstatus = loggeron;
    Logfilename = logfilenameSelect;
    if (ecu == 0)    //Apexi
    {
        {
            m_decoder->loggerApexi(Logfilename);
        }
    }
    if (ecu == 1)    //Adaptronic
    {
        QString filename = Logfilename + ".csv";
        QFile file( filename );
        //qDebug() << "Adaptronic start Log";
        if ( file.open(QIODevice::ReadWrite) )
        {
            QTextStream stream( &file );
            stream << "Time (s),RPM,MAP (kPa),MAT (°C),WT (°C),AuxT (°C),AFR,Knock,TPS %,Idle,MVSS (km/h),SVSS (km/h),Batt (V),Inj 1 (ms), Inj 2 (ms),Inj 3 (ms),Inj 4 (ms),Ign 1 (°),Ign 2 (°),Ign 3 (°),Ign 4 (°),Trim" << endl;
        }
        file.close();
        m_decoder->loggerAdaptronic(Logfilename);
        m_decoder->loggerActivationstatus(loggingstatus);
    }


    return;
}

//function for Stop Logging
void Serial::stopLogging(const int &loggeron)
{
    loggingstatus = loggeron;
    m_decoder->loggerActivationstatus(loggingstatus);
    return;
}


void Serial::Auxcalc (const QString &unitaux1,const int &an1V0,const int &an2V5,const QString &unitaux2,const int &an3V0,const int &an4V5)
{
    int aux1min = an1V0;
    int aux2max = an2V5;
    int aux3min = an3V0;
    int aux4max = an4V5;
    QString Auxunit1 = unitaux1;
    QString Auxunit2 = unitaux2;

    m_decoder->calculatorAux(aux1min,aux2max,aux3min,aux4max,Auxunit1,Auxunit2);
}


