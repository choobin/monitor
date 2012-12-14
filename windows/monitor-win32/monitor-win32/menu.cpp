#include <windows.h>

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
	L"Interface"
};

void 
construct_menu()
{
	application.menu = CreatePopupMenu();

	AppendMenu(application.menu, MF_STRING,	ID_MENU_ABOUT, menu_text[MENU_INDEX(ID_MENU_ABOUT)]);

	AppendMenu(application.menu, MF_STRING,	ID_MENU_HIDESHOW, menu_text[MENU_INDEX(ID_MENU_HIDESHOW)]);

	HMENU settings = CreatePopupMenu();

	application.submenu.interfaces = CreatePopupMenu();

	int count = 0;

	for (auto& i : application.interface_names) {
		AppendMenu(application.submenu.interfaces, MF_STRING, ID_MENU_INTERFACE + count, i.c_str());
		count++;
	}

	ModifyMenu(application.submenu.interfaces, application.interface_id, MF_STRING | MF_CHECKED, application.interface_id, application.interface_names[INTERFACE_INDEX(application.interface_id)].c_str());

	AppendMenu(settings, MF_POPUP, (UINT_PTR)application.submenu.interfaces, menu_text[MENU_INDEX(ID_MENU_INTERFACE)]);

	application.submenu.placement = CreatePopupMenu();

	AppendMenu(application.submenu.placement, MF_STRING, ID_MENU_PLACEMENT_TL, menu_text[MENU_INDEX(ID_MENU_PLACEMENT_TL)]);

	AppendMenu(application.submenu.placement, MF_STRING, ID_MENU_PLACEMENT_TR, menu_text[MENU_INDEX(ID_MENU_PLACEMENT_TR)]);
	
	AppendMenu(application.submenu.placement, MF_STRING, ID_MENU_PLACEMENT_BL, menu_text[MENU_INDEX(ID_MENU_PLACEMENT_BL)]);
	
	AppendMenu(application.submenu.placement, MF_STRING, ID_MENU_PLACEMENT_BR, menu_text[MENU_INDEX(ID_MENU_PLACEMENT_BR)]);

	ModifyMenu(application.submenu.placement, application.placement_id, MF_STRING | MF_CHECKED, application.placement_id, menu_text[MENU_INDEX(application.placement_id)]);
	
	AppendMenu(settings, MF_POPUP, (UINT_PTR)application.submenu.placement, menu_text[MENU_INDEX(ID_MENU_PLACEMENT)]);

	AppendMenu(settings, MF_STRING,	ID_MENU_FONT, menu_text[MENU_INDEX(ID_MENU_FONT)]);

	AppendMenu(settings, MF_STRING,	ID_MENU_COLOR, menu_text[MENU_INDEX(ID_MENU_COLOR)]);

	AppendMenu(application.menu, MF_POPUP, (UINT_PTR)settings,menu_text[MENU_INDEX(ID_MENU_SETTINGS)]);

	AppendMenu(application.menu, MF_STRING,	ID_MENU_EXIT, menu_text[MENU_INDEX(ID_MENU_EXIT)]);
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

	if (result == ID_MENU_EXIT) {
		free_application(hwnd);

		PostQuitMessage(0);
	}
	else if (result == ID_MENU_ABOUT) {
		show_about(hwnd);
	}
	else if (result == ID_MENU_HIDESHOW) {
		if (IsWindowVisible(hwnd))
			ShowWindow(hwnd, SW_HIDE);
		else
			ShowWindow(hwnd, SW_SHOW);
	}
	else if (result == ID_MENU_FONT) {
		select_font(hwnd);

		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	}
	else if (result == ID_MENU_COLOR) {
		select_color(hwnd);

		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	}
	else if (result >= ID_MENU_PLACEMENT_TL && result <= ID_MENU_PLACEMENT_BR) {
		ModifyMenu(application.submenu.placement, application.placement_id, MF_STRING | MF_UNCHECKED, application.placement_id, menu_text[MENU_INDEX(application.placement_id)]);
		
		application.placement_id = result;
		
		ModifyMenu(application.submenu.placement, application.placement_id, MF_STRING | MF_CHECKED, application.placement_id, menu_text[MENU_INDEX(application.placement_id)]);
		
		update_application(hwnd);
	}
	else if (result >= ID_MENU_INTERFACE) {
		ModifyMenu(application.submenu.interfaces, application.interface_id, MF_STRING | MF_UNCHECKED, application.interface_id, application.interface_names[INTERFACE_INDEX(application.interface_id)].c_str());
		
		application.interface_id = result;
		
		ModifyMenu(application.submenu.interfaces, application.interface_id, MF_STRING | MF_CHECKED, application.interface_id, application.interface_names[INTERFACE_INDEX(application.interface_id)].c_str());
		
		application.monitor.switch_interface(INTERFACE_INDEX(application.interface_id));

		update_application(hwnd);
	}
}