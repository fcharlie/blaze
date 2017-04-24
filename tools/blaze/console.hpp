#ifndef CONSOLE_HPP
#define CONSOLE_HPP
#pragma once
#include <Windows.h>
#include <string>
#include <cstdint>

class ProgressBar {
public:
	ProgressBar(const std::wstring &title);
	void Update(int64_t current,int64_t total,const std::wstring &msg);
private:
	HANDLE hConsole{ nullptr };
	int left = 10;
	int backlen = 50;
	WORD bkcolor = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
	WORD fccolor = BACKGROUND_GREEN;
	WORD oldcolor;
	COORD coord;

	void SetCursorPosition(short x, short y) {
		COORD res = { x,y };
		SetConsoleCursorPosition(hConsole, res);
	}
	COORD GetCursorPosition() {
		CONSOLE_SCREEN_BUFFER_INFO res;
		GetConsoleScreenBufferInfo(hConsole, &res);
		return res.dwCursorPosition;
	}
};

namespace console {
	namespace fc {
		enum Color : WORD {
			Black = 0,
			Blue = FOREGROUND_BLUE,
			Green = FOREGROUND_GREEN,
			Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
			Red = FOREGROUND_RED,
			Purple = FOREGROUND_RED | FOREGROUND_BLUE,
			Yellow=FOREGROUND_RED|FOREGROUND_GREEN,
			White = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
			Gray= FOREGROUND_INTENSITY,
			LightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
			LightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
			LightWhite = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED |FOREGROUND_INTENSITY,
			LightCyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
			LightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
			LightMagenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
			LightYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
		};
	}
	namespace bc {
		enum Color : WORD {
			Black = 0,
			White = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED,
			Blue = BACKGROUND_BLUE,
			Green = BACKGROUND_GREEN,
			Red = BACKGROUND_RED,
			Yellow = BACKGROUND_RED | BACKGROUND_GREEN,
			Magenta = BACKGROUND_RED | BACKGROUND_BLUE,
			Cyan = BACKGROUND_GREEN | BACKGROUND_BLUE,
			LightWhite = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED |
			BACKGROUND_INTENSITY,
			LightBlue = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
			LightGreen = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
			LightRed = BACKGROUND_RED | BACKGROUND_INTENSITY,
			LightYellow = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
			LightMagenta = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
			LightCyan = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		};
	}
}

////

int BaseWriteConhostEx(HANDLE hConsole, WORD color, const wchar_t *buf,
	size_t len);
int BaseMessagePrintEx(WORD color, const wchar_t *format, ...);

/// Only foreground color
int BaseWriteConhost(HANDLE hConsole, WORD fcolor, const wchar_t *buf,
	size_t len);
int BaseMessagePrint(WORD color, const wchar_t *format, ...);
int BaseErrorMessagePrint(const wchar_t *format, ...);

/// no colored
int BaseMessagePrint(const wchar_t *format, ...);

#endif
