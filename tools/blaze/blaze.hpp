///

#ifndef BLAZE_HPP
#define BLAZE_HPP
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <functional>

class BzStringA {
public:
	BzStringA(const wchar_t *wstr, int cp = CP_UTF8);
	BzStringA(const wchar_t *wstr, size_t length, int cp = CP_UTF8);
	BzStringA(const BzStringA &other) = delete;
	BzStringA(BzStringA &&other) {
		Release();
		data_ = other.data_;
		size_ = other.size_;
		other.data_ = nullptr;
		other.size_ = 0;
	}
	~BzStringA();

	const char *data()const {
		return data_;
	}
	size_t size()const { return size_; }
private:
	void Release();
	char *data_;
	size_t size_;
};


class BzString {
public:
	BzString(const char *str, int cp = CP_UTF8);
	BzString(const char *str, size_t length, int cp = CP_UTF8);
	BzString(const BzString &other)=delete;
	BzString(BzString &&other) {
		Release();
		data_ = other.data_;
		size_ = other.size_;
		other.data_ = nullptr;
		other.size_ = 0;
	}
	~BzString();
	const wchar_t *data()const {
		return data_;
	}
	size_t size()const { return size_; }
private:
	void Release();
	wchar_t *data_;
	size_t size_;
};

struct BlazeProgress {
	bool(*impl)(void *data, std::uint32_t rate, std::uint64_t bytes);
	void *userdate;
};

class BlazeManager {
private:
  void Finalize(){}
};



namespace console {
	namespace fc {
		enum Color :WORD {
			Black = 0,
			White = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
			Blue = FOREGROUND_BLUE,
			Green = FOREGROUND_GREEN,
			Red = FOREGROUND_RED,
			Yellow = FOREGROUND_RED | FOREGROUND_BLUE,
			Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
			LightWhite = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
			LightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
			LightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
			LightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
			LightYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
			LightMagenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
			LightCyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
		};
	}
	namespace bc {
		enum Color : WORD
		{
			Black = 0,
			White = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED,
			Blue = BACKGROUND_BLUE,
			Green = BACKGROUND_GREEN,
			Red = BACKGROUND_RED,
			Yellow = BACKGROUND_RED | BACKGROUND_GREEN,
			Magenta = BACKGROUND_RED | BACKGROUND_BLUE,
			Cyan = BACKGROUND_GREEN | BACKGROUND_BLUE,
			LightWhite = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
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

int BaseWriteConhostEx(HANDLE hConsole, WORD color, const wchar_t *buf, size_t len);
int BaseMessagePrintEx(WORD color, const wchar_t *format, ...);

/// Only foreground color
int BaseWriteConhost(HANDLE hConsole, WORD fcolor, const wchar_t *buf, size_t len);
int BaseMessagePrint(WORD color, const wchar_t *format, ...);
int BaseErrorMessagePrint(const wchar_t *format, ...);

/// no colored
int BaseMessagePrint(const wchar_t *format, ...);

#endif
