#include "blaze.hpp"
#include "unrar.h"

enum ERR_TYPE { ERR_OPEN, ERR_READ, ERR_PROCESS };

void OutError(int Error, wchar_t *ArcName, int ErrType)
{
	switch (Error)
	{
	case ERAR_NO_MEMORY:
		BaseErrorMessagePrint(L"\nNot enough memory");
		break;
	case ERAR_BAD_DATA:
		BaseErrorMessagePrint(L"\n%s: archive header or data are damaged", ArcName);
		break;
	case ERAR_BAD_ARCHIVE:
		BaseErrorMessagePrint(L"\n%s is not RAR archive", ArcName);
		break;
	case ERAR_UNKNOWN_FORMAT:
		BaseErrorMessagePrint(L"Unknown archive format");
		break;
	case ERAR_EOPEN:
		if (ErrType == ERR_PROCESS) // Returned by RARProcessFile.
			BaseErrorMessagePrint(L"Volume open error");
		else
			BaseErrorMessagePrint(L"\nCannot open %s", ArcName);
		break;
	case ERAR_ECREATE:
		BaseErrorMessagePrint(L"File create error");
		break;
	case ERAR_ECLOSE:
		BaseErrorMessagePrint(L"File close error");
		break;
	case ERAR_EREAD:
		BaseErrorMessagePrint(L"Read error");
		break;
	case ERAR_EWRITE:
		BaseErrorMessagePrint(L"Write error");
		break;
	case ERAR_SMALL_BUF:
		BaseErrorMessagePrint(L"Buffer for archive comment is too small, comment truncated");
		break;
	case ERAR_UNKNOWN:
		BaseErrorMessagePrint(L"Unknown error");
		break;
	case ERAR_MISSING_PASSWORD:
		BaseErrorMessagePrint(L"Password for encrypted file or header is not specified");
		break;
	case ERAR_EREFERENCE:
		BaseErrorMessagePrint(L"Cannot open file source for reference record");
		break;
	case ERAR_BAD_PASSWORD:
		BaseErrorMessagePrint(L"Wrong password is specified");
		break;
	}
}


int CALLBACK CallbackProc(UINT msg, LPARAM UserData, LPARAM P1, LPARAM P2) {
	return 0;
}

bool RarDecompressArchive(const std::wstring &file, const std::wstring &dir) {
	RARHeaderData HeaderData;
	RAROpenArchiveDataEx OpenArchiveData;
	char CmtBuf[16384];
	memset(&HeaderData, 0, sizeof(HeaderData));
	memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
	OpenArchiveData.ArcNameW = (wchar_t*)file.data();
	OpenArchiveData.CmtBuf = CmtBuf;
	OpenArchiveData.CmtBufSize = sizeof(CmtBuf);
	OpenArchiveData.OpenMode = RAR_OM_EXTRACT;
	OpenArchiveData.Callback = CallbackProc;
	//OpenArchiveData.UserData = EXTRACT;
	auto hArcData = RAROpenArchiveEx(&OpenArchiveData);
	if (OpenArchiveData.OpenResult != 0) {
		return false;
	}
	return true;
}