////////////////////////
#define WIN32_LEAN_AND_MEAN   
#include <windows.h>
#include <winnt.h>
#include <Shellapi.h>
#include <Shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "blaze-link.h"

#pragma comment(lib,"shell32")

class StringBuffer {
public:
	StringBuffer() :m_size(0), m_data(nullptr)
	{
	}
	StringBuffer(StringBuffer &&other)
	{
		m_data = other.m_data;
		other.m_data = nullptr;
		m_size = other.m_size;
		other.m_size = 0;
	}
	StringBuffer(size_t n)
	{
		m_data = new wchar_t[n];
		m_size = n;
	}
	~StringBuffer()
	{
		if (m_data) {
			delete[] m_data;
		}
	}
	size_t sync(const StringBuffer &other)
	{
		if (m_data) {
			delete[] m_data;
		}
		m_size = wcslen(other.m_data)+1;
		m_data = new wchar_t[m_size];
		wcscpy_s(m_data, m_size, other.m_data);
		return m_size;
	}
	size_t size()const
	{
		return m_size;
	}
	wchar_t *data()
	{
		return m_data;
	}
private:
	wchar_t *m_data;
	size_t m_size;
};

bool CheckSpace(const wchar_t *str)
{
	for (; *str; str++)
		if (*str == L' ')
			return true;
	return false;
}

class IconCache {
public:
	IconCache() :n(0){}
	~IconCache() {
		for (UINT8 i = 1; i < n &&i < 20; i++) {
			DestroyIcon(icons[i]);
		}
	}
	void PushIcon(HICON icon)
	{
		if (n < 19) {
			icons[n] = icon;
			n++;
		}
	}
private:
	HICON icons[20];
	UINT8 n;
};

bool UpdateConsoleIcon(const wchar_t *target)
{
	static IconCache cache;
	HWND hWnd = GetConsoleWindow();
	if (!hWnd)return false;
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

bool SubsystemIsConsole(const wchar_t *target)
{
	HANDLE hFile = CreateFileW(target,
							   GENERIC_READ,
							   FILE_SHARE_READ,
							   NULL,
							   OPEN_ALWAYS,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL);
	if (!hFile)
		return false;
	LARGE_INTEGER largeFile;
	if (!GetFileSizeEx(hFile, &largeFile)) {
		return false;
	}
	IMAGE_DOS_HEADER dosHeader;
	DWORD dwReadSize = 0;
	if (!ReadFile(hFile, &dosHeader, sizeof(dosHeader), &dwReadSize, nullptr)
		|| dwReadSize != sizeof(dosHeader)) {
		CloseHandle(hFile);
		return false;
	}
	LONG high = 0;
	SetFilePointer(hFile, dosHeader.e_lfanew, &high, FILE_BEGIN);
	IMAGE_NT_HEADERS ntheaders;
	if (!ReadFile(hFile, &ntheaders, sizeof(ntheaders), &dwReadSize, nullptr)
		|| dwReadSize != sizeof(ntheaders)) {
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

bool BlazeLinkCreateCMD(const wchar_t *target, StringBuffer &cmd)
{
	int Argc = 0;
	auto Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);
	if (!Argv) {
		return false;
	}
	StringBuffer buffer(0x8000);
	if (CheckSpace(target)) {
		wcscpy_s(buffer.data(), buffer.size(), L"\"");
		wcscat_s(buffer.data(), buffer.size(), target);
		wcscat_s(buffer.data(), buffer.size(), L"\" ");
	} else {
		wcscpy_s(buffer.data(), buffer.size(), target);
		wcscat_s(buffer.data(), buffer.size(), L" ");
	}
	for (int i = 1; i < Argc; i++) {
		if (CheckSpace(Argv[i])) {
			wcscpy_s(buffer.data(), buffer.size(), L"\"");
			wcscat_s(buffer.data(), buffer.size(), Argv[i]);
			wcscat_s(buffer.data(), buffer.size(), L"\" ");
		} else {
			wcscat_s(buffer.data(), buffer.size(), Argv[i]);
			wcscat_s(buffer.data(), buffer.size(), L" ");
		}
	}
	cmd.sync(buffer);
	LocalFree(Argv);
	return true;
}

bool BlazeLinkToApp(const wchar_t *target)
{
	STARTUPINFOW siw;
	GetStartupInfoW(&siw);
	if (siw.dwFlags &STARTF_USESHOWWINDOW) {
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
		CREATE_UNICODE_ENVIRONMENT,
		nullptr, nullptr, &si, &pi)) {
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

bool DiscoverTargetApp(StringBuffer &app)
{
	auto hInstance = GetModuleHandleW(nullptr);
	StringBuffer buffer(8192);
	auto n = LoadStringW(hInstance, IDS_BLAZELINK_TARGET, buffer.data(), (int)buffer.size());
	if (n != 0) {
		StringBuffer cmd(0x8000);
		auto l = ExpandEnvironmentStringsW(buffer.data(), cmd.data(), (DWORD)cmd.size());
		if (l != 0) {
			app.sync(cmd);
			return true;
		}
	}
	return false;
}

int wmain()
{
	StringBuffer app;
	if (DiscoverTargetApp(app)) {
		if (BlazeLinkToApp(app.data())) {
			return 0;
		}
	}
	return 1;
}