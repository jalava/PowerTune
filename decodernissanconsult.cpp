#include "decodernissanconsult.h"
#include "dashboard.h"
#include "serial.h"
#include <QDebug>



DecoderNissanConsult::DecoderNissanConsult(QObject *parent)
    : QObject(parent)
    , m_dashboard(Q_NULLPTR)
{
}

DecoderNissanConsult::DecoderNissanConsult(DashBoard *dashboard, QObject *parent)
    : QObject(parent)
    , m_dashboard(dashboard)
{
}

void DecoderNissanConsult::decodeLiveStream(QByteArray serialdataconsult)
{

    int StartFrame      = (serialdataconsult.indexOf(QByteArray::fromHex("FF")));
    int CASPosRPMMSB 	= serialdataconsult.indexOf((QByteArray::fromHex("2500")));
    int CASRefRPM       = serialdataconsult.indexOf((QByteArray::fromHex("2502")));
    int MAFvoltage      = serialdataconsult.indexOf((QByteArray::fromHex("2504")));
    int RHMAFvoltage	= serialdataconsult.indexOf((QByteArray::fromHex("2506")));
    int Coolanttemp 	= serialdataconsult.indexOf((QByteArray::fromHex("2508")));
    int LHO2SensorVolt 	= serialdataconsult.indexOf((QByteArray::fromHex("2509")));
    int RHO2SensorVolt 	= serialdataconsult.indexOf((QByteArray::fromHex("250a")));
    int Speed           = serialdataconsult.indexOf((QByteArray::fromHex("250b")));

    if (CASPosRPMMSB >=0)
    {
        quint16 RPM = serialdataconsult[StartFrame+2+CASPosRPMMSB] *12.5;
        qDebug() <<  "RPM" << RPM ;
        //m_dashboard->setRevs(RPM);
    }
    if (CASRefRPM >=0)
    {
        quint16 RPMRef = serialdataconsult[StartFrame+2+CASRefRPM] * 8;
        qDebug() <<  "RPMRef" << RPMRef ;
        //m_dashboard->setRevs(RPMRef);
    }
    if (MAFvoltage >=0)
    {
        quint16 MAFV = serialdataconsult[StartFrame+2+MAFvoltage] * 5;
        qDebug() <<  "MAF Voltage" << MAFV ;
        //m_dashboard->setRevs(RPMRef);
    }

    if (RHMAFvoltage >=0)
    {
        quint16 MAFVRH = serialdataconsult[StartFrame+2+RHMAFvoltage] * 5;
        qDebug() <<  "RHMAFvoltage" << MAFVRH ;
        //m_dashboard->setRevs(RPMRef);
    }
    if (Coolanttemp >=0)
    {
        quint8 Coolant = serialdataconsult[StartFrame+2+Coolanttemp] -50;
        qDebug() <<  "Coolant" << Coolant ;
        //m_dashboard->setRevs(RPMRef);
    }
    if (LHO2SensorVolt >=0)
    {
        quint8 LHO2SensorV = serialdataconsult[StartFrame+2+LHO2SensorVolt] *10;
        qDebug() <<  "LHO2SensorV" << LHO2SensorV ;
        //m_dashboard->setRevs(RPMRef);
    }
    if (RHO2SensorVolt >=0)
    {
        quint8 RHO2SensorV = serialdataconsult[StartFrame+2+RHO2SensorVolt] *10;
        qDebug() <<  "LHO2SensorV" << RHO2SensorV ;
        //m_dashboard->setRevs(RPMRef);
    }
    if (Speed >=0)
    {
        quint8 SpeedKPH = serialdataconsult[StartFrame+2+Speed] *2;
        qDebug() <<  "SpeedKPH" << SpeedKPH ;
        //m_dashboard->setRevs(RPMRef);
    }
    /*




    Battery Voltage 				Single Byte 	= serialdataconsult.indexOf((QByteArray::fromHex("250c);  					Value * 80 (mV)
    Throttle Position Sensor 		Single Byte 	= serialdataconsult.indexOf((QByteArray::fromHex("250d);  					Value * 20 (mV)
    FUEL TEMP SEN 					Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("250f);  					Value-50 (deg C)
    Intake Air Temp 				Single Byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2511);  					Value -50 (deg C)
    Exhaust Gas Temp 				Single Byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2512);  					Value * 20 (mV)
    Digital Bit Register 			Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2513);  					See digital register table
    Injection Time (LH) 			MSB 			= serialdataconsult.indexOf((QByteArray::fromHex("2514);  					See LSB
    Injection Time (LH) 			LSB 			= serialdataconsult.indexOf((QByteArray::fromHex("2515);  					Value / 100 (mS)
    Ignition Timing 				Single Byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2516);  					110 – Value (Deg BTDC)
    AAC Valve (Idle Air Valve %) 	Single Byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2517);  					Value / 2 (%)
    A/F ALPHA-LH 					Single byte     = serialdataconsult.indexOf((QByteArray::fromHex("251a);  					Value (%)
    A/F ALPHA-RH 					Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("251b);  					Value (%)
    A/F ALPHA-LH (SELFLEARN) 		Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("251c);  					Value (%)
    A/F ALPHA-RH (SELFLEARN) 		Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("251d);  					Value (%)
    Digital Control Register 		Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("251e);  					See digital register table
    Digital Control Register 		Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("251f);  					See digital register table
    M/R F/C MNT 					Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2521);  					See digital register table
    Injector time (RH) 				MSB 			= serialdataconsult.indexOf((QByteArray::fromHex("2522);  					See LSB
    Injector time (RH) 				LSB 			= serialdataconsult.indexOf((QByteArray::fromHex("2523);  					Value / 100 (mS)
    Waste Gate Solenoid % 			Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2528);
    Turbo Boost Sensor, Voltage 	Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2529);
    Engine Mount On/Off 			Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("252a);
    Position Counter 				Single byte		= serialdataconsult.indexOf((QByteArray::fromHex("252e);
    Purg. Vol. Control Valve, Step 	Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2525);
    Tank Fuel Temperature, C 		Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2526);
    FPCM DR, Voltage 				Single byte		= serialdataconsult.indexOf((QByteArray::fromHex("2527);
    Fuel Gauge, Voltage 			Single byte	 	= serialdataconsult.indexOf((QByteArray::fromHex("252f);
    FR O2 Heater-B1 				Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2530);  					Bank 1?
    FR O2 Heater-B2 				Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2531);  					Bank 2?
    Ignition Switch 				Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2532);
    CAL/LD Value, % 				Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2533);
    B/Fuel Schedule, mS 			Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2534);
    RR O2 Sensor Voltage 			Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2535);
    RR O2 Sensor-B2 Voltage 		Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2536);  					Bank 2?
    Absolute Throttle Position,		Single byte		= serialdataconsult.indexOf((QByteArray::fromHex("2537);
    Voltage
    MAF gm/S 						Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2538);
    Evap System Pressure, Voltage 	Single byte 	= serialdataconsult.indexOf((QByteArray::fromHex("2539);
    Absolute Pressure Sensor,		Dual 			= serialdataconsult.indexOf((QByteArray::fromHex("253a);
    Voltage= serialdataconsult.indexOf((QByteArray::fromHex("254a);
    FPCM F/P Voltage 				Dual 			= serialdataconsult.indexOf((QByteArray::fromHex("2552);
                                                    = serialdataconsult.indexOf((QByteArray::fromHex("2553);


*/
}

void DecoderNissanConsult::decodeDTCConsult(QByteArray serialdataconsult)

{

}
