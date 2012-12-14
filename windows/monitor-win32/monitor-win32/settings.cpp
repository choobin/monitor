#include <windows.h>
#include <stdio.h>

#include <Shlwapi.h> // PathAppend, PathFileExists
#pragma comment(lib, "Shlwapi.lib")

#include <shlobj.h> // SHGetKnownFolderPath

#include "application.h"
#include "menu.h"
#include "resource.h"
#include "settings.h"

static void 
default_settings()
{
	application.font.type = CreateFont(
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

	// XXX What if the desktop background is white?
	application.font.color = RGB(255, 255, 255); 
		
	application.placement_id = ID_MENU_PLACEMENT_TR;

	application.interface_id = ID_MENU_INTERFACE;
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
	application.interface_names = application.monitor.interfaces();

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

	DWORD nbytes;

	LOGFONT logfont;
	
	ReadFile(file, &logfont, sizeof logfont, &nbytes, NULL);

	application.font.type = CreateFontIndirect(&logfont);

	ReadFile(file, &application.font.color, sizeof application.font.color, &nbytes, NULL);

	ReadFile(file, &application.placement_id, sizeof application.placement_id, &nbytes, NULL);

	ReadFile(file, &application.interface_id, sizeof application.interface_id, &nbytes, NULL);

	size_t length = 0;

	ReadFile(file, &length, sizeof length, &nbytes, NULL);

	wchar_t buffer[BUFSIZ] = {0};

	ReadFile(file, buffer, length, &nbytes, NULL);
	
	if (wcsncmp(buffer, application.interface_names[INTERFACE_INDEX(application.interface_id)].c_str(), length) != 0) {
		// If the interface string does not match the value of the saved index there has been a hardware change. 
		// So we reset the value to the default 'All Interfaces' value (learnt this on the hard way :D).
		application.interface_id = ID_MENU_INTERFACE;
	}

	application.monitor.switch_interface(INTERFACE_INDEX(application.interface_id));

	CloseHandle(file);
}

void
save_settings()
{
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
	
	DWORD nbytes;

	LOGFONT logfont;

	GetObject(application.font.type, sizeof logfont, &logfont);

	WriteFile(file, &logfont, sizeof logfont, &nbytes, NULL);

	WriteFile(file, &application.font.color, sizeof application.font.color, &nbytes, NULL);

	WriteFile(file, &application.placement_id, sizeof application.placement_id, &nbytes, NULL);

	WriteFile(file, &application.interface_id, sizeof application.interface_id, &nbytes, NULL);

	size_t length = application.interface_names[INTERFACE_INDEX(application.interface_id)].length();

	length *= sizeof(wchar_t);

	WriteFile(file, &length, sizeof length, &nbytes, NULL);

	WriteFile(file, application.interface_names[INTERFACE_INDEX(application.interface_id)].c_str(), length, &nbytes, NULL);

	CloseHandle(file);
}