TEMPLATE = app
TARGET = ../bin/qshapes
DEPENDPATH += . ../ui
INCLUDEPATH += . 

# include qtcanvas 
include(./qtcanvas-opensource-4.1.0/src/qtcanvas.pri)

#build with debug support
CONFIG += debug

QMAKE_CXXFLAGS += -g

win32 {
	RC_FILE = ../winrc.rc
}

!win32 {
	QMAKE_CXXFLAGS += -DUNIXHOST__
}

mac{
	ICON = ../images/qshapes.icns
}

QT += xml

# Input
FORMS = ../ui/FormProperty.ui ../ui/FormAssociateFile.ui ../ui/FormAbout.ui

HEADERS += Action.h \
           ActionCombo.h \
	   ActionConnectionLine.h \
	   ActionConnectionBox.h \
	   ActionItem.h \
	   ActionMoveResize.h \
           ActionManager.h \
	   ActionProperties.h \
	   BoxPosAndSize.h \
	   DlgAbout.h \
	   DlgAssociateFile.h \
	   DlgProperty.h \
	   LinePosAndSize.h \
	   PosAndSize.h \
           ConnectionCollector.h \
           ClipboardManager.h \
           DiagramBox.h \
           DiagramItem.h \
           DiagramItemCollector.h \
           DiagramLine.h \
           DiagramSupport.h \
           DiagramView.h \
           DlgMain.h \
           ItemConnection.h \
	   ProductBox.cpp \
	   SelectionBox.h \
	   SelectionManager.h \
	   SimplifiedConnection.h \
           SuperSystem.h \
	   SystemBox.h \
	   XmlSupport.h 
SOURCES += Action.cpp \
           ActionCombo.cpp \
	   ActionConnectionLine.cpp \
	   ActionConnectionBox.cpp \
	   ActionItem.cpp \
	   ActionMoveResize.cpp \
           ActionManager.cpp \
	   ActionProperties.cpp \
           BoxPosAndSize.cpp \
	   DlgAbout.cpp \
	   DlgAssociateFile.cpp \
	   DlgProperty.cpp \
	   LinePosAndSize.cpp \
	   PosAndSize.cpp \
	   ConnectionCollector.cpp \
           ClipboardManager.cpp \
           DiagramBox.cpp \
           DiagramItem.cpp \
           DiagramItemCollector.cpp \
           DiagramLine.cpp \
           DiagramSupport.cpp \
           DiagramView.cpp \
           DlgMain.cpp \
           ItemConnection.cpp \
           main.cpp \
	   ProductBox.cpp \
	   SelectionBox.cpp \
	   SelectionManager.cpp \
	   SimplifiedConnection.cpp \
	   SuperSystem.cpp \ 
           SystemBox.cpp \
	   XmlSupport.cpp
RESOURCES += ../resources.qrc
