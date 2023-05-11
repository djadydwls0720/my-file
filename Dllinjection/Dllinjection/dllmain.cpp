#include <windows.h>
#include "detours.h"
#include "stdio.h"
#include "pch.h"
#include "sigscan.h"

#define DLLBASIC_API extern "C" __declspec(dllexport)


static INT(WINAPI* TrueMessageBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) = MessageBoxA;

DLLBASIC_API INT WINAPI Ch4njunMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
	return TrueMessageBoxA(hWnd, lpText, lpCaption, uType);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)TrueMessageBoxA, Ch4njunMessageBoxA);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueMessageBoxA, Ch4njunMessageBoxA);
		DetourTransactionCommit();
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}