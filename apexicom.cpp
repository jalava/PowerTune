#include "apexicom.h"
#include "dashboard.h"
#include "serial.h"
#include "decoder.h"
#include "serialport.h"
#include "appsettings.h"
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QTextStream>
#include <QByteArrayMatcher>
#include <QProcess>

int reqquestInd = 0; //ID for requested data type Power FC
int ExpectedBytes;
//int Bytes;
//QString Logfilename;
QByteArray checksumh;
QByteArray recvchecksumh;

ApexiCom::ApexiCom(QObject *parent)
    : QObject(parent),
      m_serial(Q_NULLPTR),
     // m_decoder(Q_NULLPTR),
      m_bytesWritten(0)
      //m_dashboard(Q_NULLPTR)

{

}




void ApexiCom::initSerialPort()
{
    if (m_serial)
        delete m_serial;
    m_serial = new SerialPort(this);
    connect(this->m_serial,SIGNAL(readyRead()),this,SLOT(readyToRead()));
    connect(m_serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &ApexiCom::handleError);
    connect(m_serial, &QSerialPort::bytesWritten, this, &ApexiCom::handleBytesWritten);
    connect(&m_timer, &QTimer::timeout, this, &ApexiCom::handleTimeout);
    m_readData.clear();


}

//function for flushing all serial buffers
void ApexiCom::clear() const
{
    m_serial->clear();
}


//function to open serial port
void ApexiCom::openConnection(const QString &portName)
{

    qDebug() <<("Opening Port");

    initSerialPort();
    m_serial->setPortName(portName);
    m_serial->setBaudRate(QSerialPort::Baud57600);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);;

    if(m_serial->open(QIODevice::ReadWrite) == false)
    {
       //m_dashBoard->setSerialStat(m_serial->errorString());
    }
    else
    {
      // m_dashBoard->setSerialStat(QString("Connected to Serialport"));
    }

    reqquestInd = 0;

    ApexiCom::sendRequest(reqquestInd);

}

void ApexiCom::closeConnection()

{
    m_serial->close();
}

void ApexiCom::handleTimeout()
{
    m_readData.clear();
    ApexiCom::sendRequest(reqquestInd);
}

void ApexiCom::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        QString fileName = "Errors.txt";
        QFile mFile(fileName);
        if(!mFile.open(QFile::Append | QFile::Text)){
        }
        QTextStream out(&mFile);
        out << "Serial Error " << (m_serial->errorString()) <<endl;
        mFile.close();
        //m_dashBoard->setSerialStat(m_serial->errorString());

    }
}


void ApexiCom::readyToRead()
{

    m_readData = m_serial->readAll();
    ApexiCom::apexiECU(m_readData);

}

void ApexiCom::apexiECU(const QByteArray &buffer)
{
    if (ExpectedBytes != m_buffer.length())
    {
        m_timer.start(5000);
    }
    m_buffer.append(buffer);

    QByteArray startpattern = m_writeData.left(1);
    QByteArrayMatcher startmatcher(startpattern);

    int pos = 0;
    while((pos = startmatcher.indexIn(m_buffer, pos)) != -1)
    {
        if (pos !=0)
        {
            m_buffer.remove(0, pos);
            if (m_buffer.length() > ExpectedBytes)
            {
                m_buffer.remove(ExpectedBytes,m_buffer.length() );
            }
        }

        if (pos == 0 )
        {
            break;
        }


    }

    if (m_buffer.length() == ExpectedBytes)
    {
        m_apexiMsg =  m_buffer;
        m_buffer.clear();
        m_timer.stop();
        if(reqquestInd <= 5){reqquestInd++;}
        else{reqquestInd = 2;}
        readData(m_apexiMsg);
        m_apexiMsg.clear();
        ApexiCom::sendRequest(reqquestInd);


    }
}




void ApexiCom::readData(QByteArray serialdata)
{

    if( serialdata.length() )
    {
        //Power FC Decode
        quint8 requesttype = serialdata[0];

        //Write all OK Serial Messages to a file
        if(serialdata[1] + 1 == serialdata.length())
        {

            switch (requesttype) {
            case APEXI::DATA::Advance:
                m_decoder->decodeAdv(serialdata);
                break;
            default:
                break;
            }


            if(requesttype == APEXI::DATA::Advance) {m_decoder->decodeAdv(serialdata);}
            if(requesttype == 0xDD){m_decoder->decodeSensorStrings(serialdata);}
            if(requesttype == 0xDE){m_decoder->decodeSensor(serialdata);}
            if(requesttype == 0x00){m_decoder->decodeAux(serialdata);}
            if(requesttype == 0x00){m_decoder->decodeAux2(serialdata);}
            if(requesttype == 0xDB){m_decoder->decodeMap(serialdata);}
            if(requesttype == 0xDA){m_decoder->decodeBasic(serialdata);}
            if(requesttype == 0xB8){m_decoder->decodeRevIdle(serialdata);}
            if(requesttype == 0x7D){m_decoder->decodeTurboTrans(serialdata);}
            if(requesttype == 0x76){m_decoder->decodeLeadIgn(serialdata, 0);}
            if(requesttype == 0x77){m_decoder->decodeLeadIgn(serialdata, 5);}
            if(requesttype == 0x78){m_decoder->decodeLeadIgn(serialdata, 10);}
            if(requesttype == 0x79){m_decoder->decodeLeadIgn(serialdata, 15);}
            if(requesttype == 0x81){m_decoder->decodeTrailIgn(serialdata, 0);}
            if(requesttype == 0x82){m_decoder->decodeTrailIgn(serialdata, 5);}
            if(requesttype == 0x83){m_decoder->decodeTrailIgn(serialdata, 10);}
            if(requesttype == 0x84){m_decoder->decodeTrailIgn(serialdata, 15);}
            if(requesttype == 0x86){m_decoder->decodeInjcorr(serialdata, 0);}
            if(requesttype == 0x87){m_decoder->decodeInjcorr(serialdata, 5);}
            if(requesttype == 0x88){m_decoder->decodeInjcorr(serialdata, 10);}
            if(requesttype == 0x89){m_decoder->decodeInjcorr(serialdata, 15);}

            if(requesttype == 0xB0){m_decoder->decodeFuelBase(serialdata, 0);}
            if(requesttype == 0xB1){m_decoder->decodeFuelBase(serialdata, 1);}
            if(requesttype == 0xB2){m_decoder->decodeFuelBase(serialdata, 2);}
            if(requesttype == 0xB3){m_decoder->decodeFuelBase(serialdata, 3);}
            if(requesttype == 0xB4){m_decoder->decodeFuelBase(serialdata, 4);}
            if(requesttype == 0xB5){m_decoder->decodeFuelBase(serialdata, 5);}
            if(requesttype == 0xB6){m_decoder->decodeFuelBase(serialdata, 6);}
            if(requesttype == 0xB7){m_decoder->decodeFuelBase(serialdata, 7);}

            if(requesttype == 0xF5){m_decoder->decodeVersion(serialdata);}
            if(requesttype == 0xF3){m_decoder->decodeInit(serialdata);}
            if(requesttype == 0xAB){m_decoder->decodeBoostCont(serialdata);}
            if(requesttype == 0x7B){m_decoder->decodeInjOverlap(serialdata);}
            if(requesttype == 0x92){m_decoder->decodeInjPriLagvsBattV(serialdata);}
            if(requesttype == 0x9F){m_decoder->decodeInjScLagvsBattV(serialdata);}
            if(requesttype == 0x8D){m_decoder->decodeFuelInjectors(serialdata);}
        }
        serialdata.clear();
        \

    }


}
void ApexiCom::handleBytesWritten(qint64 bytes)
{
    m_bytesWritten += bytes;
    if (m_bytesWritten == m_writeData.size()) {
        m_bytesWritten = 0;
        //qDebug() <<("Data successfully sent to port") << (m_serial->portName());

    }
}
// Serial requests are send via Serial
void ApexiCom::writeRequestPFC(QByteArray p_request)
{
    //qDebug() << "write request" << p_request.toHex();
    m_writeData = p_request;
    qint64 bytesWritten = m_serial->write(p_request);
    //m_dashBoard->setSerialStat(QString("Sending Request " + p_request.toHex()));

    //Action to be implemented
    if (bytesWritten == -1) {
     //   m_dashBoard->setSerialStat(m_serial->errorString());
        //qDebug() << "Write request to port failed" << (m_serial->errorString());
    } else if (bytesWritten != m_writeData.size()) {
     //   m_dashBoard->setSerialStat(m_serial->errorString());
        //qDebug() << "could not write complete request to port" << (m_serial->errorString());
    }

}

//Power FC requests

void ApexiCom::sendRequest(int reqquestInd)
{
    switch (reqquestInd){
    /*
case 0:
    //First request from (this is what FC Edit does seems to get a 4 or 8 Byte response dependant on Aux inputs ??)
    ApexiCom::writeRequestPFC(QByteArray::fromHex("0102FC"));
    if (interface ==0)
    {ExpectedBytes = 4;}
    if (interface ==1)
    {ExpectedBytes = 8;}
    break;
*/
    //case 1:
    case 0:
        //Init Platform (This returns the Platform String )
        ApexiCom::writeRequestPFC(QByteArray::fromHex("F3020A"));
        ExpectedBytes = 11;
        break;
        /*
case 2:
    //ApexiCom::getWarConStrFlags();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("D60227"));
    ExpectedBytes = 88;
    break;
case 3:
    //ApexiCom::getVersion();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("F50208"));
    ExpectedBytes = 8;
    break;
case 4:
    //ApexiCom::getMapRef();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("8A0273"));
    ExpectedBytes = 83;
    break;
case 5:
    //ApexiCom::getRevIdle();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B80245"));
    ExpectedBytes = 17;
    break;
case 6:
    //ApexiCom::getLeadign1();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("760287"));
    ExpectedBytes = 103;
    break;
case 7:
    //ApexiCom::getLeadign2();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("770286"));
    ExpectedBytes = 103;
    break;
case 8:
    //ApexiCom::getLeadign3();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("780285"));
    ExpectedBytes = 103;
    break;
case 9:
    //ApexiCom::getLeadign4();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("790284"));
    ExpectedBytes= 103;
    break;
case 10:
    //ApexiCom::getTrailIgn1();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("81027C"));
    ExpectedBytes= 103;
    break;
case 11:
    //ApexiCom::getTrailIgn2();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("82027B"));
    ExpectedBytes= 103;
    break;
case 12:
    //ApexiCom::getTrailIgn3();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("83027A"));
    ExpectedBytes = 103;
    break;
case 13:
    //ApexiCom::getTrailIgn4();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("840279"));
    ExpectedBytes = 103;
    break;
case 14:
    //ApexiCom::getPimStrInjA();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("CB0232"));
    ExpectedBytes = 110;
    break;
case 15:
    //ApexiCom::getInjOverlap();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("7B0282"));
    ExpectedBytes = 9;
    break;
case 16:
    //ApexiCom::getInjvsFuelT();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("7C0281"));
    ExpectedBytes = 12;
    break;
case 17:
    //ApexiCom::getTurboTrans();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("7D0280"));
    ExpectedBytes = 12;
    break;
case 18:
    //ApexiCom::getOilervsWaterT();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("7E027F"));
    ExpectedBytes = 9;
    break;
case 19:
    //ApexiCom::getFanvsWater();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("7F027E"));
    ExpectedBytes = 6;
    break;
case 20:
    //ApexiCom::getInjcorr1();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("860277"));
    ExpectedBytes = 103;
    break;
case 21:
    //ApexiCom::getInjcorr2();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("870276"));
    ExpectedBytes = 103;
    break;
case 22:
    //ApexiCom::getInjcorr3();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("880275"));
    ExpectedBytes = 103;
    break;
case 23:
    //ApexiCom::getInjcorr4();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("890274"));
    ExpectedBytes = 103;
    break;
case 24:
    //ApexiCom::getFuelInj();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("8D0270"));
    ExpectedBytes = 27;
    break;
case 25:
    //ApexiCom::getCranking();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("8E026F"));
    ExpectedBytes = 15;
    break;
case 26:
    //ApexiCom::getWaterTcorr();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("8F026E"));
    ExpectedBytes = 17;
    break;
case 27:
    //ApexiCom::getInjvsWaterBoost();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("90026D"));
    ExpectedBytes = 9;
    break;
case 28:
    //ApexiCom::getInjvsAirTBoost();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("91026C"));
    ExpectedBytes = 11;
    break;
case 29:
    //ApexiCom::getInjPrimaryLag();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("92026B"));
    ExpectedBytes = 15;
    break;
case 30:
    //ApexiCom::getAccInj();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("93026A"));
    ExpectedBytes = 28;
    break;
case 31:
    //ApexiCom::getInjvsAccel();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("940269"));
    ExpectedBytes = 12;
    break;
case 32:
    //ApexiCom::getIgnvsAircold();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("960267"));
    ExpectedBytes = 7;
    break;
case 33:
    //ApexiCom::getIgnvsWater();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("980265"));
    ExpectedBytes = 7;
    break;
case 34:
    //ApexiCom::getIgnvsAirwarm();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("9A0263"));
    ExpectedBytes = 9;
    break;
case 35:
    //ApexiCom::getLIgnvsRPM();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("9B0262"));
    ExpectedBytes = 9;
    break;
case 36:
    //ApexiCom::getIgnvsBatt();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("9C0261"));
    ExpectedBytes = 9;
    break;
case 37:
    //ApexiCom::getBoostvsIgn();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("9D0260"));
    ExpectedBytes = 7;
    break;
case 38:
    //ApexiCom::getTrailIgnvsRPM();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("9E025F"));
    ExpectedBytes = 9;
    break;
case 39:
    //ApexiCom::getInjSecLagvsBattV();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("9F025E"));
    ExpectedBytes = 15;
    break;
case 40:
    //ApexiCom::getKnockWarn();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("A90254"));
    ExpectedBytes = 7;
    break;
case 41:
    //Injejtor warning
    ApexiCom::writeRequestPFC(QByteArray::fromHex("A80255"));
    ExpectedBytes = 7;
    break;
case 42:
    //ApexiCom::getO2Feedback();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("AA0253"));
    ExpectedBytes = 6;
    break;
case 43:
    //ApexiCom::getBoostcontrol();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("AB0252"));
    ExpectedBytes = 14;
    break;
case 44:
    //ApexiCom::getSettingProtections();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("AC0251"));
    ExpectedBytes = 13;
    break;
case 45:
    //ApexiCom::getTunerString();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("AD0250"));
    ExpectedBytes = 11;
    break;
case 46:
    //ApexiCom::getFuelBase0();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B0024D"));
    ExpectedBytes = 103;
    break;
case 47:
    //ApexiCom::getFuelBase1();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B1024C"));
    ExpectedBytes = 103;
    break;
case 48:
    //ApexiCom::getFuelBase2();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B2024B"));
    ExpectedBytes = 103;
    break;
case 49:
    //ApexiCom::getFuelBase3();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B3024A"));
    ExpectedBytes = 103;
    break;
case 50:
    //ApexiCom::getFuelBase4();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B40249"));
    ExpectedBytes = 103;
    break;
case 51:
    //ApexiCom::getFuelBase5();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B50248"));
    ExpectedBytes = 103;
    break;
case 52:
    //ApexiCom::getFuelBase6();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B60247"));
    ExpectedBytes = 103;
    break;
case 53:
    //ApexiCom::getFuelBase7();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B70246"));
    ExpectedBytes = 103;
    break;
case 54:
    //ApexiCom::getInjvsAirTemp();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("B90244"));
    ExpectedBytes = 15;
    break;
case 55:
    //ApexiCom::getInjvsTPS();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("BB0242"));
    ExpectedBytes = 11;
    break;
case 56:
    //ApexiCom::getPIMScaleOffset();
    ApexiCom::writeRequestPFC(QByteArray::fromHex("BC0241"));
    ExpectedBytes = 23;
    break;
case 57:
    //Init Platform
    ApexiCom::writeRequestPFC(QByteArray::fromHex("F3020A"));
    ExpectedBytes = 11;
    break;
*/
        //case 58:
    case 1:
        //ApexiCom::getSensorStrings();
        ApexiCom::writeRequestPFC(QByteArray::fromHex("DD0220"));
        ExpectedBytes = 83;
        break;



        // Live Data
    case 2:
        //ApexiCom::getAdvData();
        ApexiCom::writeRequestPFC(QByteArray::fromHex("F0020D"));
        ExpectedBytes = 33;
        break;

    case 3:
        //ApexiCom::getMapIndices();
        ApexiCom::writeRequestPFC(QByteArray::fromHex("DB0222"));
        ExpectedBytes = 5;
        break;
    case 4:
        //ApexiCom::getSensorData();
        ApexiCom::writeRequestPFC(QByteArray::fromHex("DE021F"));
        ExpectedBytes = 21;
        break;
    case 5:
        //ApexiCom::getBasic();
        ApexiCom::writeRequestPFC(QByteArray::fromHex("DA0223"));
        ExpectedBytes = 23;
        break;
    case 6:
        //ApexiCom::getAux();
        ApexiCom::writeRequestPFC(QByteArray::fromHex("0002FD"));
        ExpectedBytes = 7;

        break;
    }
}
