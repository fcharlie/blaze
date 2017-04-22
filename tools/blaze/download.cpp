/// download engine
#include "blaze.hpp"
#include <winhttp.h>
#include <Shlwapi.h>
#include <string>

#define MinWarp(a,b) ((b<a)?b:a)

#define BLAZE_USERAGENT  L"Blaze/1.0"
#ifdef _WIN64
#define MOZ_USERAGENT L"Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
#else
#define MOZ_USERAGENT L"Mozilla/5.0 (Windows NT 10.0; Win32; x32)"
#endif

#ifndef WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL
#define WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL 133
#endif

#ifndef WINHTTP_PROTOCOL_FLAG_HTTP2
#define WINHTTP_PROTOCOL_FLAG_HTTP2 0x1
#endif

struct Uri {
	int nPort;
	int nScheme;
	std::wstring scheme;
	std::wstring host;
	std::wstring path;
	std::wstring username;
	std::wstring password;
	std::wstring extrainfo;
	bool Parse(const std::wstring &url) {
		URL_COMPONENTS urlComp;
		DWORD dwUrlLen = 0;
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);
		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		if (!WinHttpCrackUrl(url.data(), dwUrlLen, 0, &urlComp)) {
			return false;
		}
		scheme.assign(urlComp.lpszScheme, urlComp.dwSchemeLength);
		host.assign(urlComp.lpszHostName, urlComp.dwHostNameLength);
		path.assign(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
		nPort = urlComp.nPort;
		nScheme = urlComp.nScheme;
		if (urlComp.lpszUserName) {
			username.assign(urlComp.lpszUserName, urlComp.dwUserNameLength);
		}
		if (urlComp.lpszPassword) {
			password.assign(urlComp.lpszPassword, urlComp.dwPasswordLength);
		}
		if (urlComp.lpszExtraInfo) {
			extrainfo.assign(urlComp.lpszExtraInfo,
				urlComp.dwExtraInfoLength);
		}
		return true;
	}
};

class InternetObject {
public:
	InternetObject(HINTERNET hInternet) :hInternet_(hInternet) {
	}
	operator HINTERNET() {
		return hInternet_;
	}
	operator bool() {
		return hInternet_ != nullptr;
	}
	InternetObject() {
		if (hInternet_) {
			WinHttpCloseHandle(hInternet_);
		}
	}
private:
	HINTERNET hInternet_;
};

bool DownloadArchive(const std::wstring &url, std::wstring &localFile, const BlazeProgress &pg) {
	Uri uri;
	if (!uri.Parse(url)) {
		BaseErrorMessagePrint(L"Wrong URL: %s\n", url.c_str());
		return false;
	}
	InternetObject hInternet =
		WinHttpOpen(BLAZE_USERAGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hInternet) {
		ErrorMessage err(GetLastError());
		BaseErrorMessagePrint(L"WinHttpOpen(): %s", err.message());
		return false;
	}
	//	WinHttpSetOption(hInternet, WINHTTP_OPTION_REDIRECT_POLICY, &dwOption,sizeof(DWORD));
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/aa384066(v=vs.85).aspx
	/// WINHTTP_PROTOCOL_FLAG_HTTP2
	DWORD dwOption = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
	if (!WinHttpSetOption(hInternet, WINHTTP_OPTION_REDIRECT_POLICY,
		&dwOption, sizeof(DWORD))) {
		ErrorMessage err(GetLastError());
		BaseErrorMessagePrint(L"WINHTTP_OPTION_REDIRECT_POLICY: %s", err.message());
	}
	dwOption = WINHTTP_PROTOCOL_FLAG_HTTP2;
	if (!WinHttpSetOption(hInternet, WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL, &dwOption, sizeof(dwOption))) {
		ErrorMessage err(GetLastError());
		BaseErrorMessagePrint(L"WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL: %s", err.message());
	}
	InternetObject hConnect = WinHttpConnect(hInternet, uri.host.c_str(),
		(INTERNET_PORT)uri.nPort, 0);
	if (!hConnect) {
		BaseErrorMessagePrint(L"Server unable to connect: %s", uri.host.c_str());
		return false;
	}
	DWORD dwOpenRequestFlag =
		(uri.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
	InternetObject hRequest = WinHttpOpenRequest(
		hConnect, L"GET", uri.path.c_str(), nullptr, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES, dwOpenRequestFlag);
	if (!hRequest) {
		ErrorMessage err(GetLastError());
		BaseErrorMessagePrint(L"Open Request failed: %s", err.message());
		return false;
	}
	if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
		WINHTTP_NO_REQUEST_DATA, 0, 0, 0) == FALSE) {
		ErrorMessage err(GetLastError());
		BaseErrorMessagePrint(L"Send Request failed: %s", err.message());
		return false;
	}
	if (WinHttpReceiveResponse(hRequest, NULL) == FALSE) {
		ErrorMessage err(GetLastError());
		BaseErrorMessagePrint(L"Receive Response failed: %s", err.message());
		return false;
	}
	DWORD dwStatusCode = 0;
	DWORD dwXsize = sizeof(dwStatusCode);
	if (!WinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwXsize,
		WINHTTP_NO_HEADER_INDEX)) {
		ErrorMessage err(GetLastError());
		BaseErrorMessagePrint(L"WinHttpQueryHeaders failed: %s", err.message());
		return false;
	}
	if (dwStatusCode != 200 && dwStatusCode != 201) {
		BaseErrorMessagePrint(L"Reponse status code: %d\n", dwStatusCode);
		return false;
	}
	uint64_t dwContentLength = 0;
	wchar_t conlen[36];
	dwXsize = sizeof(conlen);
	if (WinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_CONTENT_LENGTH,
		WINHTTP_HEADER_NAME_BY_INDEX, conlen, &dwXsize,
		WINHTTP_NO_HEADER_INDEX)) {
	}
	wchar_t *cx = nullptr;
	dwContentLength = wcstoull(conlen, &cx, 10);
	wprintf(L"File size: %lld\n", dwContentLength);
	std::wstring tmp = localFile + L".part";
	HANDLE hFile =
		CreateFileW(tmp.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	///
	uint64_t total = 0;
	DWORD dwSize = 0;
	char fixedsizebuf[16384];
	///
	if (pg.impl) {
		pg.impl(pg.userdata,0,0);
	}
	do {
		// Check for available data.
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
			break;
		}
		total += dwSize;
		if (dwContentLength > 0) {
			if (pg.impl) {
				pg.impl(pg.userdata, (uint32_t)(total * 100 / dwContentLength), total);
			}
		}
		auto dwSizeN = dwSize;
		while (dwSizeN > 0) {
			DWORD dwDownloaded = 0;
			if (!WinHttpReadData(hRequest, (LPVOID)fixedsizebuf, MinWarp(sizeof(fixedsizebuf), dwSizeN), &dwDownloaded)) {
				break;
			}
			dwSizeN = dwSizeN - dwDownloaded;
			DWORD wmWritten;
			WriteFile(hFile, fixedsizebuf, dwSize, &wmWritten, NULL);
		}
	} while (dwSize > 0);
	if (pg.impl) {
		pg.impl(pg.userdata,100,dwContentLength);
	}
	std::wstring npath = localFile;
	int i = 1;
	while (PathFileExistsW(npath.c_str())) {
		auto n = localFile.find_last_of('.');
		if (n != std::wstring::npos) {
			npath = localFile.substr(0, n) + L"(";
			npath += std::to_wstring(i);
			npath += L")";
			npath += localFile.substr(n);
		}
		else {
			npath = localFile + L"(" + std::to_wstring(i) + L")";
		}
		i++;
	}
	CloseHandle(hFile);
	MoveFileExW(tmp.c_str(), npath.c_str(), MOVEFILE_COPY_ALLOWED);
	return true;
}