TARGET = monitor

DEPENDPATH += .

INCLUDEPATH += .

SOURCES += application.cpp settings.cpp ui.cpp

HEADERS += application.h settings.h ui.h monitor.h

RESOURCES += monitor.qrc

unix {
	QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Werror

	SOURCES += "monitor-linux.cpp"
}

win32 {
	RC_FILE += monitor.rc

	SOURCES += "monitor-win32.cpp"
}
