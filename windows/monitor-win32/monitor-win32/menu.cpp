#include <windows.h>

#include <vector>
#include <string>
using std::vector;
using std::wstring;

#include "application.h"
#include "dialog.h"
#include "menu.h"

const wchar_t *menu_text[] = {
    L"Top Left",
    L"Top Right",
    L"Bottom Left",
    L"Bottom Right",
    L"Placement",
    L"Exit",
    L"About",
    L"Hide/Show",
    L"Settings",
    L"Font",
    L"Color",
    L"Bps",
    L"Kps",
    L"Mps",
    L"Gps",
    L"Units",
    L"Bits",
    L"Si (xB/s)",
    L"IEC (xiB/s)",
    L"Prefix",
    L"Interface"
};

#define MenuItem(menu, id) AppendMenu(menu, MF_STRING, id, menu_text[MENU_INDEX(id)])

#define MenuAdd(menu, submenu, id) AppendMenu(menu, MF_POPUP, (UINT_PTR)submenu, menu_text[MENU_INDEX(id)])

#define MenuCheck(menu, id) ModifyMenu(menu, id, MF_STRING | MF_CHECKED, id, menu_text[MENU_INDEX(id)]);

#define MenuUncheck(menu, id) ModifyMenu(menu, id, MF_STRING | MF_UNCHECKED, id, menu_text[MENU_INDEX(id)]);

void
construct_menu()
{
    vector<wstring> interface_names = application.monitor.interfaces();

    application.menu = CreatePopupMenu();

    MenuItem(application.menu, ID_MENU_ABOUT);

    MenuItem(application.menu, ID_MENU_HIDESHOW);

    HMENU settings = CreatePopupMenu();

    application.submenu.interfaces = CreatePopupMenu();

    int count = 0;

    for (auto& i : interface_names) {
        AppendMenu(application.submenu.interfaces, MF_STRING, ID_MENU_INTERFACE + count, i.c_str());
        count++;
    }

    MenuCheck(application.submenu.interfaces, application.settings.interface_id);

    MenuAdd(settings, application.submenu.interfaces, ID_MENU_INTERFACE);

    application.submenu.units = CreatePopupMenu();

    MenuItem(application.submenu.units, ID_MENU_UNITS_BPS);

    MenuItem(application.submenu.units, ID_MENU_UNITS_KPS);

    MenuItem(application.submenu.units, ID_MENU_UNITS_MPS);

    MenuItem(application.submenu.units, ID_MENU_UNITS_GPS);

    MenuCheck(application.submenu.units, application.settings.units_id);

    MenuAdd(settings, application.submenu.units, ID_MENU_UNITS);

    application.submenu.prefix = CreatePopupMenu();

    MenuItem(application.submenu.prefix, ID_MENU_PREFIX_BITS);

    MenuItem(application.submenu.prefix, ID_MENU_PREFIX_SI);

    MenuItem(application.submenu.prefix, ID_MENU_PREFIX_IEC);

    MenuCheck(application.submenu.prefix, application.settings.prefix_id);

    MenuAdd(settings, application.submenu.prefix, ID_MENU_PREFIX);

    application.submenu.placement = CreatePopupMenu();

    MenuItem(application.submenu.placement, ID_MENU_PLACEMENT_TL);

    MenuItem(application.submenu.placement, ID_MENU_PLACEMENT_TR);

    MenuItem(application.submenu.placement, ID_MENU_PLACEMENT_BL);

    MenuItem(application.submenu.placement, ID_MENU_PLACEMENT_BR);

    MenuCheck(application.submenu.placement, application.settings.placement_id);

    MenuAdd(settings, application.submenu.placement, ID_MENU_PLACEMENT);

    MenuItem(settings, ID_MENU_FONT);

    MenuItem(settings, ID_MENU_COLOR);

    MenuAdd(application.menu, settings, ID_MENU_SETTINGS);

    MenuItem(application.menu, ID_MENU_EXIT);
}

void
show_menu(HWND hwnd)
{
    POINT cursor;
    GetCursorPos(&cursor);

    SetForegroundWindow(hwnd);

    int result = TrackPopupMenu(
        application.menu,
        TPM_RETURNCMD | TPM_NONOTIFY,
        cursor.x,
        cursor.y,
        0,
        hwnd,
        NULL);

    if (result == ID_MENU_ABOUT) {
        show_about(hwnd);
    }
    else if (result == ID_MENU_HIDESHOW) {
        if (IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
        }
        else {
            ShowWindow(hwnd, SW_SHOW);
        }
    }
    else if (result >= ID_MENU_INTERFACE) {
        vector<wstring> interface_names = application.monitor.interfaces();

        MenuUncheck(application.submenu.interfaces, application.settings.interface_id);

        application.settings.interface_id = result;

        MenuCheck(application.submenu.interfaces, application.settings.interface_id);

        application.monitor.switch_interface(INTERFACE_INDEX(application.settings.interface_id));

        update_application(hwnd);
    }
    else if (result >= ID_MENU_UNITS_BPS && result <= ID_MENU_UNITS_GPS) {
        MenuUncheck(application.submenu.units, application.settings.units_id);

        application.settings.units_id = result;

        MenuCheck(application.submenu.units, application.settings.units_id);

        update_application(hwnd);
    }
    else if (result >= ID_MENU_PREFIX_BITS && result <= ID_MENU_PREFIX_IEC) {
        MenuUncheck(application.submenu.prefix, application.settings.prefix_id);

        application.settings.prefix_id = result;

        MenuCheck(application.submenu.prefix, application.settings.prefix_id);

        update_application(hwnd);
    }
    else if (result >= ID_MENU_PLACEMENT_TL && result <= ID_MENU_PLACEMENT_BR) {
        MenuUncheck(application.submenu.placement, application.settings.placement_id);

        application.settings.placement_id = result;

        MenuCheck(application.submenu.placement, application.settings.placement_id);

        update_application(hwnd);
    }
    else if (result == ID_MENU_FONT) {
        select_font(hwnd);

        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
    }
    else if (result == ID_MENU_COLOR) {
        select_color(hwnd);

        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
    }
    else if (result == ID_MENU_EXIT) {
        free_application(hwnd);

        PostQuitMessage(0);
    }
}
