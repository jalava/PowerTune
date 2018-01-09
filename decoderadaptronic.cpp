#include "decoderadaptronic.h"
#include "dashboard.h"
#include "serial.h"
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QBitArray>
#include <QModbusDataUnit>
#include <QFile>
#include <QTextStream>
#include <QThread>

int unitsa;// 0 Metric / 1 Imperial
QByteArray serialdataa;

//QBitArray flagArray;
QString Logfilea;
qreal odometeradaptronic;
QTime startTimer = QTime::currentTime();
QTime loggerTimer = QTime::currentTime();
int Log =3;
int Loggerstatadaptronic;


DecoderAdaptronic::DecoderAdaptronic (QObject *parent)
    : QObject(parent)
    , m_dashboard(Q_NULLPTR)
{
}

DecoderAdaptronic::DecoderAdaptronic(DashBoard *dashboard, QObject *parent)
    : QObject(parent)
    , m_dashboard(dashboard)
{
}



void DecoderAdaptronic::setUnits(const int &unitSelect)
{
    unitsa = unitSelect;
    if (unitsa == 0 )
    {m_dashboard->setunits("metric");}
    if (unitsa == 1 )
    {m_dashboard->setunits("imperial");}
    //qDebug() << "Unitselection"<< units;
}


void DecoderAdaptronic::decodeAdaptronic(QModbusDataUnit unit)
{

    qreal realBoost;
    int Boostconv;


 if (unitsa == 0)
 {
     //qDebug() << "i am at 0 " ;
    m_dashboard->setSpeed(unit.value(10)); // <-This is for the "main" speedo KMH
 }
 if (unitsa == 1)
 {
    m_dashboard->setSpeed(unit.value(10)*0.621371); // <-This is for the "main" speedo in MPH
 }
    m_dashboard->setRevs(unit.value(0));
    m_dashboard->setMAP(unit.value(1));
    if (unitsa == 0)
    {
    m_dashboard->setIntaketemp(unit.value(2));
    }
    if (unitsa == 1)
    {
    m_dashboard->setIntaketemp(qRound(unit.value(2)* 1.8 + 32));
    }
    if (unitsa == 0)
    {
    m_dashboard->setWatertemp(unit.value(3));
    }
    if (unitsa == 1)
    {
    m_dashboard->setWatertemp(qRound(unit.value(3)* 1.8 + 32));
    }
    if (unitsa == 0)
    {
    m_dashboard->setAUXT(unit.value(4));
    }
    if (unitsa == 1)
    {
    m_dashboard->setAUXT(unit.value(4)* 1.8 + 32);
    }
    m_dashboard->setauxcalc1(unit.value(5)/2570.00);
    m_dashboard->setKnock(unit.value(6)/256);
    m_dashboard->setTPS(unit.value(7));
    m_dashboard->setIdleValue(unit.value(8));
    m_dashboard->setBatteryV(unit.value(9)/10);
    if (unitsa == 0)
    {
    m_dashboard->setMVSS(unit.value(10));
    }
    if (unitsa == 1)
    {
    m_dashboard->setMVSS(qRound(unit.value(10)*0.621371));
    }
    if (unitsa == 0)
    {
    m_dashboard->setSVSS(unit.value(11));
    }
    if (unitsa == 1)
    {
    m_dashboard->setSVSS(qRound(unit.value(11)*0.621371));
    }
    m_dashboard->setInj1((unit.value(12)/3)*2);
    m_dashboard->setInj2((unit.value(13)/3)*2);
    m_dashboard->setInj3((unit.value(14)/3)*2);
    m_dashboard->setInj4((unit.value(15)/3)*2);
    m_dashboard->setIgn1((unit.value(16)/5));
    m_dashboard->setIgn2((unit.value(17)/5));
    m_dashboard->setIgn3((unit.value(18)/5));
    m_dashboard->setIgn4((unit.value(19)/5));
    m_dashboard->setTRIM((unit.value(20)));


    // Convert absolute pressure in KPA to relative pressure mmhg/Kg/cm2

        if ((unit.value(1)) > 103) // while boost pressure is positive multiply by 0.01 to show kg/cm2
        {
            Boostconv = ((unit.value(1))-103) ;
            realBoost = Boostconv* 0.01;
            //qDebug() << realBoost;
        }
        else if ((unit.value(1)) < 103) // while boost pressure is negative  multiply by 0.01 to show kg/cm2
        {
            Boostconv = ((unit.value(1))-103) * 7.50061561303;
            realBoost = Boostconv;

        }


    m_dashboard->setpim(realBoost);


    //Datalogger Adaptronic Comma seperated Text File for easy import to Excel
    if (Loggerstatadaptronic ==1)
    {
    QString fileName = Logfilea;
    //qDebug() << Logfilea;
    QFile mFile(fileName);
    if(!mFile.open(QFile::Append | QFile::Text)){
        //qDebug() << "Could not open Adaptronic Loggerfile for writing";
    }
    QTextStream out(&mFile);
    out << (loggerTimer.msecsTo(QTime::currentTime()))<< "," << unit.value(0) << ","
        << unit.value(1) << "," << unit.value(2) << "," << unit.value(3) << ","
        << unit.value(4) << "," << (unit.value(5)/2570.00) << "," << (unit.value(6)/256)
        << "," << unit.value(7) << "," << unit.value(8) << "," <<(unit.value(9)/10) << ","
        << unit.value(10) << "," << unit.value(11) << ","<< ((unit.value(12)/3)*2) << ","
        <<((unit.value(13)/3)*2) << "," << ((unit.value(14)/3)*2) << ","
       << ((unit.value(15)/3)*2) << ","<< (unit.value(16)/5) << ","
       << (unit.value(17)/5) << "," << (unit.value(18)/5) << ","
       << (unit.value(19)/5) << "," << unit.value(20)<< "," << endl;

    mFile.close();
}

    emit sig_adaptronicReadFinished();
}

void DecoderAdaptronic::loggerAdaptronic(QString Logfileaname)
{
Logfilea = Logfileaname+".csv";
loggerTimer.restart();
}


void DecoderAdaptronic::loggerActivationstatus(int Logstatus)
{
Loggerstatadaptronic = Logstatus;
//qDebug() <<"Decoder Logstatus"<< Logstatus;
}
