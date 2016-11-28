#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int wmain(int argc,wchar_t **argv){
    if(argc<3){
        fwprintf(stderr,L"lns usage: %s symbol target\n",argv[0]);
        return 1;
    }
    if(!CreateSymbolicLinkW(argv[2],argv[1],0)){
        fwprintf(stderr,L"GetLastError %d\n",GetLastError());
        return 2;
    }
    return 0;
}