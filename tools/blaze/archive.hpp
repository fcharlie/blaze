#ifndef BLAZE_ARCHIVE_HPP
#define BLAZE_ARCHIVE_HPP
#pragma once
#include <string>
#include <unordered_map>
#include <functional>

bool MsiDecompressArchive(const std::wstring &msifile, const std::wstring &dir);
bool ZipDecompressArchive(const std::wstring &file, const std::wstring &dir);
bool BZipDecompressArchive(const std::wstring &file, const std::wstring &dir);
bool RarDecompressArchive(const std::wstring &file, const std::wstring &dir);
bool ExtensionDecompressArchive(const std::wstring &msifile, const std::wstring &dir);

class BlazeArchive {
public:
	typedef std::function<bool(const std::wstring &, const std::wstring &)> DecompressImpl;
	bool Initialize();
	bool Decompress(const wchar_t *format,const std::wstring &file, const std::wstring &dir);
private:
	std::unordered_map<const wchar_t*, DecompressImpl, WCharHash, WCharCompare> decompressProvider;
};

#endif