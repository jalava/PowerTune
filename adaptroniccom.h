/*
 * Copyright (C) 2016 Markus Ippy, Bastian Gschrey, Jan
 *
 * Digital Gauges for Apexi Power FC for RX7 on Raspberry Pi
 *
 *
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 *
 * No warranty is made or implied. You use this program at your own risk.
  \file adaptroniccom.h
  \brief request and receive messages from Adaptronic PNP & Select ECU's
  \author Bastian Gschrey & Markus Ippy
 */

#ifndef ADAPTRONICCOM_H
#define ADAPTRONICCOM_H
#include <QtSerialPort/QSerialPort>
#include <QObject>
#include <QModbusDataUnit>

class SerialPort;
class DecoderAdaptronic;
class QModbusClient;
class QModbusReply;
class DashBoard;

class AdaptronicCom : public QObject
{
    Q_OBJECT
public:
    ~AdaptronicCom();
    explicit AdaptronicCom(DashBoard *dashboard, QObject *parent = 0);
   // Q_INVOKABLE void clear() const;
    Q_INVOKABLE void initSerialPort();
    Q_INVOKABLE void openConnection(const QString &portName);
    Q_INVOKABLE void closeConnection();
private:
    SerialPort *m_serialadaptronic;
    DecoderAdaptronic *m_decoderadaptronic;
    DashBoard *m_dashBoard;
    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;
    QModbusDataUnit readRequest() const;

signals:

    void sig_finished();
    void sig_error(QString err);
    void start();


public slots:
    void readyToRead();
    void AdaptronicStartStream();
  //  void readData(QByteArray serialdata);

};

#endif // ADAPTRONICCOM_H
