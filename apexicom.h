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

  \file apexicom.h
  \brief request and receive messages from Apexi PowerFC
  \author Bastian Gschrey & Markus Ippy
 */

#ifndef APEXICOM_H
#define APEXICOM_H
#include <QObject>
#include <QTimer>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QTimer>


namespace APEXI {
    namespace DATA {
        enum ENUM {
            Advance=0xF0,
            MapIndex=0xDB,
            SensorData=0xDE,
            BaiscData=0xDA,
            AuxData=0x00

       };
    }
}

class DashBoard;
class SerialPort;
class DecoderApexi;



class ApexiCom : public QObject
{
    Q_OBJECT
public:

    explicit ApexiCom(QObject *parent = 0);
    explicit ApexiCom(DecoderApexi *m_decoderapexi, QObject *parent = 0);
    Q_INVOKABLE void clear() const;
    Q_INVOKABLE void initSerialPort();
    Q_INVOKABLE void openConnection(const QString &portName);
    Q_INVOKABLE void closeConnection();

public slots:


public:
    void writeRequestPFC(QByteArray);

private:
    DashBoard *m_dashboard;
    SerialPort *m_serial;
    DecoderApexi *m_decoderapexi;
    QStringList *m_ecuList;
    qint64      m_bytesWritten;
    QByteArray  m_readData;
    QTimer      m_timer;
    QByteArray  m_buffer;
    QByteArray  m_apexiMsg;
    QByteArray  m_writeData;

signals:
    void sig_finished();
    void sig_error(QString err);
    void sig_ApexiDataAvailable(QByteArray);



public slots:
    void readyToRead();
    void handleTimeout();
    void handleBytesWritten(qint64 bytes);
    void handleError(QSerialPort::SerialPortError error);
    void apexiECU(const QByteArray &buffer);
    void sendRequest(int requestIndex);
    void readData(QByteArray serialdata);

};

#endif // APEXICOM_H
