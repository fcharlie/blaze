//// make blaze-link
#include <string>
#include <shlwapi.h>
#include <pathcch.h>
#include <VersionHelpers.h>
#include <wchar.h>
#include "rescle.h"

#define IDS_BLAZELINK_TARGET 128

class ErrorMessage {
public:
  ErrorMessage(DWORD errid) : lastError(errid), release_(false) {
    if (FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            nullptr, errid, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
            (LPWSTR)&buf, 0, nullptr) == 0) {
      buf = L"Unknown error";
    } else {
      release_ = true;
    }
  }
  ~ErrorMessage() {
    if (release_) {
      LocalFree(buf);
    }
  }
  const wchar_t *message() const { return buf; }
  DWORD LastError() const { return lastError; }

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

bool GetOriginBlazeLink(std::wstring &blazelink) {
  WCHAR buffer[PATHCCH_MAX_CCH];
  auto n = GetModuleFileNameW(nullptr, buffer, PATHCCH_MAX_CCH);
  if (n == 0)
    return false;
  if (PathRemoveFileSpecW(buffer)) {
    wcscat_s(buffer, L"\\blaze-link.exe");
    if (PathFileExistsW(buffer)) {
      blazelink.assign(buffer);
      return true;
    }
  }
  return false;
}

bool BlazeAppMklink(const BlazeApp &app) {
  std::wstring blazelink;
  if (!GetOriginBlazeLink(blazelink)) {
    return false;
  }
  auto targetName = PathFindFileNameW(app.target.c_str());
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
  // PathStripPathW(&(app.target[0]));
  // CopyFileW(blazelink.c_str(),)
  return true;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *longname) {
  return (wcscmp(candidate, longname) == 0);
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *shortname,
                  const wchar_t *longname) {
  if (wcscmp(candidate, shortname) == 0 ||
      (longname != nullptr && wcscmp(candidate, longname) == 0))
    return true;
  return false;
}

inline int SearchIndex(const wchar_t *str, int ch) {
  auto iter = str;
  for (; *iter; iter++) {
    if (*iter == ch) {
      return static_cast<int>(iter - str);
    }
  }
  return -1;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *shortname,
                  const wchar_t *longname, const wchar_t **av) {
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

bool IsWindowsVersionOrGreaterEx(WORD wMajorVersion, WORD wMinorVersion, DWORD buildNumber)
{
	const wchar_t *currentVersion =
		LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)";
	HKEY hKey = nullptr;
	bool result = false;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, currentVersion, 0, KEY_READ, &hKey) !=
		ERROR_SUCCESS) {
		return false;
	}
	DWORD wMajor,wMinor;
	DWORD type = 0;
	DWORD dwSizeM = sizeof(DWORD);
	if (RegGetValueW(hKey, nullptr,
		L"CurrentMajorVersionNumber",
		RRF_RT_DWORD, &type, &wMajor, &dwSizeM) != ERROR_SUCCESS) {
		result = false;
		goto CloseLab;
	}
	dwSizeM = sizeof(DWORD);
	if (RegGetValueW(hKey, nullptr,
		L"CurrentMinorVersionNumber",
		RRF_RT_DWORD, &type, &wMinor, &dwSizeM) != ERROR_SUCCESS) {
		result = false;
		goto CloseLab;
	}
	WCHAR buffer[32];
	DWORD dwSize = sizeof(buffer);
	if (RegGetValueW(hKey, nullptr, L"CurrentBuildNumber", RRF_RT_REG_SZ, &type,
		buffer, &dwSize) != ERROR_SUCCESS) {
		result = false;
		goto CloseLab;
	}
	wchar_t *w;
	auto bn = wcstol(buffer, &w, 10);
	if (wMajor > wMajorVersion) {
		result = true;
	} else if (wMajor < wMajorVersion) {
		result = false;
	} else {
		if (wMinor > wMinorVersion) {
			result = true;
		} else if (wMinor< wMinorVersion) {
			result = false;
		} else {
			if (bn > buildNumber) {
				result = true;
			} else if (bn< buildNumber) {
				result = false;
			} else {
				result = true;
			}
		}
	}
	CloseLab:
	RegCloseKey(hKey);
	return result;
}

bool
IsWindowsBuildVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor,int buildVer)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
		VerSetConditionMask(
		VerSetConditionMask(
		0, VER_MAJORVERSION, VER_GREATER_EQUAL),
		VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL),
		VER_BUILDNUMBER, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;
	osvi.dwBuildNumber = buildVer;
	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION| VER_BUILDNUMBER | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}


/// blaze-mk --target=apppath --desc="Information About mk"
/// --copyright="Copyright"
int wmain(int argc, wchar_t **argv) {
	IsWindowsVersionOrGreaterEx(11, 8, 9600);
  // if (argc < 2) {
  //	wprintf(L"usage: %s --target=/path/to/traget\n", argv[0]);
  //	return 1;
  //}
	if (!IsWindowsBuildVersionOrGreater(10,0,0,14972)) {
		printf("Windows 10.14369 + \n");
	}
  BlazeApp app = {
      L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\PowerShell.exe",
      L"Powershell app link [blaze-link]",
      L"\xA9 Microsoft Corporation. All rights reserved.", L""};
  BlazeAppMklink(app);
  return 0;
}
