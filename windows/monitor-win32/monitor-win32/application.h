#pragma once

#include <windows.h>

#include <vector>
#include <string>

#include "monitor.h"
#pragma comment(lib, "monitor.lib")

struct Application {
	Monitor monitor;
	Data data;

	NOTIFYICONDATA tray_icon;

	struct {
	    HFONT type;
	    COLORREF color;
	} font;

	HMENU menu;
	struct {
		HMENU interfaces;
		HMENU placement;
	} submenu;

	std::vector<std::wstring> interface_names;
	int interface_id;
		
	int placement_id;
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