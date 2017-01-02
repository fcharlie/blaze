#include "blaze.hpp"
#include <zip.h>
#include <unzip.h>

bool mkdirInternal() {
	return true;
}

bool ZipDecompressArchive(const std::wstring &file, const std::wstring &dir) {
	/// when build minizip as unicode, unzOpen64 support unicode
	unzFile uf = unzOpen64(dir.c_str());
	if (uf == nullptr)return false;
	unz_global_info64 gi;
	if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK) {
		unzClose(uf);
		return false;
	}
	char rbuffer[8192];
	decltype(gi.number_entry) i;
	for (i = 0; i < gi.number_entry; i++) {
		unz_file_info64 file_info;
		//if(unzGetCurrentFileInfo64(uf,&file_info,))
	}
	//printf("%llu", gi.number_entry);
	return true;
}

bool ZipCompressArchive(const std::wstring &path, const std::wstring &file) {
	///
	return true;
}