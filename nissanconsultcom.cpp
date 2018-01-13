/*
 * Copyright (C) 2018 Markus Ippy
 *
 * Nissan Consult Communication Protocol for PowerTune 
 *
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 *
 * No warranty is made or implied. You use this program at your own risk.

  \file nissanconsultcom.h
  \brief request and receive messages from Nissan Consult ECU’s
  \author Markus Ippy
 */

/*
Generic Nissan Engine ECU Register Table.
This is a list of all known registers and their function. Any particular ECU will only respond to a sub-set of
these depending on the engine type and configuration. Please report inaccuracies back to the Nissan
Technical egroup.

INITIALISATION 			0xFF 0xFF 0xEF
SELF DIAGNOSTIC 0		xD1
ERASE ERROR CODES 		0xC1
ECU INFO 				0xD0

Data Name 						Byte 		Register No (hex) 			Scaling
CAS Position (RPM) 				MSB 			0x00 					See LSB
CAS Position (RPM) 				LSB 			0x01 					Value * 12.5 (RPM)
CAS Reference (RPM) 			MSB 			0x02 					See LSB
CAS Reference (RPM) 			LSB 			0x03 					Value * 8 (RPM) ??
MAF voltage 					MSB 			0x04 					See LSB
MAF voltage 					LSB 			0x05 					Value * 5 (mV)
RH MAF voltage 					MSB 			0x06 					See LSB
RH MAF voltage 					LSB 			0x07 					Value * 5 (mV)
Coolant temp 					Single byte 	0x08 					Value-50 (deg C)
LH O2 Sensor Voltage 			Single byte 	0x09 					Value * 10 (mV)
RH O2 Sensor Voltage 			Single byte 	0x0a 					Value * 10 (mV)
Vehicle speed 					Single byte 	0x0b 					Value * 2 (kph)
Battery Voltage 				Single Byte 	0x0c 					Value * 80 (mV)
Throttle Position Sensor 		Single Byte 	0x0d 					Value * 20 (mV)
FUEL TEMP SEN 					Single byte 	0x0f 					Value-50 (deg C)
Intake Air Temp 				Single Byte 	0x11 					Value -50 (deg C)
Exhaust Gas Temp 				Single Byte 	0x12 					Value * 20 (mV)
Digital Bit Register 			Single byte 	0x13 					See digital register table
Injection Time (LH) 			MSB 			0x14 					See LSB
Injection Time (LH) 			LSB 			0x15 					Value / 100 (mS)
Ignition Timing 				Single Byte 	0x16 					110 – Value (Deg BTDC)
AAC Valve (Idle Air Valve %) 	Single Byte 	0x17 					Value / 2 (%)
A/F ALPHA-LH 					Single byte     0x1a 					Value (%)
A/F ALPHA-RH 					Single byte 	0x1b 					Value (%)
A/F ALPHA-LH (SELFLEARN) 		Single byte 	0x1c 					Value (%)
A/F ALPHA-RH (SELFLEARN) 		Single byte 	0x1d 					Value (%)
Digital Control Register 		Single byte 	0x1e 					See digital register table
Digital Control Register 		Single byte 	0x1f 					See digital register table
M/R F/C MNT 					Single byte 	0x21 					See digital register table
Injector time (RH) 				MSB 			0x22 					See LSB
Injector time (RH) 				LSD 			0x23 					Value / 100 (mS)
Waste Gate Solenoid % 			Single byte 	0x28
Turbo Boost Sensor, Voltage 	Single byte 	0x29
Engine Mount On/Off 			Single byte 	0x2a
Position Counter 				Single byte		0x2e
Purg. Vol. Control Valve, Step 	Single byte 	0x25
Tank Fuel Temperature, C 		Single byte 	0x26
FPCM DR, Voltage 				Single byte		0x27
Fuel Gauge, Voltage 			Single byte	 	0x2f
FR O2 Heater-B1 				Single byte 	0x30 					Bank 1?
FR O2 Heater-B2 				Single byte 	0x31 					Bank 2?
Ignition Switch 				Single byte 	0x32
CAL/LD Value, % 				Single byte 	0x33
B/Fuel Schedule, mS 			Single byte 	0x34
RR O2 Sensor Voltage 			Single byte 	0x35
RR O2 Sensor-B2 Voltage 		Single byte 	0x36 					Bank 2?
Absolute Throttle Position,		Single byte		0x37
Voltage
MAF gm/S 						Single byte 	0x38
Evap System Pressure, Voltage 	Single byte 	0x39
Absolute Pressure Sensor,		Dual 			0x3a, 0x4a
Voltage
FPCM F/P Voltage 				Dual 			0x52, 0x53
*/


#include "nissanconsultcom.h"
#include "dashboard.h"
#include "serial.h"
#include "decodernissanconsult.h"
#include "serialport.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "QObject"


QByteArray InitECU = (QByteArray::fromHex("FFFFEF"));
QByteArray Liveread;

int Livedatarequested = 0;
int Stoprequested = 0;
int DTCrequested = 0;


//Live Data Request commands


int ECUinitialized = 0;
int Expectedlenght;

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

void NissanconsultCom::LiveReqMsg(const int &val1, const int &val2, const int &val3, const int &val4, const int &val5, const int &val6, const int &val7, const int &val8, const int &val9, const int &val10, const int &val11, const int &val12, const int &val13, const int &val14, const int &val15, const int &val16, const int &val17, const int &val18, const int &val19, const int &val20, const int &val21, const int &val22, const int &val23, const int &val24, const int &val25, const int &val26, const int &val27, const int &val28, const int &val29, const int &val30, const int &val31, const int &val32, const int &val33, const int &val34, const int &val35)

{

    // Ensure the Array is cleared first

    Liveread.clear();
    // Build the request message for live Data based on the usser selected Sennsors (Reequest from QML)

    qDebug() <<("bUILD mESSAGE");

    if (val1 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::CASPosRPMMSB);
    }
    if (val2 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::CASPosRPMLSB);
    }
    if (val3 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::CASRefRPMMSB);
    }
    if (val4 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::CASRefRPMLSB);
    }
    if (val5 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::MAFVoltMSB);
    }
    if (val6 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::MAFVoltLSB);
    }
    if (val7 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::RHMAFVoltMSB);
    }
    if (val8 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::RHMAFVoltLSB);
    }
    if (val9 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::CoolantTemp);
    }
    if (val10 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::LHO2Volt);
    }
    if (val11 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::RHO2Volt);
    }	if (val12 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::Speed);
    }
    if (val13 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::BattVolt);
    }
    if (val14 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::TPS);
    }
    if (val15 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::FuelTemp);
    }
    if (val16 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::IAT);
    }
    if (val17 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::EGT);
    }
    if (val18 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::DigitalBitRegister);
    }
    if (val19 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::InjectTimeLHMSB);
    }
    if (val20 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::InjectTimeLHLSB);
    }
    if (val21 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::IgnitionTiming);
    }
    if (val22 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::AACValve);
    }
    if (val23 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::AFALPHALH);
    }
    if (val24 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::AFALPHARH);
    }
    if (val25 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::AFALPHASELFLEARNLH);
    }
    if (val26 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::AFALPHASELFLEARNRH);
    }
    if (val27 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::DigitalControlReg1);
    }
    if (val28 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::DigitalControlReg2);
    }
    if (val29 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::MRFCMNT);
    }
    if (val30 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::InjecttimeRHMSB);
    }
    //
    if (val31 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::InjecttimeRHLSB);
    }
    if (val32 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::WasteGate);
    }
    if (val33 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::MAPVolt);
    }
    if (val34 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::EngineMount);
    }
    if (val35 == 1 )
    {
        Liveread.append(ConsultData::LiveDataRequest);
        Liveread.append(ConsultData::PositionCounter);
    }

    //Terminate Message
    Liveread.append(ConsultData::TerminateMessage);
    qDebug() <<("Complete Message")<< Liveread.toHex();
}


void NissanconsultCom::initSerialPort()
{
    if (m_serialconsult)
        delete m_serialconsult;
    m_serialconsult = new SerialPort(this);
    connect(this->m_serialconsult,SIGNAL(readyRead()),this,SLOT(readyToRead()));
    connect(&m_DTCtimer, &QTimer::timeout, this, &NissanconsultCom::RequestDTC);


}


//function to open serial port
void NissanconsultCom::openConnection(const QString &portName)
{


    qDebug() <<("Open Consult ")<<portName;
    initSerialPort();
    m_serialconsult->setPortName(portName);
    m_serialconsult->setBaudRate(QSerialPort::Baud9600);
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
    ECUinitialized = 0;
    NissanconsultCom::InitECU();

}

void NissanconsultCom::closeConnection()

{
    m_serialconsult->close();


}


void NissanconsultCom::InitECU()

{
    ECUinitialized = 0;
    m_serialconsult->write(QByteArray::fromHex("FFFFEF"));
}
void NissanconsultCom::clear() const
{
    m_serialconsult->clear();
}

void NissanconsultCom::StopStream()

{
    m_serialconsult->write(QByteArray::fromHex("30"));
    Stoprequested = 1;
}

void NissanconsultCom::RequestDTC()

{
    qDebug() <<("Timer Expired ");
    m_DTCtimer.stop();
    DTCrequested = 1;
    NissanconsultCom::StopStream();
    Livedatarequested = 0;


}

void NissanconsultCom::RequestLiveData()

{
    m_DTCtimer.start(5000);
    qDebug() <<("DTCTIMER START ");
    Livedatarequested = 1;
    DTCrequested = 0;
    m_serialconsult->write(Liveread);

}
void NissanconsultCom::readyToRead()
{

    m_readDataConsult = m_serialconsult->readAll();
    qDebug() <<("Received Data ")<< m_readDataConsult.toHex() ;
    qDebug() <<("Initstate ")<< ECUinitialized ;

    if (ECUinitialized == 1)
    {
        qDebug() <<("Process Raw Message");
        NissanconsultCom::ProcessRawMessage(m_readDataConsult);
    }

    if (ECUinitialized == 0 && m_readDataConsult[0] == 0x10)
    {
        ECUinitialized = 1;
        qDebug() <<("Initstate ")<< ECUinitialized ;
        m_readDataConsult.clear();
        qDebug() <<("ECU Initialized ");
        // Send Live Data Stream Request
        NissanconsultCom::RequestLiveData();

    }

}

void NissanconsultCom::ProcessRawMessage(const QByteArray &buffer)
{
    m_buffer.append(buffer);

    QByteArray StartFrame = (QByteArray::fromHex("FF"));

    qDebug() <<("Current Message")<<m_buffer.toHex();

    if (m_buffer.contains(StartFrame))
    {
        int posstart = m_buffer.indexOf(StartFrame);
        qDebug() <<("Found Start Frame at position")<<posstart;
        Expectedlenght =  (posstart+2) + (m_buffer[posstart+1]);
        int CurrentLenght = m_buffer.length();
        qDebug() <<("Expexted message Lenght")<<Expectedlenght;
        qDebug() <<("Current Lenght")<<m_buffer.length();
        if (m_buffer.length() > Expectedlenght)
        {
            m_consultreply = m_buffer;
            m_consultreply.remove(Expectedlenght,CurrentLenght);
            m_buffer.remove(0,Expectedlenght);
        }
        if (m_buffer.length() == Expectedlenght)
        {
            m_consultreply = m_buffer;
        }
        }
        qDebug() <<("Consultreply")<<m_consultreply.toHex();
        qDebug() <<("Buffer")<<m_buffer.toHex();

    if (Stoprequested ==1)
    {
        QByteArray Stopbyte = (QByteArray::fromHex("CF"));

        qDebug() <<("Current Message")<<m_buffer.toHex();

        if (m_buffer.contains(Stopbyte))
        {
            qDebug() <<("Found Stopbyte");
            m_buffer.clear();
            Stoprequested = 0;

            if (DTCrequested ==1)
            {
                DTCrequested = 0;
                Livedatarequested = 1;
                m_serialconsult->write(QByteArray::fromHex("D1F0"));

            }
            if (Livedatarequested ==1 )
            {

                NissanconsultCom::RequestLiveData();;

            }
        }

    }


    if (m_consultreply.length() == Expectedlenght)
    {
        Expectedlenght = 2000;  //Set Expexctedlenght to a ridiculous High value
        qDebug() <<("Message Lenghth as expected");
        if (DTCrequested ==1)
        {
            DTCrequested = 0;
            NissanconsultCom::StopStream();

        }
        NissanconsultCom::ProcessMessage(m_consultreply);
        m_consultreply.clear();

    }


}



void NissanconsultCom::ProcessMessage(QByteArray serialdataconsult)
{
    quint8 requesttypeconsult = serialdataconsult[0];
    qDebug() <<("Check Message Type and send to decoder")<<serialdataconsult.toHex();

        //if(requesttypeconsult == 0x25){m_decodernissanconsult->decodeLiveStream(serialdataconsult);}
        //if(requesttypeconsult == 0x2E){m_decodernissanconsult->decodeDTCConsult(serialdataconsult);}
        if(requesttypeconsult == 0x2E){NissanconsultCom::StopStream();}
        serialdataconsult.clear();


}
