//// Blaze 
#include <cstdint>
#include <cstring>
#include "package.hpp"
/*
blaze Windows Portable Package Manager
Usage: blaze [command] [arguments] [commom-options]
list
install
uninstall
reinitialize
uninitialize
*/

int wmain(int argc,wchar_t **argv){
    ///
	RepositoriesInstalled rid;
	rid.Discover(L"repositories.installed.json");
    return 0;
}