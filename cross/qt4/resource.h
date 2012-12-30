#ifndef RESOURCE_H
#define RESOURCE_H

#define MONITOR_APPLICATION_NAME "Monitor"

#define MONITOR_ORGINIZATION_NAME "Moongiraffe"

#define MONITOR_ORGINIZATION_DOMAIN "moongiraffe.net"

#define MONITOR_VERSION 1,0

#define MONITOR_VERSION_STR "1.0"

#define MONITOR_AUTHOR "Copyright © 2012 Christopher Hoobin"

#define MONITOR_DESCRIPTION "A simple lightweight network monitor."

#define MONITOR_URL "https://github.com/choobin/monitor"

#define MONITOR_FILENAME "monitor.exe"

#define MONITOR_SVG ":/monitor.svg" // A path into the QT resource file (monitor.qrc)

#define MONITOR_ICO "monitor.ico" // For the win23 resource file (monitor.rc)

#if defined WIN32
#define MONITOR_DEFAULT_INTERFACE "All interfaces"
#else
#define MONITOR_DEFAULT_INTERFACE "unknown"
#endif

#define MONITOR_DEFAULT_UNITS Settings::Kps

#define MONITOR_DEFAULT_PREFIX Settings::Si

#define MONITOR_DEFAULT_PREFIX_DISPLAY true

#define MONITOR_DEFAULT_POSITION Settings::TopLeft

#if defined WIN32
#define MONITOR_DEFAULT_FONT_FAMILY "Consolas"
#else
#define MONITOR_DEFAULT_FONT_FAMILY "Monospace"
#endif

#define MONITOR_DEFAULT_FONT_SIZE 10

#define MONITOR_DEFAULT_FONT_COLOR Qt::white

#endif /* RESOURCE_H */
