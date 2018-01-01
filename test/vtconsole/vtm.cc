#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "console.hpp"

bool EnableVTMode() {
  // Set output mode to handle virtual terminal sequences
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    return false;
  }

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    return false;
  }
  return true;
}
int __cdecl wmain(int argc, WCHAR *argv[]) {
  argc; // unused
  argv; // unused
  // First, enable VT mode
  bool fSuccess = EnableVTMode();
  if (!fSuccess) {
    printf("Unable to enter VT processing mode. Quitting.\n");
    return -1;
  }
  int red = 0; // set these to whatever
  int green = 0;
  int blue = 0;
  printf("\x1b[38;2;%d;%d;%dm XXX", red, green,
         blue); // produces RGB foreground
  printf("\x1b[48;2;%d;%d;%dm YYY", red, green,
         blue); // produces RGB background

  int index = 0;
  printf("\x1b[38;5;%dm zz",
         index); // produces xterm color table index foreground
  printf("\x1b[48;5;%dm zz",
         index); // produces xterm color table index background

  printf("\033[33;01m XXXXXXXXXXXXXXX\033[0m\n");

  console::Print(console::fc::Yellow, L"Argv: %s\n", argv[0]);
  wprintf(
      L"\x1b[33mThis text has a red foreground using SGR.33 %s .\033[0m\r\n",
      argv[0]);
  wchar_t buf[]=L"\x1b[33mZZZZZZZ\x1b[0m\n";
  DWORD N=0;
  auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteConsoleW(hConsole,buf,(DWORD)(sizeof(buf)/2-1),&N,nullptr);
  return 0;
}