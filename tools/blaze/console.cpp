#include "console.hpp"
#include <wincon.h>
#include <array>
#include <PathCch.h>
#include <io.h>


int BaseWriteConhostEx(HANDLE hConsole, WORD color, const wchar_t *buf, size_t len) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	WORD oldColor = csbi.wAttributes;
	SetConsoleTextAttribute(hConsole, color);
	DWORD dwWrite;
	WriteConsoleW(hConsole, buf, len, &dwWrite, nullptr);
	SetConsoleTextAttribute(hConsole, oldColor);
	return dwWrite;
}

int BaseMessagePrintEx(WORD color, const wchar_t *format, ...) {
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	std::wstring buffer(PATHCCH_MAX_CCH, 0);
	va_list ap;
	va_start(ap, format);
	auto l = _vswprintf_c_l(&buffer[0], buffer.capacity(), format, nullptr, ap);
	va_end(ap);
	return BaseWriteConhostEx(hConsole, color, buffer.data(), l);
}

int BaseWriteConhost(HANDLE hConsole, WORD fcolor, const wchar_t *buf, size_t len) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	WORD oldColor = csbi.wAttributes;
	WORD newColor = (oldColor & 0xF0) | fcolor;
	SetConsoleTextAttribute(hConsole, newColor);
	DWORD dwWrite;
	WriteConsoleW(hConsole, buf, len, &dwWrite, nullptr);
	SetConsoleTextAttribute(hConsole, oldColor);
	return dwWrite;
}

int BaseErrorMessagePrint(const wchar_t *format, ...) {
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	std::wstring buffer(PATHCCH_MAX_CCH, 0);
	va_list ap;
	va_start(ap, format);
	auto l = _vswprintf_c_l(&buffer[0], buffer.capacity(), format, nullptr, ap);
	va_end(ap);
	return BaseWriteConhost(hConsole, console::fc::LightRed, buffer.data(), l);
}

int BaseMessagePrint(WORD color, const wchar_t *format, ...) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	std::wstring buffer(PATHCCH_MAX_CCH,0);
	va_list ap;
	va_start(ap, format);
	auto l = _vswprintf_c_l(&buffer[0], buffer.capacity(), format, nullptr, ap);
	va_end(ap);
	return BaseWriteConhost(hConsole, color, buffer.data(), l);
}

bool IsUnderConhost(FILE *fp) {
	HANDLE hStderr = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(fp)));
	return GetFileType(hStderr) == FILE_TYPE_CHAR;
}

int BaseMessagePrint(const wchar_t *format, ...) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	static bool isatty__ = IsUnderConhost(stdout);
	std::wstring buffer(PATHCCH_MAX_CCH, 0);
	va_list ap;
	va_start(ap, format);
	auto l = _vswprintf_c_l(&buffer[0], buffer.capacity(), format, nullptr, ap);
	va_end(ap);
	DWORD dwWrite=0;
	if (isatty__) {
		WriteConsoleW(hConsole, buffer.data(), l, &dwWrite, nullptr);
	}else {
		dwWrite = (DWORD)fwrite(buffer.data(), sizeof(wchar_t), buffer.size(), stdout);
	}
	//WriteConsoleW(h)
	return (int)dwWrite;
}

bool EnableVTMode()
{
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		return false;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		return false;
	}
	return true;
}

void DoProgress(const std::wstring &label, int step, int total)
{
	//progress width

	const int pwidth = 72;

	//minus label len

	int width = pwidth - label.size();
	int pos = (step * width) / total;


	int percent = (step * 100) / total;

	//set green text color, only on Windows

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	wprintf(L"%s[", label.data());

	//fill progress bar with =

	for (int i = 0; i < pos; i++)  wprintf(L"%c", '#');

	//fill progress bar with spaces

	wprintf(L"% *c", width - pos + 1, ']');
	wprintf(L" %3d%%\r", percent);

	//reset text color, only on Windows

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x08);
}

ProgressBar::ProgressBar(const std::wstring &title)
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	coord = csbi.dwCursorPosition;
	SetCursorPosition(left, coord.Y);
	DWORD dwWrite = 0;
	WriteConsoleW(hConsole, title.data(), (DWORD)title.size(), &dwWrite, nullptr);
	SetCursorPosition(left, coord.Y + 1);
	oldcolor = csbi.wAttributes;
	SetConsoleTextAttribute(hConsole, bkcolor);
	std::wstring wstr(backlen, L' ');
	WriteConsoleW(hConsole, wstr.data(), wstr.size(), &dwWrite, nullptr);
	SetConsoleTextAttribute(hConsole, oldcolor);
	printf("\n");
}

void ProgressBar::Update(int64_t current, int64_t total, const std::wstring & msg)
{
	int i = (int)ceil(current / (double)total * 100);
	SetCursorPosition(left, coord.Y + 1);
	int count = (int)ceil((double)i / 100 * backlen);
	wchar_t buf[20];
	auto N=swprintf_s(buf, L" %d%% ",i);
	SetConsoleTextAttribute(hConsole, fccolor);
	DWORD dwWrite = 0;
	std::wstring wstr(count, L' ');
	WriteConsoleW(hConsole, wstr.data(), wstr.size(), &dwWrite, nullptr);
	SetCursorPosition(left + backlen + 1, coord.Y + 1);
	SetConsoleTextAttribute(hConsole, oldcolor);
	WriteConsoleW(hConsole, buf, N, &dwWrite, nullptr);
	SetCursorPosition(left + (backlen / 2 - msg.size()), coord.Y + 2);
	WriteConsoleW(hConsole, msg.data(), msg.size(), &dwWrite, nullptr);
	if (i >= 100)
		printf("\n");
}
