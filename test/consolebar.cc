#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <iostream>
#include <CommCtrl.h>

#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"User32.lib")

#define WINDOWEXSTYLE WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY
#define PROGRESSRATESTYLE WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE

bool ProgressConsoleWindow(){
	auto hWnd=GetConsoleWindow();
	if(hWnd==nullptr){
		fprintf(stderr,"no console window\n");
		return false;
	}
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT logFont = { 0 };
	GetObjectW(hFont, sizeof(logFont), &logFont);
	DeleteObject(hFont);
	hFont = NULL;

	logFont.lfHeight = 20;
	logFont.lfWeight = FW_NORMAL;
	wcscpy_s(logFont.lfFaceName, L"Segoe UI");
	hFont = CreateFontIndirectW(&logFont);
	auto LambdaCreateWindow = [&](LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
								  int X, int Y, int nWidth, int nHeight, HMENU hMenu)->HWND{
		auto hw = CreateWindowExW(WINDOWEXSTYLE, lpClassName, lpWindowName,
								  dwStyle, X, Y, nWidth, nHeight, hWnd, hMenu, 
								  GetModuleHandle(nullptr), nullptr);
		if (hw) {
			::SendMessageW(hw, WM_SETFONT, (WPARAM)hFont, 0);
		}
		return hw;
	};
	auto hProgress = LambdaCreateWindow(PROGRESS_CLASSW, 
		L"", PROGRESSRATESTYLE, 125, 180, 420, 27, nullptr);
	for(auto i=0;i<100;i++){
		Sleep(10);
		SendMessage(hProgress,PBM_SETPOS,i,0);
	}
	return true;
}

int wmain(){
	ProgressConsoleWindow();
	return 0;
}