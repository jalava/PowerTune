#ifndef DECODERNISSANCONSULT_H
#define DECODERNISSANCONSULT_H
#include <QObject>

class DashBoard;


class DecoderNissanConsult: public QObject
{
    Q_OBJECT

public:
    explicit DecoderNissanConsult(QObject *parent = 0);
    explicit DecoderNissanConsult(DashBoard *dashboard, QObject *parent = 0);

private:
     DashBoard *m_dashboard;
public slots:
    void decodeLiveStream(QByteArray serialdataconsult);
    void decodeDTCConsult(QByteArray serialdataconsult);
};
#endif // DECODERNISSANCONSULT_H
