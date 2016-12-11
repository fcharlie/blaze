////
#include <Windows.h>
#include <string>
#include <functional>

/// On Windows 14972, CreateSymbolicLink not require Administrator

#ifndef SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
#define SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE 0x2
#endif

// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion
// Value: CurrentBuildNumber CurrentMajorVersionNumber CurrentMinorVersionNumber
// ReleaseId
//
bool IsWindowsCurrentBuildNumberOrGreaterEx(DWORD buildNumber) {
  const wchar_t *currentVersion =
      LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)";
  HKEY hKey = nullptr;
  if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, currentVersion, 0, KEY_READ, &hKey) !=
      ERROR_SUCCESS) {
    return false;
  }
  WCHAR buffer[256];
  DWORD type = 0;
  DWORD dwSize = sizeof(buffer);
  if (RegGetValueW(hKey, nullptr, L"CurrentBuildNumber", RRF_RT_REG_SZ, &type,
                   buffer, &dwSize) != ERROR_SUCCESS) {
    RegCloseKey(hKey);
    return false;
  }
  RegCloseKey(hKey);
  wchar_t *w;
  auto bn = wcstol(buffer, &w, 10);
  return (bn >= buildNumber);
}

bool IsWindowsVersionOrGreaterEx2(WORD wMajorVersion, WORD wMinorVersion,
                                  DWORD buildNumber) {
  const wchar_t *currentVersion =
      LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)";
  HKEY hKey = nullptr;
  bool result = false;
  if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, currentVersion, 0, KEY_READ, &hKey) !=
      ERROR_SUCCESS) {
    return false;
  }
  DWORD wMajor, wMinor;
  DWORD type = 0;
  DWORD dwSizeM = sizeof(DWORD);
  if (RegGetValueW(hKey, nullptr, L"CurrentMajorVersionNumber", RRF_RT_DWORD,
                   &type, &wMajor, &dwSizeM) != ERROR_SUCCESS) {
    result = false;
    goto CloseLab;
  }
  dwSizeM = sizeof(DWORD);
  if (RegGetValueW(hKey, nullptr, L"CurrentMinorVersionNumber", RRF_RT_DWORD,
                   &type, &wMinor, &dwSizeM) != ERROR_SUCCESS) {
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
    } else if (wMinor < wMinorVersion) {
      result = false;
    } else {
      if (bn >= buildNumber) {
        result = true;
      } else
        result = false;
    }
  }
CloseLab:
  RegCloseKey(hKey);
  return result;
}

/// say must add manifest
bool IsWindowsVersionOrGreaterEx(WORD wMajorVersion, WORD wMinorVersion,
                                 WORD wServicePackMajor, int buildVer) {
  OSVERSIONINFOEXW osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0};
  DWORDLONG const dwlConditionMask = VerSetConditionMask(
      VerSetConditionMask(
          VerSetConditionMask(
              VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL),
              VER_MINORVERSION, VER_GREATER_EQUAL),
          VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL),
      VER_BUILDNUMBER, VER_GREATER_EQUAL);

  osvi.dwMajorVersion = wMajorVersion;
  osvi.dwMinorVersion = wMinorVersion;
  osvi.wServicePackMajor = wServicePackMajor;
  osvi.dwBuildNumber = buildVer;
  return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION |
                                       VER_BUILDNUMBER | VER_SERVICEPACKMAJOR,
                            dwlConditionMask) != FALSE;
}

static bool PtrCreateSymbolicLink(const wchar_t *file, const wchar_t *target) {
  if (CreateSymbolicLinkW(file, target,
                          SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE))
    return true;
  return false;
}

class Symboliclink {
public:
  Symboliclink() {
    ///
    InitializeSymboliclink();
  }
  bool BlazeCreateSymbolicLinkImpl(const wchar_t *file, const wchar_t *target) {
    if (symFun)
      return symFun(file, target);
    return false;
  }

private:
  bool InitializeSymboliclink() {
    //
    if (IsWindowsCurrentBuildNumberOrGreaterEx(14972)) {
      symFun = &PtrCreateSymbolicLink;
    } else {
    }
    return true;
  }

  std::function<bool(const wchar_t *, const wchar_t *)> symFun;
};

bool BlazeCreateSymbolicLink(const std::wstring &sources,
                             const std::wstring &target) {
  static Symboliclink symlink;
  return symlink.BlazeCreateSymbolicLinkImpl(sources.c_str(), target.c_str());
}