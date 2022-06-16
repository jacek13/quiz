QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# For Web Assembly build
CONFIG += WASM
WASM {
CONFIG += -feature-thread
QMAKE_LFLAGS += -fwasm-exceptions
#QMAKE_CXXFLAGS += -fwasm-exceptions
#QMAKE_CXXFLAGS += --preload-file /resources
QMAKE_LFLAGS += --preload-file ./resources
} else {

}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connectionthread.cpp \
    dialogloading.cpp \
    emoji.cpp \
    jsonhandler.cpp \
    main.cpp \
    mainwindow.cpp \
    quizstorage.cpp

HEADERS += \
    connectionthread.h \
    dialogloading.h \
    emoji.h \
    jsonhandler.h \
    mainwindow.h \
    quizstorage.h

FORMS += \
    dialogloading.ui \
    mainwindow.ui

TRANSLATIONS += \
    quiz_pl_PL.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resources/questions.json
