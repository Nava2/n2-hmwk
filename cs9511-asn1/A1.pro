
TEMPLATE = app
TARGET = A1
INCLUDEPATH += . geometry/E2
QT += core gui svg printsupport

CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11

mac:CONFIG  -= app_bundle

# Input
HEADERS += GameWindow.hpp \
           RefinedQWidget.hpp \
           Tile.hpp \
           Composite.hpp \
           GeometryUtils.hpp \
           mainwindow.h
SOURCES += main.cpp \
           GameWindow.cpp \
           Tile.cpp \
           Composite.cpp \
           mainwindow.cpp

FORMS +=   mainwindow.ui
