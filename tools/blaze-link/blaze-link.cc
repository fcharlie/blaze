////////////////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>
#include <Shellapi.h>
#include <Shlobj.h>
#include <stdio.h>
#include <stdlib.h>
//#include <locale.h>
#include "blaze-link.h"

#pragma comment(lib, "shell32")

size_t StringLength(const wchar_t *s) {
  const wchar_t *a;
  for (a = s; *s; s++)
    ;
  return s - a;
}

wchar_t *StringCopy(wchar_t *d, const wchar_t *s) {
  wchar_t *a = d;
  while ((*d++ = *s++))
    ;
  return a;
}

class StringBuffer {
public:
  StringBuffer() : m_capability(0), m_data(nullptr) {}
  StringBuffer(StringBuffer &&other) {
    m_data = other.m_data;
    other.m_data = nullptr;
    m_capability = other.m_capability;
    other.m_capability = 0;
    m_size = other.m_size;
    other.m_size = 0;
  }
  StringBuffer(size_t n) {
    m_data = (wchar_t *)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * n);
    m_capability = n;
    m_size = 0;
  }
  ~StringBuffer() {
    if (m_data) {
      HeapFree(GetProcessHeap(), 0, m_data);
    }
  }
  StringBuffer &transfer(StringBuffer &&other) {
    m_data = other.m_data;
    other.m_data = nullptr;
    m_capability = other.m_capability;
    other.m_capability = 0;
    m_size = other.m_size;
    other.m_size = 0;
    return *this;
  }
  bool append(const wchar_t *cstr) {
    auto l = StringLength(cstr);
    if (l > m_capability - m_size)
      return false;
    StringCopy(m_data + m_size, cstr);
    m_size += l;
    return true;
  }
  size_t capability() const { return m_capability; }
  size_t setsize(size_t sz) {
    if (m_size == 0)
      m_size = sz;
    return m_size;
  }
  const wchar_t *data() const {
    if (m_size < m_capability && m_size != 0) {
      m_data[m_size] = 0;
    }
    return m_data;
  }
  wchar_t *data() { return m_data; }

private:
  wchar_t *m_data;
  size_t m_capability;
  size_t m_size;
};

class IconCache {
public:
  enum { kIconMaxSize = 20 };
  IconCache() { size = 0; }
  ~IconCache() {
    for (unsigned i = 0; i < size; i++) {
      DestroyIcon(icons[i]);
    }
  }
  void PushIcon(HICON icon) {
    if (size < kIconMaxSize) {
      icons[size] = icon;
      size++;
    } else {
      DestroyIcon(icon);
    }
  }

private:
  HICON icons[kIconMaxSize];
  unsigned size;
};

bool UpdateConsoleIcon(const wchar_t *target) {
  static IconCache cache;
  HWND hWnd = GetConsoleWindow();
  if (!hWnd)
    return false;
  if (IsWindow(hWnd)) {
    HICON hSmall = nullptr;
    HICON hLarge = nullptr;
    if (ExtractIconExW(target, 0, &hLarge, &hSmall, 1) != 0) {
      SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hLarge);
      SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
      cache.PushIcon(hSmall);
    }
  }
  return true;
}

bool SubsystemIsConsole(const wchar_t *target) {
  HANDLE hFile = CreateFileW(target, GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (!hFile)
    return false;
  LARGE_INTEGER largeFile;
  if (!GetFileSizeEx(hFile, &largeFile)) {
    return false;
  }
  IMAGE_DOS_HEADER dosHeader;
  DWORD dwReadSize = 0;
  if (!ReadFile(hFile, &dosHeader, sizeof(dosHeader), &dwReadSize, nullptr) ||
      dwReadSize != sizeof(dosHeader)) {
    CloseHandle(hFile);
    return false;
  }
  LONG high = 0;
  SetFilePointer(hFile, dosHeader.e_lfanew, &high, FILE_BEGIN);
  IMAGE_NT_HEADERS ntheaders;
  if (!ReadFile(hFile, &ntheaders, sizeof(ntheaders), &dwReadSize, nullptr) ||
      dwReadSize != sizeof(ntheaders)) {
    CloseHandle(hFile);
    return false;
  }
  if (ntheaders.OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI) {
    CloseHandle(hFile);
    return true;
  }
  CloseHandle(hFile);
  return false;
}

// size_t StringCopy(wchar_t *)
//
// size_t StringLength(const wchar_t *s) {
//
//}

bool DoCheckSpace(const wchar_t *s) {
  for (; *s && *s != L' '; s++)
    ;
  return *s ? true : false;
}

bool BlazeLinkCreateCMD(const wchar_t *target, StringBuffer &cmd) {
  int Argc = 0;
  auto Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);
  if (!Argv) {
    return false;
  }
  StringBuffer buffer(0x8000);

  if (DoCheckSpace(target)) {
    buffer.append(L"\"");
    buffer.append(target);
    buffer.append(L"\" ");
  } else {
    buffer.append(target);
    buffer.append(L" ");
  }
  for (int i = 1; i < Argc; i++) {
    if (DoCheckSpace(Argv[i])) {
      buffer.append(L"\"");
      buffer.append(Argv[i]);
      buffer.append(L"\" ");
    } else {
      buffer.append(Argv[i]);
      buffer.append(L" ");
    }
  }
  cmd.transfer(reinterpret_cast<StringBuffer &&>(buffer));
  LocalFree(Argv);
  return true;
}

bool BlazeLinkToApp(const wchar_t *target) {
  STARTUPINFOW siw;
  GetStartupInfoW(&siw);
  if (siw.dwFlags & STARTF_USESHOWWINDOW) {
    UpdateConsoleIcon(target);
  }
  auto issc = SubsystemIsConsole(target);
  StringBuffer cmd;
  if (!BlazeLinkCreateCMD(target, cmd)) {
    return false;
  }
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  SecureZeroMemory(&si, sizeof(si));
  SecureZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE,
                      CREATE_UNICODE_ENVIRONMENT, nullptr, nullptr, &si, &pi)) {
    return 1;
  }
  CloseHandle(pi.hThread);
  if (issc) {
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    ExitProcess(exitCode);
  }
  return 0;
}

bool DiscoverTargetApp(StringBuffer &app) {
  auto hInstance = GetModuleHandleW(nullptr);
  StringBuffer buffer(8192);
  auto n = LoadStringW(hInstance, IDS_BLAZELINK_TARGET, buffer.data(),
                       (int)buffer.capability());
  if (n != 0) {
    buffer.setsize(n);
    StringBuffer cmd(0x8000);
    auto l = ExpandEnvironmentStringsW(buffer.data(), cmd.data(),
                                       (DWORD)cmd.capability());
    if (l != 0) {
      cmd.setsize(l);
      app.transfer(reinterpret_cast<StringBuffer &&>(cmd));
      return true;
    }
  }
  return false;
}

int wmain() {
  StringBuffer app;
  if (DiscoverTargetApp(app)) {
    if (BlazeLinkToApp(app.data())) {
      return 0;
    }
  }
  return 1;
}