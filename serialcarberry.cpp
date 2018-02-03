/*
 * OBD/CAN Communication with Carberry board
 * Original code can be found on https://github.com/cjtejada/QtDigitalInstrumentCluster
 *
*/

#include "dashboard.h"
#include "serialcarberry.h"
#include "serial.h"

QString CarberryCANSelect;

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
    qDebug() << "Port "<< portName << "Selected!";
}
