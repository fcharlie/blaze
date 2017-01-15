//// Blaze
#include <cstdint>
#include <cstring>
#include "blaze.hpp"
#include "package.hpp"
#include "archive.hpp"

struct BuiltinCommand {
  const wchar_t *cmd;
  int (*impl)(int, wchar_t **);
};

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

inline bool IsArg(const wchar_t *candidate, const wchar_t *longname) {
  if (wcscmp(candidate, longname) == 0)
    return true;
  return false;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *shortname,
                  const wchar_t *longname) {
  if (wcscmp(candidate, shortname) == 0 ||
      (longname != nullptr && wcscmp(candidate, longname) == 0))
    return true;
  return false;
}

inline bool IsArg(const wchar_t *candidate, const wchar_t *longname, size_t n,
                  const wchar_t **off) {
  auto l = wcslen(candidate);
  if (l < n)
    return false;
  if (wcsncmp(candidate, longname, n) == 0) {
    if (l > n && candidate[n] == '=') {
      *off = candidate + n + 1;
    } else {
      *off = nullptr;
    }
    return true;
  }
  return false;
}

int blazehelp(int, wchar_t **) {
  const wchar_t *kUsage = LR"(blaze Windows Portable Package Manager
Usage: blaze [command] [arguments] [commom-options]
    list
    search
    install
    uninstall
    sync
    update
    reinitialize
    uninitialize
    help
    -V
)";
  BaseMessagePrint(L"%s\n", kUsage);
  return 0;
}

int blazelist(int Argc, wchar_t **Argv) {
  /// blaze list
  return 0;
}

int blazesearch(int Argc, wchar_t **Argv) {
  //// blaze search command
  if (Argc == 0) {
    BaseErrorMessagePrint(L"usage: blaze search <package>\n");
    return 1;
  }
  return 0;
}

int blazesync(int Argc, wchar_t **Argv) {
  /// fetch remote
  return 0;
}

int blazeupdate(int Argc, wchar_t **Argv) {
  /// update command
  return 0;
}

int blazeinstall(int Argc, wchar_t **Argv) {
  ///
  if (Argc >= 1) {
    DebugPrint(L"Install package: %s\n", Argv[0]);
  }
  return 0;
}

int blazeuninstall(int Argc, wchar_t **Argv) {
  ////
  return 0;
}

int blazeinitialize(int Argc, wchar_t **Argv) {
  ///
  return 0;
}

int blazeuninitialize(int Argc, wchar_t **Argv) {
  ///
  return 0;
}

int ExternalCommand(const wchar_t *cmd, int Argc, wchar_t **Argv) {
  /// if not define command
  std::wstring extcmd = L"blaze-";
  extcmd.append(cmd);
  DebugPrint(L"External command: %s\n", extcmd.c_str());
  for (int i = 0; i < Argc; i++) {
    BaseMessagePrint(L"Command: %s\n", Argv[i]);
  }
  //// Run command
  return 0;
}

/// initialize environment
bool blazeinit() {
	///
	return true;
}

int wmain(int argc, wchar_t **argv) {
	BuiltinCommand cmds[] = {
		//// built in commands
		{ L"search", blazesearch },
		{ L"help", blazehelp }, /// help command
		{ L"list", blazelist }, ////
		{ L"sync", blazesync },
		{ L"update", blazeupdate },
		{ L"install", blazeinstall },
		{ L"uninstall", blazeuninstall },
		{ L"initialize", blazeinitialize },
		{ L"uninitialize", blazeuninitialize }
		///
	};
	auto Isbuiltin = [&](const wchar_t *cmd_) -> decltype(cmds[0].impl) {
		for (auto &c : cmds) {
			if (wcscmp(c.cmd, cmd_) == 0)
				return c.impl;
		}
		return nullptr;
	};
	for (int i = 1; i < argc; i++) {
		auto ArgX = argv[i];
		if (ArgX[0] == '-') {

		}
		else {
			auto impl = Isbuiltin(ArgX);
			i++;
			if (impl != nullptr)
				return impl(argc - i, argv + i);
			return ExternalCommand(ArgX, argc - i, argv + i);
		}
	}
	return blazehelp(argc, argv);
}