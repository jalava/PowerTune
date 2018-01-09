TEMPLATE = app

QT += qml quick serialport serialbus network charts
CONFIG += c++11
static {
    QT += svg
    QTPLUGIN += qtvirtualkeyboardplugin
}


SOURCES += main.cpp \
    serial.cpp \
    dashboard.cpp \
    serialport.cpp \
    appsettings.cpp \
    gopro.cpp \
    gps.cpp \
    serialobd.cpp \
    apexicom.cpp \
    adaptroniccom.cpp \
    nissanconsultcom.cpp \
    decoderapexi.cpp \
    decoderadaptronic.cpp \
    decodernissanconsult.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    serial.h \
    dashboard.h \
    serialport.h \
    appsettings.h \
    gopro.h \
    gps.h \
    instrumentcluster.h \
    pids.h \
    serialobd.h \
    apexicom.h \
    adaptroniccom.h \
    nissanconsultcom.h \
    decoderapexi.h \
    decoderadaptronic.h \
    decodernissanconsult.h

FORMS +=

DISTFILES += \
    ../../../Users/Marku/Downloads/Changed Files.zip

