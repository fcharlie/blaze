//// make blaze-link
#include <string>
#include <shlwapi.h>
#include <pathcch.h>
#include "rescle.h"

#define IDS_BLAZELINK_TARGET 128

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

struct BlazeApp {
	std::wstring target;
	std::wstring desc;
	std::wstring copyright;
	std::wstring version;
};

bool GetOriginBlazeLink(std::wstring &blazelink)
{
	WCHAR buffer[PATHCCH_MAX_CCH];
	auto n=GetModuleFileNameW(nullptr, buffer, PATHCCH_MAX_CCH);
	if (n == 0)return false;
	if (PathRemoveFileSpecW(buffer)) {
		wcscat_s(buffer, L"\\blaze-link.exe");
		if (PathFileExistsW(buffer)) {
			blazelink.assign(buffer);
			return true;
		}
	}
	return false;
}

bool BlazeAppMklink(const BlazeApp &app)
{
	std::wstring blazelink;
	if (!GetOriginBlazeLink(blazelink)) {
		return false;
	}
	auto targetName=PathFindFileNameW(app.target.c_str());
	if (!CopyFileW(blazelink.c_str(), targetName, TRUE)) {
		ErrorMessage error(GetLastError());
		MessageBoxW(nullptr, error.message(), L"CopyFileW()", MB_OK);
		return false;
	}
	rescle::ResourceUpdater updater;
	if (!updater.Load(targetName)) {
		DeleteFileW(targetName);
		return false;
	}
	updater.ChangeString(IDS_BLAZELINK_TARGET, app.target.c_str());
	if (!app.desc.empty()) {
		updater.SetVersionString(L"FileDescription", app.desc.c_str());
	}
	if (!app.copyright.empty()) {
		updater.SetVersionString(L"LegalCopyright", app.copyright.c_str());
	}

	if (!updater.Commit()) {
		DeleteFileW(targetName);
		return false;
	}
	///
	//PathStripPathW(&(app.target[0]));
	//CopyFileW(blazelink.c_str(),)
	return true;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *longname)
{
	return (wcscmp(candidate, longname) == 0);
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *shortname,
				  const wchar_t *longname)
{
	if (wcscmp(candidate, shortname) == 0 ||
		(longname != nullptr && wcscmp(candidate, longname) == 0))
		return true;
	return false;
}

inline int SearchIndex(const wchar_t *str, int ch)
{
	auto iter = str;
	for (; *iter; iter++) {
		if (*iter == ch) {
			return (iter - str);
		}
	}
	return -1;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *shortname,
				  const wchar_t *longname, const wchar_t **av)
{
	auto i = SearchIndex(candidate, '=');
	if (i == -1) {
		if (wcscmp(candidate, shortname) == 0 || wcscmp(candidate, longname) == 0) {
			*av = nullptr;
			return true;
		}
		return false;
	}
	if (i < 2)
		return false;
	//
	if (shortname) {
		auto sl = wcslen(shortname);
		if (sl == (decltype(sl))i && wcsncmp(candidate, shortname, sl) == 0) {
			*av = candidate + i + 1;
			return true;
		}
	}
	if (longname) {
		auto ll = wcslen(longname);
		if (ll == (decltype(ll))i && wcsncmp(candidate, longname, ll) == 0) {
			*av = candidate + i + 1;
			return true;
		}
	}
	return false;
}



/// blaze-mk --target=apppath --desc="Information About mk" --copyright="Copyright"
int wmain(int argc, wchar_t **argv)
{
	//if (argc < 2) {
	//	wprintf(L"usage: %s --target=/path/to/traget\n", argv[0]);
	//	return 1;
	//}
	BlazeApp app = {
		L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\PowerShell.exe",
		L"Powershell app link [blaze-link]",
		L"\xA9 Microsoft Corporation. All rights reserved.",
		L""
	};
	BlazeAppMklink(app);
	return 0;
}
