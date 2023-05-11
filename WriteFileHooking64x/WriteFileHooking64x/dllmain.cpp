#include "pch.h"
#include "detours.h"
#include <iostream>
#include <WinUser.h>

using namespace std;

static INT(WINAPI* fakeWriteFile)(HANDLE hFile,
LPCVOID      lpBuffer,
DWORD        nNumberOfBytesToWrite,
LPDWORD      lpNumberOfBytesWritten,
LPOVERLAPPED lpOverlapped) = WriteFile;

extern "C" __declspec(dllexport)INT WINAPI fakesave(HANDLE hFile,
    LPCVOID      lpBuffer,
    DWORD        nNumberOfBytesToWrite,
    LPDWORD      lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped) {

    MessageBoxA(NULL, "hook@","HOONK!", MB_OK);
        
    return fakeWriteFile(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);
}
    
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call) 
    {
    case DLL_PROCESS_ATTACH:
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)fakeWriteFile, fakesave);
        DetourTransactionCommit();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)fakeWriteFile, fakesave);
        DetourTransactionCommit();
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}