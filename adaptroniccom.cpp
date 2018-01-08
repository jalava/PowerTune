#include "adaptroniccom.h"
#include "serial.h"
#include "serialport.h"
#include "decoder.h"
#include "appsettings.h"
#include <QDebug>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusRtuSerialMaster>


AdaptronicCom::AdaptronicCom(QObject *parent)
    : QObject(parent),
      m_serialadaptronic(Q_NULLPTR),
      m_decoder(Q_NULLPTR),
      lastRequest(nullptr),
      modbusDevice(nullptr)

    {
        modbusDevice = new QModbusRtuSerialMaster(this);
        connect(m_decoder,SIGNAL(sig_adaptronicReadFinished()),this,SLOT(AdaptronicStartStream()));
    }


AdaptronicCom::AdaptronicCom(Decoder *decoder, QObject *parent)
    : QObject(parent)
    , m_decoder(decoder)

{
}

void AdaptronicCom::initSerialPort()
{
    if (m_serialadaptronic)
        delete m_serialadaptronic;
    m_serialadaptronic = new SerialPort(this);
    connect(this->m_serialadaptronic,SIGNAL(readyRead()),this,SLOT(readyToRead()));


}


//function to open serial port
void AdaptronicCom::openConnection(const QString &portName)
{

    qDebug() <<("Opening Port Adaptronic");

     {

        if (!modbusDevice)
            return;

        if (modbusDevice->state() != QModbusDevice::ConnectedState)
        {
            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,portName);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,57600);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,8);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,0);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,1);
            modbusDevice->setTimeout(200);
            modbusDevice->setNumberOfRetries(10);
            modbusDevice->connectDevice();
            qDebug() <<("Connected");
            AdaptronicCom::AdaptronicStartStream();

       }

    }

}
void AdaptronicCom::closeConnection()
{
        modbusDevice->disconnectDevice();
}


void AdaptronicCom::readyToRead()
{
        auto reply = qobject_cast<QModbusReply *>(sender());
        if(!reply)
            return;
        if(reply->error() == QModbusDevice::NoError){
            const QModbusDataUnit unit = reply->result();
            m_decoder->decodeAdaptronic(unit);
        }

}

// Adaptronic streaming comms

void AdaptronicCom::AdaptronicStartStream()
{
    auto *reply = modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 4096, 21),1); // read first twenty-one realtime values
    if (!reply->isFinished())
        connect(reply, &QModbusReply::finished, this,&AdaptronicCom::readyToRead);
    else
        delete reply;

}
