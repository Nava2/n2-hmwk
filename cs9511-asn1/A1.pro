
TEMPLATE = app
TARGET = A1
INCLUDEPATH += . geometry/E2
QT += core gui svg printsupport

CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -stdlib=libc++

mac:CONFIG  -= app_bundle

# Input
HEADERS += GameWindow.hpp \
           RefinedQWidget.hpp \
           Tile.hpp \
           geometry/E2/Geometry_E2.hpp \
           geometry/E2/Point_E2.hpp \
           geometry/E2/Triangle_E2.hpp \
           misc/determinant.hpp \
    Composite.hpp
SOURCES += main.cpp \
    GameWindow.cpp \
    Tile.cpp \
    Composite.cpp
