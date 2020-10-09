TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L/home/kapsch/src/sharedc/etrans3/etr03_drv/etr03_protocols/etr03_aesys_mep/x86-64_Ubuntu-18.04_lib -letr03_cutest -letr03_log -letr03_mutex
INCLUDEPATH += ../x86-64_Ubuntu-18.04_include/ ../src/ /usr/include/
SOURCES += \
        ../src/aesys_mep.c \
        ../unittest/unittest.c
