#pragma once

#include <windows.h>

enum {
    ID_MENU_PLACEMENT_TL = WM_USER + 1,
    ID_MENU_PLACEMENT_TR,
    ID_MENU_PLACEMENT_BL,
    ID_MENU_PLACEMENT_BR,
    ID_MENU_PLACEMENT,
    ID_MENU_EXIT,
    ID_MENU_ABOUT,
    ID_MENU_HIDESHOW,
    ID_MENU_SETTINGS,
    ID_MENU_FONT,
    ID_MENU_COLOR,
    ID_MENU_INTERFACE,
    ID_TIMER,
    WM_NOTIFYICON,
};

#define MENU_INDEX(x) (x - ID_MENU_PLACEMENT_TL)

#define INTERFACE_INDEX(x) (x - ID_MENU_INTERFACE)

extern const wchar_t *menu_text[];

void
construct_menu();

void
show_menu(HWND);
