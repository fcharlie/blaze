/// Msi Installer
#include "blaze.hpp"
#include <Msi.h>
#pragma comment(lib,"Msi.lib")


int FGetInteger(WCHAR*& rpch)
{
	WCHAR* pchPrev = rpch;
	while (*rpch && *rpch != L' ')
		rpch++;
	*rpch = L'\0';
	int i = _wtoi(pchPrev);
	return i;
}

BOOL ParseProgressString(LPWSTR sz, int* pnFields)
{
	WCHAR* pch = sz;
	if (0 == *pch)
		return FALSE; // no msg

	while (*pch != 0) {
		WCHAR chField = *pch++;
		pch++; // for ':'
		pch++; // for sp
		switch (chField) {
		case L'1': // field 1
		{
			// progress message type
			if (0 == isdigit(*pch))
				return FALSE; // blank record
			pnFields[0] = *pch++ - L'0';
			break;
		}
		case L'2': // field 2
		{
			pnFields[1] = FGetInteger(pch);
			if (pnFields[0] == 2 || pnFields[0] == 3)
				return TRUE; // done processing
			break;
		}
		case '3': // field 3
		{
			pnFields[2] = FGetInteger(pch);
			if (pnFields[0] == 1)
				return TRUE; // done processing
			break;
		}
		case '4': // field 4
		{
			pnFields[3] = FGetInteger(pch);
			return TRUE; // done processing
		}
		default: // unknown field
		{
			return FALSE;
		}
		}
		pch++; // for space (' ') between fields
	}

	return TRUE;
}


class BlazeMsiContext {
public:
	BlazeMsiContext() {

	}
	int MsiInstallerHandler(UINT iMessage, LPCWSTR szMessage) {
		return 0;
	}
private:
};

inline bool IsEmptyDirectory(const std::wstring &dir)
{
	std::wstring dir_(dir);
	dir_.append(L"\\*.*");
	WIN32_FIND_DATAW fdata;
	auto hFind = FindFirstFileW(dir_.c_str(), &fdata);
	if (hFind == INVALID_HANDLE_VALUE)
		return true;
	BOOL ret = true;
	while (ret) {
		if (wcscmp(fdata.cFileName, L".") != 0
			&& wcscmp(fdata.cFileName, L"..") != 0)
		{
			FindClose(hFind);
			return false;
		}
		ret = FindNextFileW(hFind, &fdata);
	}
	FindClose(hFind);
	return true;
}

INT CALLBACK MsiInstallerUIHandler(
	LPVOID  pvContext,
	UINT    iMessageType,
	LPCWSTR szMessage
) {
	auto bctx=reinterpret_cast<BlazeMsiContext *>(pvContext);
	return bctx->MsiInstallerHandler(iMessageType, szMessage);
}

bool MsiDecompressArchive(const std::wstring &msifile, const std::wstring &dir) {
	//
	return false;
}