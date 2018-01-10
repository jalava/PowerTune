#ifndef NISSANCONSULTCOM_H
#define NISSANCONSULTCOM_H
#include <QtSerialPort/QSerialPort>

class DashBoard;
class SerialPort;
class DecoderNissanConsult;



class NissanconsultCom : public QObject
{
    Q_OBJECT
public:

    explicit NissanconsultCom(QObject *parent = 0);
    explicit NissanconsultCom(DecoderNissanConsult *m_decodernissanconsult, QObject *parent = 0);
    Q_INVOKABLE void clear() const;
    Q_INVOKABLE void initSerialPort();
    Q_INVOKABLE void openConnection(const QString &portName);
    Q_INVOKABLE void closeConnection();

public slots:


public:


private:
    DashBoard *m_dashboard;
    SerialPort *m_serialconsult;
    DecoderNissanConsult *m_decodernissanconsult;

signals:



public slots:
    void InitECU();
    void StartStream();
    void StopStream();
    void ReadErrors();
    void readyToRead();


};

#endif // NISSANCONSULTCOM_H
