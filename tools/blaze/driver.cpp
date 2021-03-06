//// Blaze
#include <cstdint>
#include <cstring>
#include <unordered_map>
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

bool verbose = false;
std::wstring root;

bool IsVerbose() {
	return verbose;
}

const std::wstring &BlazeRoot() {
	return root;
}

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
    initialize
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
    BaseErrorMessagePrint(L"usage: blaze search package\n");
    return 1;
  }
  ////
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
  std::vector<std::wstring> pks;
  for (int i = 0; i < Argc; i++) {
    auto Arg = Argv[i];
    if (Arg[0] == '-') {
    } else {
      pks.push_back(Arg);
    }
  }
  return 0;
}

int blazeremove(int Argc, wchar_t **Argv) {
  //// blaze remove --pure
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

typedef int (*EnCommand)(int, wchar_t **);

EnCommand BuiltinResolve(const wchar_t *cmd) {
  std::unordered_map<const wchar_t *, EnCommand, CharHashW, CharCompareW> cmds =
      {
          //// built in commands
          {L"search", blazesearch},
          {L"help", blazehelp}, /// help command
          {L"list", blazelist}, ////
          {L"sync", blazesync},
          {L"update", blazeupdate},
          {L"install", blazeinstall},
          {L"remove", blazeremove},
          {L"initialize", blazeinitialize},
          {L"uninitialize", blazeuninitialize}
          ///
      };
  auto iter = cmds.find(cmd);
  if (iter == cmds.end())
    return nullptr;
  return iter->second;
}

int CommandPatternResolve(const wchar_t *Arg, wchar_t *nArg) {
  if (wcsncmp(L"-V", Arg, 2) == 0 || wcscmp(L"--verbose", Arg) == 0) {
    verbose = true;
    return 0;
  } else if (wcsncmp(L"-v", Arg, 2) == 0 || wcscmp(L"--version", Arg) == 0) {
    /// print version
    BaseMessagePrint(L"%s\n", L"1.0.0.1");
    ExitProcess(0);
  } else if (wcsncmp(L"-r", Arg, 2) == 0 || wcscmp(L"--blaze-root", Arg) == 0) {
    if (!nArg)
      return -1;
    root.assign(nArg);
    return 1;
  }
  return -1;
}

int wmain(int argc, wchar_t **argv) {
	///
	printf("Progress Bar test:\n");
	ProgressBar bar(L"download 7zip.exe ...");
	wchar_t buf[100];
	auto x = rand() % 1000 + 100;
	for (int i = 0; i < x; i++) {
		Sleep(50);
		swprintf_s(buf, L"%d/%d", i, x);
		bar.Update(i, x, buf);
	}
	system("pause");
	return 0;

  for (int i = 1; i < argc; i++) {
    auto Arg = argv[i];
    if (Arg[0] == '-') {
      switch (CommandPatternResolve(Arg, i + 1 < argc ? argv[i + 1] : nullptr)) {
      case 0:
        break;
      case 1:
        i++;
        break;
      case -1:
        BaseErrorMessagePrint(L"Invailed argument: %s\n", Arg);
        return 1;
      }
    } else {
      auto impl = BuiltinResolve(Arg);
      i++;
      if (impl != nullptr)
        return impl(argc - i, argv + i);
      return ExternalCommand(Arg, argc - i, argv + i);
    }
  }
  return blazehelp(argc, argv);
}