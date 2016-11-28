/////
#include <stdio.h>
#include <stdlib.h>
#include "rescle.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class ErrorMessage {
public:
	ErrorMessage(DWORD errid) :lastError(errid), release_(false)
	{
		if (FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, errid,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			(LPWSTR)&buf, 0, nullptr) == 0) {
			buf = L"Unknown error";
		} else {
			release_ = true;
		}
	}
	~ErrorMessage()
	{
		if (release_) {
			LocalFree(buf);
		}
	}
	const wchar_t *message()const
	{
		return buf;
	}
	DWORD LastError()const { return lastError; }
private:
	DWORD lastError;
	LPWSTR buf;
	bool release_;
	char reserved[sizeof(intptr_t) - sizeof(bool)];
};

#define IDS_BLAZELINK_TARGET 128

int wmain(int argc, const wchar_t *argv[])
{
	wchar_t command[] = L"%SystemRoot%\\System32\\PowerShell.exe";
	rescle::ResourceUpdater updater;
	if (updater.Load(L"blaze-link.exe")) {
		if (updater.ChangeString(IDS_BLAZELINK_TARGET, command)) {
			if (updater.Commit()) {
				MessageBoxW(nullptr, L"Update blaze-link success", L"Notice", MB_OK);
				return 0;
			}
		}
	}
	ErrorMessage error(GetLastError());
	MessageBoxW(nullptr, error.message(), L"Error", MB_OK);
	return 0;
}