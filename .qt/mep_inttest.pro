TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../src
SOURCES += \
        ../interactivetest/inttest.c \
        ../interactivetest/socknet.c \
        ../src/aesys_mep.c 
