#include "blaze.hpp"
#include "archive.hpp"



bool BlazeArchive::Initialize()
{
	decompressProvider.insert(std::pair<const wchar_t *, DecompressImpl>(L"msi", MsiDecompressArchive));
	decompressProvider.insert(std::pair<const wchar_t *, DecompressImpl>(L"zip", ZipDecompressArchive));
	decompressProvider.insert(std::pair<const wchar_t *, DecompressImpl>(L"bzip", BZipDecompressArchive));
	return true;
}


bool BlazeArchive::Decompress(const wchar_t *format,const std::wstring & file, const std::wstring & dir)
{
	//const wchar_t* knownFormats[] = { L"7zip", L"gnutar", L"pax", L"paxr", L"zip" };
	//bool known = std::find(std::begin(knownFormats), std::end(knownFormats),
	//	format) != std::end(knownFormats);
	if (format==nullptr) {
		BaseErrorMessagePrint(L"Invaild format\n");
		return false;
	}
	auto func = decompressProvider.find(format);
	if (func != decompressProvider.end()) {
		return func->second(file, dir);
	}
	BaseErrorMessagePrint(L"Format '%s' not found register function provider\n",format);
	return false;
}
