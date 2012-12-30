#include <windows.h>

#include "application.h"
#include "dialog.h"
#include "menu.h"
#include "resource.h"

static LRESULT CALLBACK
window_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        init_application(hwnd);

        update_application(hwnd);

        break;

    case WM_PAINT:
        paint_application(hwnd);

        break;

    case WM_TIMER:
        application.data = application.monitor.poll();

        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

        break;

    case WM_NOTIFYICON:
        if (lParam == WM_LBUTTONDOWN) {
            if (IsWindowVisible(hwnd)) {
                ShowWindow(hwnd, SW_HIDE);
            }
            else {
                ShowWindow(hwnd, SW_SHOW);
            }
        }

        if (lParam == WM_RBUTTONDOWN) {
            show_menu(hwnd);
        }

        break;

    case WM_DESTROY:
        free_application(hwnd);

        PostQuitMessage(0);

        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

static void
register_class(HINSTANCE hInstance)
{
    WNDCLASSEX wnd;

    ZeroMemory(&wnd, sizeof wnd);

    wnd.cbSize = sizeof wnd;

    wnd.style = CS_HREDRAW | CS_VREDRAW;

    wnd.lpfnWndProc = window_procedure;

    wnd.cbClsExtra = 0;

    wnd.cbWndExtra = 0;

    wnd.hInstance = hInstance;

    wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

    wnd.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);

    wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    wnd.lpszMenuName = NULL;

    wnd.lpszClassName = NULL;

    wnd.lpszClassName = MONITOR_NAME;

    RegisterClassEx(&wnd);
}

static void
create_window(HINSTANCE hInstance, int nCmdShow)
{
    HWND hwnd = CreateWindowEx(
        // http://msdn.microsoft.com/en-us/library/windows/desktop/ff700543%28v=vs.85%29.aspx
        WS_EX_LAYERED,
        MONITOR_NAME,
        MONITOR_NAME,
        // http://msdn.microsoft.com/en-us/library/windows/desktop/ms632600%28v=vs.85%29.aspx
        WS_VISIBLE | WS_POPUP,
        CW_USEDEFAULT, // x
        CW_USEDEFAULT, // y
        CW_USEDEFAULT, // width
        CW_USEDEFAULT, // height
        NULL,
        NULL,
        hInstance,
        NULL);

    SetTimer(hwnd, ID_TIMER, TIMEOUT_INTERVAL, NULL);

    ZeroMemory(&application.tray_icon, sizeof application.tray_icon);

    application.tray_icon.cbSize = sizeof application.tray_icon;

    application.tray_icon.hWnd = hwnd;

    application.tray_icon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_GUID;

    application.tray_icon.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

    wcscpy_s(application.tray_icon.szTip, MONITOR_NAME);

    application.tray_icon.uCallbackMessage = WM_NOTIFYICON;

    Shell_NotifyIcon(NIM_ADD, &application.tray_icon);

    SetLayeredWindowAttributes(hwnd, RGB(0, 255, 255), 255, LWA_COLORKEY);

    ShowWindow(hwnd, nCmdShow);
}

static WPARAM
message_loop()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);

    register_class(hInstance);

    create_window(hInstance, nCmdShow);

    return message_loop();
}
