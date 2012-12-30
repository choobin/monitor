#include <windows.h>

#include "application.h"
#include "menu.h"
#include "settings.h"

Application application;

void
init_application(HWND hwnd)
{
    UNREFERENCED_PARAMETER(hwnd);

    load_settings();

    construct_menu();
}

void
free_application(HWND hwnd)
{
    save_settings();

    DestroyMenu(application.submenu.interfaces);

    DestroyMenu(application.submenu.units);

    DestroyMenu(application.submenu.prefix);

    DestroyMenu(application.submenu.placement);

    DestroyMenu(application.menu);

    DeleteObject(application.settings.font.family);

    Shell_NotifyIcon(NIM_DELETE, &application.tray_icon);

    KillTimer(hwnd, ID_TIMER);
}

void
update_application(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(hwnd, &ps);

    SelectObject(hdc, application.settings.font.family);

    // XXX What if rx/tx value is greater than 8.2 digits?
    wchar_t buffer[BUFSIZ];
    swprintf_s(buffer, L"%8.2f", 0.0);

    SIZE size;
    GetTextExtentPoint32(hdc, buffer, wcslen(buffer), &size);

    // GetTextExtentPoint32 returns the height of a single line of text.
    // The monitor displays two lines (rx \n tx) so we multiply cy by two.
    size.cy = size.cy * 2;

    EndPaint(hwnd, &ps);

    RECT window;
    GetWindowRect(hwnd, &window);

    // Note: For multiple displays call GetMonitorInfo.
    RECT workingarea = {0, 0, 0, 0};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workingarea, 0);

    POINT screen;
    screen.x = workingarea.right - workingarea.left;
    screen.y = workingarea.bottom - workingarea.top;

    switch (application.settings.placement_id) {
    case ID_MENU_PLACEMENT_TL:
        MoveWindow(hwnd, 0, 0, size.cx, size.cy, TRUE);
        break;
    case ID_MENU_PLACEMENT_TR:
        MoveWindow(hwnd, screen.x - size.cx, 0, size.cx, size.cy, TRUE);
        break;
    case ID_MENU_PLACEMENT_BL:
        MoveWindow(hwnd, 0, screen.y - size.cy, size.cx, size.cy, TRUE);
        break;
    case ID_MENU_PLACEMENT_BR:
        MoveWindow(hwnd, screen.x - size.cx, screen.y - size.cy, size.cx, size.cy, TRUE);
        break;
    }
}

void
paint_application(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(hwnd, &ps);

    RECT client;
    GetClientRect(hwnd, &client);

    HBRUSH brush = CreateSolidBrush(RGB(0, 255, 255));
    FillRect(hdc, &client, brush);
    DeleteObject(brush);

    SetBkMode(hdc, TRANSPARENT);

    SelectObject(hdc, application.settings.font.family);

    RECT rect = {0, 0, 0, 0};
    SetTextColor(hdc, application.settings.font.color);

    double rx = static_cast<double>(application.data.rx);
    double tx = static_cast<double>(application.data.tx);
    double divisor = 0.0;

    switch (application.settings.prefix_id) {
    case ID_MENU_PREFIX_BITS:
        divisor = 1000.0;
        rx *= 8;
        tx *= 8;
        break;

    case ID_MENU_PREFIX_SI:
        divisor = 1000.0;
        break;

    case ID_MENU_PREFIX_IEC:
        divisor = 1024.0;
        break;
    }

    switch (application.settings.units_id) {
    case ID_MENU_UNITS_GPS:
        rx /= divisor;
        tx /= divisor;
        // Fall through.
    case ID_MENU_UNITS_MPS:
        rx /= divisor;
        tx /= divisor;
        // Fall through.
    case ID_MENU_UNITS_KPS:
        rx /= divisor;
        tx /= divisor;
        break;
    }

    wchar_t buffer[BUFSIZ];

    swprintf_s(buffer, L"%8.2f\n%8.2f", rx, tx);

    DrawText(hdc, buffer, -1, &rect, DT_NOCLIP);

    EndPaint(hwnd, &ps);
}
