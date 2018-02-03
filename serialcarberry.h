
/*
 * OBD Communication by Claudio Tejada
 * Original code can be found on https://github.com/cjtejada/QtDigitalInstrumentCluster
 *
*/


#ifndef SERIALCARBERRY_H
#define SERIALCARBERRY_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QObject>
#include <QTimer>

#include "pids.h"

class DashBoard;

class SerialCarberry : public QObject
{
    Q_OBJECT
public:
    explicit SerialCarberry(QObject *parent = 0);
    explicit SerialCarberry(DashBoard *dashboard, QObject *parent = 0);
    Q_INVOKABLE void SelectCANPort(const QString &portName);

private:
    PIDs PID;
    QSerialPort m_serial;
    DashBoard *m_dashboard;

    bool ArrayEngineOff[3] = {false};
    int m_tCodeCounter = 0;
    int m_engineOffcount = 0;
};

#endif // SERIALOBD_H
