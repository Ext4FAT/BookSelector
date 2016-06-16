#-------------------------------------------------
#
# Project created by QtCreator 2016-04-09T17:21:50
#
#-------------------------------------------------
QT       += core

TARGET = Crawl
CONFIG   += console
CONFIG   += c++11

TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/Book.cpp \
    src/Parameter.cpp \
    src/Parse.cpp

HEADERS += \
    include/Book.hpp \
    include/Bucket.hpp \
    include/Common.hpp \
    include/Macro.hpp \
    include/Parameter.hpp \
    include/Parse.hpp


#INCLUDEPATH -=  /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64

# htmlcxx
INCLUDEPATH +=  ./include \
                /usr/include/htmlcxx \
                /usr/include/htmlcxx/css \
                /usr/include/htmlcxx/html
LIBS += /usr/lib/x86_64-linux-gnu/libhtmlcxx.so

# opencv
INCLUDEPATH +=  /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2
LIBS += /usr/local/lib/libopencv_*.so

