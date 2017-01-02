//// Blaze 
#include <cstdint>
#include <cstring>
#include "blaze.hpp"
#include "package.hpp"
#include "archive.hpp"
/*
blaze Windows Portable Package Manager
Usage: blaze [command] [arguments] [commom-options]
list
install
uninstall
update
reinitialize
uninitialize
*/

const wchar_t *kUsage = LR"(blaze Windows Portable Package Manager
Usage: blaze [command] [arguments] [commom-options]
    list
    install
    uninstall
    sync
    update
    reinitialize
    uninitialize
    help
    -V
)";


inline bool IsArg(const wchar_t *candidate, const wchar_t *longname)
{
	if (wcscmp(candidate, longname) == 0)
		return true;
	return false;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *shortname,
	const wchar_t *longname)
{
	if (wcscmp(candidate, shortname) == 0 ||
		(longname != nullptr && wcscmp(candidate, longname) == 0))
		return true;
	return false;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *longname, size_t n,
	const wchar_t **off)
{
	auto l = wcslen(candidate);
	if (l < n)
		return false;
	if (wcsncmp(candidate, longname, n) == 0) {
		if (l > n && candidate[n] == '=') {
			*off = candidate + n + 1;
		}
		else {
			*off = nullptr;
		}
		return true;
	}
	return false;
}

void PrintUsage() {
	//BaseMessagePrint()
	BaseMessagePrint(L"%s\n", kUsage);
}

int ProcessMain(int Argc, wchar_t **Argv) {
	return 0;
}

int wmain(int argc,wchar_t **argv){
    ///
	if (argc >= 3) {
		ZipDecompressArchive(argv[1], argv[2]);
	}
	PrintUsage();
	RepositoriesInstalled rid;
	rid.Discover(L"repositories.installed.json");
    return 0;
}