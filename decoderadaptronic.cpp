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
#include <QQmlApplicationEngine>

int unitsa;// 0 Metric / 1 Imperial
QByteArray serialdataa;

//QBitArray flagArray;
QString Logfilea;
qreal odometeradaptronic;
QTime startTimer = QTime::currentTime();
QTime loggerTimer = QTime::currentTime();
int Log =3;
int Loggerstatadaptronic;

DecoderAdaptronic::DecoderAdaptronic(DashBoard *dashboard, QObject *parent)
    : QObject(parent)
    , m_dashBoard(dashboard)
{
    QQmlApplicationEngine *engine = dynamic_cast<QQmlApplicationEngine*>(parent);
    if (engine == Q_NULLPTR){
        qDebug() << "engine is NULL from serial class";
        return;
    }


}



void DecoderAdaptronic::setUnits(const int &unitSelect)
{
    unitsa = unitSelect;
    if (unitsa == 0 )
    {m_dashBoard->setunits("metric");}
    if (unitsa == 1 )
    {m_dashBoard->setunits("imperial");}
    //qDebug() << "Unitselection"<< units;
}


void DecoderAdaptronic::decodeAdaptronic(QModbusDataUnit unit)
{

    qreal realBoost;
    int Boostconv;


 if (unitsa == 0)
 {
     //qDebug() << "i am at 0 " ;
    m_dashBoard->setSpeed(unit.value(10)); // <-This is for the "main" speedo KMH
 }
 if (unitsa == 1)
 {
    m_dashBoard->setSpeed(unit.value(10)*0.621371); // <-This is for the "main" speedo in MPH
 }
    m_dashBoard->setRevs(unit.value(0));
    qDebug() << "REVS" << m_dashBoard->revs();
    m_dashBoard->setMAP(unit.value(1));
    if (unitsa == 0)
    {
    m_dashBoard->setIntaketemp(unit.value(2));
    }
    if (unitsa == 1)
    {
    m_dashBoard->setIntaketemp(qRound(unit.value(2)* 1.8 + 32));
    }
    if (unitsa == 0)
    {
    m_dashBoard->setWatertemp(unit.value(3));
    }
    if (unitsa == 1)
    {
    m_dashBoard->setWatertemp(qRound(unit.value(3)* 1.8 + 32));
    }
    if (unitsa == 0)
    {
    m_dashBoard->setAUXT(unit.value(4));
    }
    if (unitsa == 1)
    {
    m_dashBoard->setAUXT(unit.value(4)* 1.8 + 32);
    }
    m_dashBoard->setauxcalc1(unit.value(5)/2570.00);
    m_dashBoard->setKnock(unit.value(6)/256);
    m_dashBoard->setTPS(unit.value(7));
    m_dashBoard->setIdleValue(unit.value(8));
    m_dashBoard->setBatteryV(unit.value(9)/10);

    if (unitsa == 0)
    {
    m_dashBoard->setMVSS(unit.value(10));
    }
    if (unitsa == 1)
    {
    m_dashBoard->setMVSS(qRound(unit.value(10)*0.621371));
    }
    if (unitsa == 0)
    {
    m_dashBoard->setSVSS(unit.value(11));
    }
    if (unitsa == 1)
    {
    m_dashBoard->setSVSS(qRound(unit.value(11)*0.621371));
    }
    m_dashBoard->setInj1((unit.value(12)/3)*2);
    m_dashBoard->setInj2((unit.value(13)/3)*2);
    m_dashBoard->setInj3((unit.value(14)/3)*2);
    m_dashBoard->setInj4((unit.value(15)/3)*2);
    m_dashBoard->setIgn1((unit.value(16)/5));
    m_dashBoard->setIgn2((unit.value(17)/5));
    m_dashBoard->setIgn3((unit.value(18)/5));
    m_dashBoard->setIgn4((unit.value(19)/5));
    m_dashBoard->setTRIM((unit.value(20)));


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


    m_dashBoard->setpim(realBoost);


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
