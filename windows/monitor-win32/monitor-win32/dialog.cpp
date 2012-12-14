#include <windows.h>

#include "application.h"
#include "dialog.h"
#include "resource.h"

static BOOL CALLBACK
about_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (uMsg == WM_COMMAND)
		EndDialog(hwnd, IDD_ABOUT);
	
	return 0;
}

void
show_about(HWND hwnd)
{	
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, about_callback);
}

void
select_font(HWND hwnd)
{
	LOGFONT logfont;

	GetObject(application.font.type, sizeof logfont, &logfont);

	CHOOSEFONT choosefont;

	ZeroMemory(&choosefont,  sizeof choosefont);

	choosefont.lStructSize = sizeof choosefont;
	choosefont.hwndOwner = hwnd;
	choosefont.lpLogFont = &logfont;
	choosefont.iPointSize = 0;
	choosefont.Flags = CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	choosefont.rgbColors = RGB(0,0,0);
	choosefont.lCustData = 0L;
	choosefont.lpfnHook = NULL;
	choosefont.lpTemplateName = NULL;
	choosefont.hInstance = NULL;
	choosefont.lpszStyle = NULL;
	choosefont.nFontType = SCREEN_FONTTYPE;
	choosefont.nSizeMin = 0;
	choosefont.nSizeMax = 0;

	BOOL value = ChooseFont(&choosefont);

	if (value == FALSE)
		return;

	choosefont.lpLogFont->lfQuality = NONANTIALIASED_QUALITY;

	application.font.type = CreateFontIndirect(choosefont.lpLogFont);

	update_application(hwnd);
}

void
select_color(HWND hwnd)
{
	CHOOSECOLOR choosecolor;
	COLORREF custom_colors[16];

	ZeroMemory(&choosecolor, sizeof choosecolor);

	choosecolor.lStructSize = sizeof choosecolor;
	choosecolor.hwndOwner = hwnd;
	choosecolor.lpCustColors = (LPDWORD)custom_colors;
	choosecolor.rgbResult = application.font.color;
	choosecolor.Flags = CC_FULLOPEN | CC_RGBINIT;

	BOOL value = ChooseColor(&choosecolor);

	if (value == FALSE)
		return;

	application.font.color = choosecolor.rgbResult;

	update_application(hwnd);
}