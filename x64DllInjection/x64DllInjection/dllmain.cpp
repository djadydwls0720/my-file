// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include<stdio.h>
#include<Windows.h>
#pragma pack(push,1)//구조체 데이터 정렬크기를 1바이트로 정렬, 구조체 변수간에 공간이 있을수있기에 1바이트로 할당하는것같다.
struct GOGO_NAM {
    BYTE opcode1;
    DWORD lpTarget1;
    DWORD opcode2;
    DWORD lpTarget2;
    BYTE opcode3;
};
#pragma pack(pop)//위의 설정 이전상태로 돌림

DWORD WINAPI Hook64();
DWORD WINAPI Unhook64();
GOGO_NAM orgFP;
BOOL Hooked = FALSE;

INT WINAPI NewWriteFile(
    _In_        HANDLE       hFile,
    _In_        LPCVOID      lpBuffer,
    _In_        DWORD        nNumberOfBytesToWrite,
    _Out_opt_   LPDWORD      lpNumberOfBytesWritten,
    _Inout_opt_ LPOVERLAPPED lpOverlapped) {
    MessageBoxA(NULL, (LPCSTR)lpBuffer, "hooking api call success", MB_OK);
    MessageBoxA(NULL, "내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈내돈", "hooking api call success", MB_OK);
    
    Unhook64();
    BOOL ret = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    Hook64();
    return ret;
}

DWORD WINAPI Hook64() {
    if (Hooked)
        return 0; // Already Hooked 
    // Get address of target function 
    LPVOID lpOrgFunc = NULL;
    if ((lpOrgFunc = GetProcAddress(GetModuleHandleA("kernel32.dll"), "WriteFile")) == NULL)
        return -1; // Check KernelBase-Redirect 
    if (*(SHORT*)lpOrgFunc == 0x25FF) {
        if ((lpOrgFunc = GetProcAddress(GetModuleHandleA("kernelbase.dll"), "WriteFile")) == NULL)
            return -1;
    } // Inline Hook 
      // Backup old protect 
    DWORD dwOldProtect;
    if (VirtualProtect(lpOrgFunc, sizeof(GOGO_NAM), PAGE_EXECUTE_READWRITE, &dwOldProtect) == NULL)
        return -1;
    memcpy_s(&orgFP, sizeof(GOGO_NAM), lpOrgFunc, sizeof(GOGO_NAM));
    GOGO_NAM newFuncObj;
    newFuncObj.opcode1 = 0x68; // Push ???? 
    newFuncObj.lpTarget1 = (DWORD)((DWORD64)(&NewWriteFile) & 0xFFFFFFFF);
    newFuncObj.opcode2 = 0x042444C7; // MOV [ESP+4], ???? 
    newFuncObj.lpTarget2 = (DWORD)((DWORD64)(&NewWriteFile) >> 32);
    newFuncObj.opcode3 = 0xC3; // RET 
    memcpy_s(lpOrgFunc, sizeof(GOGO_NAM), &newFuncObj, sizeof(GOGO_NAM)); // Replacing 
    // Rollback protection 
    VirtualProtect(lpOrgFunc, sizeof(GOGO_NAM), dwOldProtect, NULL);
    Hooked = TRUE;
    return 0;
}

DWORD WINAPI Unhook64() {
    if (!Hooked)
        return 0; // Not Hooked 
    LPVOID lpOrgFunc = NULL;
    if ((lpOrgFunc = GetProcAddress(GetModuleHandleA("kernel32.dll"), "WriteFile")) == NULL)
        return -1; // Check KernelBase-Redirect 
    if (*(SHORT*)lpOrgFunc == 0x25FF) {
        if ((lpOrgFunc = GetProcAddress(GetModuleHandleA("kernelbase.dll"), "WriteFile")) == NULL)
            return -1;
    } // Inline Hook 
      // Backup old protect 
    DWORD dwOldProtect;
    if (VirtualProtect(lpOrgFunc, sizeof(GOGO_NAM), PAGE_EXECUTE_READWRITE, &dwOldProtect) == NULL)
        return -1; // reset org FP 
    memcpy_s(lpOrgFunc, sizeof(GOGO_NAM), &orgFP, sizeof(GOGO_NAM)); // Rollback protection 
    VirtualProtect(lpOrgFunc, sizeof(GOGO_NAM), dwOldProtect, NULL);
    Hooked = FALSE;
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Hook64();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}