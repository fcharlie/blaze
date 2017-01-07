#include "blaze.hpp"
#include "unrar.h"

bool RarDecompressArchive(const std::wstring &file, const std::wstring &dir) {
	RAROpenArchiveDataEx rarArchiveEx;
	memset(&rarArchiveEx, 0, sizeof(RAROpenArchiveDataEx));
	//rarArchiveEx.ArcNameW = (wchar_t *)file.data();
	return true;
}