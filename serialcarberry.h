
/*
 * OBD Communication by Claudio Tejada
 * Original code can be found on https://github.com/cjtejada/QtDigitalInstrumentCluster
 *
*/


#ifndef SERIALCARBERRY_H
#define SERIALCARBERRY_H

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QDebug>

class DashBoard;
class SerialPort;

class SerialCarberry : public QObject
{
    Q_OBJECT
public:
    explicit SerialCarberry(QObject *parent = 0);
    explicit SerialCarberry(DashBoard *dashboard, QObject *parent = 0);
    Q_INVOKABLE void SelectCANPort(const QString &portName);
    Q_INVOKABLE void initSerialPort();
    Q_INVOKABLE void openConnection(const QString &portName);
    Q_INVOKABLE void closeConnection();

private:
    DashBoard *m_dashboard;
    SerialPort *m_serialcarberry;
    QByteArray  m_readDataCarberry;
    QByteArray  m_ECUResponsecomplete;
    QByteArray  m_buffer;
    QByteArray  m_carberryreply;

signals:



public slots:
    void InitCANStream();
//    void LiveReqMsg(const int &val1, const int &val2, const int &val3, const int &val4, const int &val5, const int &val6, const int &val7, const int &val8, const int &val9, const int &val10, const int &val11, const int &val12, const int &val13, const int &val14, const int &val15, const int &val16, const int &val17, const int &val18, const int &val19, const int &val20, const int &val21, const int &val22, const int &val23, const int &val24, const int &val25, const int &val26, const int &val27, const int &val28, const int &val29);
//    void StopStream();
//    void RequestDTC();
//    void RequestLiveData();
    void readyToRead();
//    void ProcessRawMessage(const QByteArray &buffer);
//    void ProcessMessage(QByteArray m_ECUResponsecomplete);


};

#endif // SERIALOBD_H
