#pragma once

#include <windows.h>

#include <vector>
#include <string>

#include "monitor.h"
#pragma comment(lib, "monitor.lib")

#include "settings.h"

struct Application {
    Monitor monitor;

    Data data;

    Settings settings;

    NOTIFYICONDATA tray_icon;

    HMENU menu;
    struct {
        HMENU interfaces;
        HMENU units;
        HMENU prefix;
        HMENU placement;
    } submenu;
};

extern Application application;

void
init_application(HWND);

void
free_application(HWND);

void
update_application(HWND);

void
paint_application(HWND);
