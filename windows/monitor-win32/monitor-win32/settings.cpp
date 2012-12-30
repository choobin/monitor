#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <Shlwapi.h> // PathAppend, PathFileExists
#pragma comment(lib, "Shlwapi.lib")

#include <shlobj.h> // SHGetKnownFolderPath

#include <vector>
#include <string>
using std::vector;
using std::wstring;

#include "application.h"
#include "menu.h"
#include "resource.h"
#include "settings.h"

#define ___IO(operation, handle, value, nbytes) do {                \
    DWORD read;                                                     \
    operation(handle, value, nbytes, &read, NULL);                  \
    assert(read == nbytes);                                         \
__pragma(warning(push))                                             \
__pragma(warning(disable:4127))                                     \
    } while (0)  /* C4127: conditional expression is constant. */   \
__pragma(warning(pop))

#define Read(handle, value) ___IO(ReadFile, handle, &value, sizeof value)

#define Write(handle, value) ___IO(WriteFile, handle, &value, sizeof value)

#define ReadPtr(handle, value, nbytes) ___IO(ReadFile, handle, value, nbytes)

#define WritePtr(handle, value, nbytes) ___IO(WriteFile, handle, value, nbytes)

static void
default_settings()
{
    application.settings.interface_id = ID_MENU_INTERFACE;

    application.settings.units_id = ID_MENU_UNITS_KPS;

    application.settings.prefix_id = ID_MENU_PREFIX_SI;

    application.settings.placement_id = ID_MENU_PLACEMENT_TR;

    application.settings.font.family = CreateFont(
        14,
        0,
        0,
        0,
        FW_NORMAL, // cWeight
        FALSE,     // bItalic
        FALSE,     // bUnderline
        FALSE,     // bStrikeout
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        NONANTIALIASED_QUALITY, // Note: ClearType/Anti-aliased fonts look really shitty on a transparent background.
        DEFAULT_PITCH,
        L"Courier New");

    // TODO What if the desktop background is white?
    // Computer the average color of the desktop background and adjust default color to white or black?
    application.settings.font.color = RGB(255, 255, 255);
}

static void
save_path(wchar_t *path)
{
    wchar_t *ptr;

    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &ptr);

    wcsncpy_s(path, BUFSIZ, ptr, _TRUNCATE);

    CoTaskMemFree(ptr);

    PathAppend(path, MONITOR_NAME);

    if (!PathFileExists(path)) {
        CreateDirectory(path, NULL);
    }

    PathAppend(path, L"settings");
}

void
load_settings()
{
    vector<wstring> interface_names = application.monitor.interfaces();

    wchar_t path[BUFSIZ];

    save_path(path);

    HANDLE file = CreateFile(
        path,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (file == INVALID_HANDLE_VALUE) {
        default_settings();
        return;
    }

    uint16_t magic;

    Read(file, magic);

    if (magic != MONITOR_MAGIC) {
        default_settings();
        return;
    }

    Read(file, application.settings.interface_id);

    size_t length = 0;

    Read(file, length);

    wchar_t buffer[BUFSIZ] = {0};

    ReadPtr(file, buffer, length);

    if (wcsncmp(buffer, interface_names[INTERFACE_INDEX(application.settings.interface_id)].c_str(), length) != 0) {
        // If the interface string does not match the value of the saved index there has been a hardware change.
        // So we reset the value to the default 'All Interfaces' value (learnt this on the hard way :D).
        application.settings.interface_id = ID_MENU_INTERFACE;
    }

    application.monitor.switch_interface(INTERFACE_INDEX(application.settings.interface_id));

    Read(file, application.settings.units_id);

    Read(file, application.settings.prefix_id);

    Read(file, application.settings.placement_id);

    LOGFONT logfont;

    Read(file, logfont);

    application.settings.font.family = CreateFontIndirect(&logfont);

    Read(file, application.settings.font.color);

    CloseHandle(file);
}

void
save_settings()
{
    vector<wstring> interface_names = application.monitor.interfaces();

    wchar_t path[BUFSIZ];

    save_path(path);

    HANDLE file = CreateFile(
        path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    assert(file != INVALID_HANDLE_VALUE);

    uint16_t magic = MONITOR_MAGIC;

    Write(file, magic);

    Write(file, application.settings.interface_id);

    size_t length = interface_names[INTERFACE_INDEX(application.settings.interface_id)].length();

    length *= sizeof(wchar_t);

    Write(file, length);

    WritePtr(file, interface_names[INTERFACE_INDEX(application.settings.interface_id)].c_str(), length);

    Write(file, application.settings.units_id);

    Write(file, application.settings.prefix_id);

    Write(file, application.settings.placement_id);

    LOGFONT logfont;

    GetObject(application.settings.font.family, sizeof logfont, &logfont);

    Write(file, logfont);

    Write(file, application.settings.font.color);

    CloseHandle(file);
}
