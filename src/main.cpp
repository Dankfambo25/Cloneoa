#if _WIN32 || _WIN64 || defined(WIN32) || defined(__NT__)
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdint.h>

#pragma region data
typedef unsigned char byte;
typedef unsigned int number; // <-- lol
typedef char* string;
// in case if i want to change these types later
// forgot LPSTR (no C) existed

struct POINT32 {
	unsigned int x, y;
};

struct SIZE32 {
	unsigned int cx, cy;
};

static const string title = "Cloneoa", buildtime[] = { __DATE__, __TIME__ };

class object {

};

struct PLAYERINFO {
	POINT32 pos;
	byte ctrl; //fast8
	byte*keys;
	number lives, gems, stars, health; //fast32
};

struct VISION {
	object*objects;
	SIZE32 size;
};

#define LEVEL VISION

static const USHORT VISVER = 0;

#pragma region ctrl defines
#define CTRL_START	0b01000000
#define CTRL_FIRE	0b00100000
#define CTRL_JUMP	0b00010000
#define CTRL_LEFT	0b00001000
#define CTRL_UP		0b00000100
#define CTRL_DOWN	0b00000010
#define CTRL_RIGHT	0b00000001
#pragma endregion

static /*const*/ SIZE32 res;
static const SIZE32 resmin = { 160, 80 };

enum objtype {
	item,
	entity,
	door,
	sign,
	fore = 0xF,
	back = 0xE
};

enum item {
	gem,
	diamond,
	heart,
	star,
	life
};

enum entity {
	mu,
	mufly,
	box,
	ball
};

enum door {
	goal,
	locked0,
	locked1,
	warp
};

enum lvlfore {
	ground,
	damage,
	turbulance
};

enum lvlback {
	fence
};

// i dont know why im adding things that allow two or more players in memory

#define PLAYERCOUNT 1

#define LOGGING 0

#if PLAYERCOUNT > 1
static PLAYERINFO player[PLAYERCOUNT];
#else
static PLAYERINFO player;
#endif

const unsigned long strbufsz = 512;
string strbuf = new char[strbufsz];
#pragma endregion

#if _WIN32 || _WIN64 || defined(WIN32) || defined(__NT__)
#pragma region beta windows

static const string __static = "static", button = "button";
static char ofname[MAX_PATH];
static string ini;

static HWND hWndmain, hWndCFG, menu[10], confctrls[20];
static POINT32 center;

static OPENFILENAME ofn;

#if LOGGING
static HWND stdoutdisp;
static FILE*logdata[3];
#endif

#define FONTS_ENABLED 1
#if FONTS_ENABLED
HFONT arial;
#endif

#if FONTS_ENABLED
void SetWindowFont(HWND ctrl, HFONT font) {
	SendMessage(ctrl, WM_SETFONT, (LPARAM)font, 1);
}
#endif

void fileAssoc() {
	DWORD disp;
	HKEY regext, regclass, regtemp;
	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, ".klo",
		0, NULL, 0, KEY_WRITE, NULL, &regext, &disp) == ERROR_SUCCESS)
		if (RegSetValue(regext, 0, REG_SZ,
			"Cloneoa.KLO", 12) == ERROR_SUCCESS)
			if (RegCloseKey(regext) != ERROR_SUCCESS)
				fprintf(stderr,"/!\\ %s", "Closing the extension key failed, how weird is that");
			else { }
		else
			fprintf(stderr,"/!\\ %s", "Setting extension value failed");
	else
		fprintf(stderr, "/!\\ %s", "Creating extension key failed");
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcConf(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	static WNDCLASSEX wc, wc2;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)5;
	wc.lpszClassName = title;

	RegisterClassEx(&wc);
	
	GetCurrentDirectoryA(strbufsz, strbuf);
	sprintf(strbuf, "%s\\settings.ini", strbuf);
	ini = strbuf;

	res.cx = GetPrivateProfileInt("GFX", "ResX", 360, ini);
	res.cy = GetPrivateProfileInt("GFX", "ResY", 320, ini);

	center.x = GetSystemMetrics(SM_CXSCREEN) / 2 - res.cx / 2;
	center.y = GetSystemMetrics(SM_CYSCREEN) / 2 - res.cy / 2;

	hWndmain = CreateWindowExA(NULL,title,title,
		WS_OVERLAPPED | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MINIMIZE,
		center.x,center.y,res.cx + 6,res.cy + 34,NULL,NULL,hInstance,NULL);

#pragma region freaking menu
	menu[0] = CreateWindowA(__static, "KLONOA\n    CUSTOM LEVEL ENGINE",
		WS_CHILD | WS_VISIBLE, 12, 12, 176, 32, hWndmain,
		0, wc.hInstance, 0);

	menu[1] = CreateWindowA(button, "Load Vision...",
		WS_CHILD | WS_VISIBLE, 20, 64, 104, 24, hWndmain,
		(HMENU)0, wc.hInstance, 0);

	menu[2] = CreateWindowA(button, "Settings...",
		WS_CHILD | WS_VISIBLE, 20, 98, 104, 24, hWndmain,
		(HMENU)1, wc.hInstance, 0);

	menu[3] = CreateWindowA(__static, "Level",
		WS_CHILD | WS_VISIBLE, 25, 137, 37, 16, hWndmain,
		0, wc.hInstance, 0);

	menu[4] = CreateWindowA(button, "Editor",
		WS_CHILD | WS_VISIBLE, 70, 134, 76, 24, hWndmain,
		(HMENU)2, wc.hInstance, 0);

	menu[5] = CreateWindowA(button, "Analyzer",
		WS_CHILD | WS_VISIBLE, 70, 165, 76, 24, hWndmain,
		(HMENU)3, wc.hInstance, 0);

	menu[6] = CreateWindowA(button, "Directory",
		WS_CHILD | WS_VISIBLE, 70, 196, 76, 24, hWndmain,
		(HMENU)4, wc.hInstance, 0);

	menu[7] = CreateWindowA(__static, "developed by donnaken15",
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWndmain, 0, wc.hInstance, 0);

	sprintf_s(strbuf,strbufsz,"v0.04 | buildtime: %s, %s",buildtime[0],buildtime[1]);

	menu[8] = CreateWindowA(__static, strbuf,
		WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWndmain, 0, wc.hInstance, 0);

#if FONTS_ENABLED
	arial = CreateFontA(17, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0x20, "Arial");
	SetWindowFont(menu[0], arial);
	SetWindowFont(menu[1], arial);
	SetWindowFont(menu[2], arial);
	SetWindowFont(menu[3], arial);
	SetWindowFont(menu[4], arial);
	SetWindowFont(menu[5], arial);
	SetWindowFont(menu[6], arial);
	SetWindowFont(menu[7], arial);
	SetWindowFont(menu[8], arial);
#endif

	ZeroMemory(&ofname, sizeof(ofname));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = "Level File (*.klo)\0*.klo\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = ofname;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Load Level";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

#pragma region cfg diag
	ZeroMemory(&wc2, sizeof(WNDCLASSEX));

	wc2.cbSize = sizeof(WNDCLASSEX);
	wc2.style = CS_HREDRAW | CS_VREDRAW;
	wc2.lpfnWndProc = WindowProcConf;
	wc2.hInstance = hInstance;
	wc2.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc2.hbrBackground = (HBRUSH)5;
	wc2.lpszClassName = "CloneaConf";

	RegisterClassEx(&wc2);

	hWndCFG = CreateWindowExA(NULL, wc2.lpszClassName,
		"Settings", WS_OVERLAPPED | WS_SYSMENU | WS_DLGFRAME,
		GetSystemMetrics(SM_CXSCREEN) / 2 - 270 / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2 - 300 / 2,
		270, 340, hWndmain, NULL, hInstance, NULL);
	//ShowWindow(hWndCFG, SW_HIDE);
#pragma endregion
#pragma region config *literal* controls
	confctrls[0] = CreateWindowA(__static, "Graphics",
		WS_CHILD | WS_VISIBLE, 12, 12, 176, 17, hWndCFG,
		0, wc.hInstance, 0);
	confctrls[1] = CreateWindowA(__static, "Sound",
		WS_CHILD | WS_VISIBLE, 12, 96, 176, 17, hWndCFG,
		0, wc.hInstance, 0);
	confctrls[2] = CreateWindowA(__static, "System",
		WS_CHILD | WS_VISIBLE, 12, 180, 176, 17, hWndCFG,
		0, wc.hInstance, 0);
	confctrls[3] = CreateWindowA(button, "Write File Associations",
		WS_CHILD | WS_VISIBLE, 16, 208, 166, 26, hWndCFG,
		0, wc.hInstance, 0);
#if FONTS_ENABLED
	SetWindowFont(confctrls[0], arial);
	SetWindowFont(confctrls[1], arial);
	SetWindowFont(confctrls[2], arial);
	SetWindowFont(confctrls[3], arial);
#endif
#pragma endregion
#pragma endregion


#if LOGGING
	for (int i = 0; i < 3; i++)
		logdata[i] = freopen("in.txt", "w", (&__iob_func()[i]));
#endif

	ShowWindow(hWndmain, nCmdShow);

	static MSG msg;
	
	while (TRUE)
	{
		while (GetMessageA(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/*while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			Sleep(1);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}*/

		if ((msg.message == WM_QUIT ||
			msg.message == WM_CLOSE ||
			msg.message == WM_DESTROY))
			break;
	}

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (wParam) {
		case 0:
			if (GetOpenFileNameA(&ofn))
			{

			}
			break;
		case 1:
			ShowWindow(hWndCFG, SW_SHOW);
			EnableWindow(hWndmain, 0);
			break;
		}
		break;
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = resmin.cx;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = resmin.cy;
		break;
	case WM_PAINT:

		break;
	case WM_SIZE:
		res.cx = LOWORD(lParam) - 6;
		res.cy = HIWORD(lParam) + 34;
		SetWindowPos(menu[7], 0, 12, HIWORD(lParam) - 52, 176, 17, 0);
		SetWindowPos(menu[8], 0, 12, HIWORD(lParam) - 30, 270, 17, 0);
		break;
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
	{
#if LOGGING
		for (int i = 0; i < 3; i++)
			fclose(logdata[i]);
#endif
		PostQuitMessage(0);
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WindowProcConf(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (wParam) {
		case 0:
			fileAssoc();
			break;
		case 1:
			break;
		}
		break;
	case WM_CLOSE:
	{
		EnableWindow(hWndmain, 1);
		ShowWindow(hWndCFG, SW_HIDE);
		return 1;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

#pragma endregion

#elif unix || __unix || __unix__ || __linux__ || linux
#pragma region alpha penguin

#pragma endregion

#elif (unix || __unix || __unix__) && (__APPLE__ || __MACH__ || TARGET_OS_MAC)
#pragma region omega apple

#pragma endregion

#else

LPCSTR error = "Either broken preprocessor directive or an unsupported system, nice going, dingus";
int main() {
	puts(error);
	exit(999);
}

#endif
