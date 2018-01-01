#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

bool EnableVTMode()
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }
    return true;
}
int __cdecl wmain(int argc, WCHAR* argv[])
{   
    argc; // unused
    argv; // unused
    //First, enable VT mode
    bool fSuccess = EnableVTMode();
    if (!fSuccess)
    {
        printf("Unable to enter VT processing mode. Quitting.\n");
        return -1;
    }
    int red = 0; // set these to whatever
    int green = 0;
    int blue = 0;
    printf("\x1b[38;2;%d;%d;%dm XXX", red, green, blue); // produces RGB foreground
    printf("\x1b[48;2;%d;%d;%dm YYY", red, green, blue); // produces RGB background

    int index = 0;
    printf("\x1b[38;5;%dm zz", index); // produces xterm color table index foreground
    printf("\x1b[48;5;%dm zz", index); // produces xterm color table index background
	
	printf("\033[33;01m XXXXXXXXXXXXXXX");
	
	return 0;
}