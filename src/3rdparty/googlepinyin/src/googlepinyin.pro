QT       -= gui

TARGET = googlepinyin
TEMPLATE = lib
#CONFIG += staticlib
INCLUDEPATH += ../include

SOURCES += \
    dictbuilder.cpp \
    dictlist.cpp \
    dicttrie.cpp \
    lpicache.cpp \
    matrixsearch.cpp \
    mystdlib.cpp \
    ngram.cpp \
    pinyinime.cpp \
    searchutility.cpp \
    spellingtable.cpp \
    spellingtrie.cpp \
    splparser.cpp \
    sync.cpp \
    userdict.cpp \
    utf16char.cpp \
    utf16reader.cpp

HEADERS += \
    ../include/atomdictbase.h \
    ../include/dictbuilder.h \
    ../include/dictdef.h \
    ../include/dictlist.h \
    ../include/dicttrie.h \
    ../include/lpicache.h \
    ../include/matrixsearch.h \
    ../include/mystdlib.h \
    ../include/ngram.h \
    ../include/pinyinime.h \
    ../include/searchutility.h \
    ../include/spellingtable.h \
    ../include/spellingtrie.h \
    ../include/splparser.h \
    ../include/sync.h \
    ../include/userdict.h \
    ../include/utf16char.h \
    ../include/utf16reader.h
