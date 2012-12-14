#pragma once

#include <windows.h>

#define MONITOR_NAME L"Monitor"

#define MONITOR_VERSION 1,0

#define MONITOR_VERSION_STR L"1.0"

// .rc files do not accept concatenated strings, i.e., "Hello" " " "world".
// This means I can not use MONITOR_NAME L" v" MONITOR_VERSION.
// Thus the following definition.

#define MONITOR L"Monitor v1.0"

#define MONITOR_AUTHOR L"Copyright © 2012 Christopher Hoobin"

#define MONITOR_DESCRIPTION L"A simple lightweight network monitor"

#define MONITOR_URL L"https://github.com/choobin/monitor"

#define MONITOR_FILENAME L"Monitor.exe"

#define MONITOR_ICON L"monitor.ico"

#define TIMEOUT_INTERVAL 1000

#define IDI_ICON 1000

#define IDD_ABOUT 1001

#define IDC_STATIC 1002
