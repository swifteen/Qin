######################################################################
# Automatically generated by qmake (2.01a) Fri Dec 10 15:12:06 2010
######################################################################

TEMPLATE = app
QT += sql xml
TARGET = 
DEPENDPATH += . plugins
INCLUDEPATH += . plugins 3rdparty/chewing/include
LIBS += -lchewing -L$$PWD/3rdparty/chewing/lib

DEFINES += DEBUG
# Input
RESOURCES += qin.qrc

HEADERS += \
  QinConfig.h \
  QinEngine.h \
  QinIMBases.h \
  QVirtualKeyboard.h \
plugins/QinChewing.h \


FORMS += QVirtualKeyboard.ui

SOURCES += \
  Qin.cpp \
  QinEngine.cpp \
  QinIMBases.cpp \
  QVirtualKeyboard.cpp \
 plugins/QinChewing.cpp \

