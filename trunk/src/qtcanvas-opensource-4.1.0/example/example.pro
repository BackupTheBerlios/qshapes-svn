TEMPLATE = app
TARGET = canvas

include(../src/qtcanvas.pri)

HEADERS += canvas.h
SOURCES += canvas.cpp main.cpp
RESOURCES += example.qrc
