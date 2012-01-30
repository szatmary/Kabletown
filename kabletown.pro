#-------------------------------------------------
#
# Project created by QtCreator 2010-11-13T17:57:08
#
#-------------------------------------------------

QT += core gui network sql webkit

QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9

TARGET = "Kabletown"
TEMPLATE = app

CONFIG += x86
CONFIG -= x86_64
CONFIG += release
CONFIG -= debug

SOURCES += main.cpp \
    prefrences.cpp \
    qtsingleapplication.cpp \
    qtlockedfile.cpp \
    qtlockedfile_unix.cpp \
    qtlockedfile_win.cpp \
    qtlocalpeer.cpp \
    db.cpp \
    bwcheck.cpp \
    charts.cpp \
    graph.cpp \
    updatedialog.cpp \
    menubaricon.cpp

HEADERS  += main.h \
    prefrences.h \
    qtsingleapplication.h \
    qtlockedfile.h \
    qtlocalpeer.h \
    db.h \
    bwcheck.h \
    charts.h \
    graph.h \
    updatedialog.h \
    menubaricon.h

RESOURCES += \
    resources.qrc

FORMS += \
    prefrences.ui \
    charts.ui \
    updatedialog.ui

OTHER_FILES += \
    changelog.txt \
    v \
    b

win32{
    RC_FILE = cbm.rc
}

mac{
    #For Snow Leopard
#    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk

    OBJECTIVE_SOURCES += \
    cocoaappinitializer.mm

    HEADERS  += \
    cocoaappinitializer.h

    LIBS += -framework Foundation -framework CoreServices -framework IOKit -framework Security -lcrypto

    OTHER_FILES += \
        Info.plist \
        product_definition.plist \
        deploy_appstore.sh

    ICON = icon.icns
    QMAKE_INFO_PLIST = Info.plist
    #QMAKE_POST_LINK = deploy.sh
}
