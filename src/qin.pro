######################################################################
# Automatically generated by qmake (2.01a) Fri Dec 10 15:12:06 2010
######################################################################

TEMPLATE = app
QT += sql xml
TARGET = 
DEPENDPATH += . plugins
INCLUDEPATH += . plugins
#�¿������뷨
INCLUDEPATH += 3rdparty/chewing/include
LIBS += -lchewing -L$$PWD/3rdparty/chewing/lib
#�ȸ�ƴ�����뷨
INCLUDEPATH += 3rdparty/googlepinyin/include
LIBS += -lgooglepinyin -L$$PWD/3rdparty/googlepinyin/lib

DEFINES += DEBUG
# Input
RESOURCES += qin.qrc

HEADERS += \
  QinConfig.h \
  QinEngine.h \
  QinIMBases.h \
  QVirtualKeyboard.h \
  plugins/QinChewing.h \
	plugins/QinGooglePinyin.h \

FORMS += QVirtualKeyboard.ui

SOURCES += \
  Qin.cpp \
  QinEngine.cpp \
  QinIMBases.cpp \
  QVirtualKeyboard.cpp \
  plugins/QinChewing.cpp \
  plugins/QinGooglePinyin.cpp \

