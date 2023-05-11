#include "stdio.h"
#include "string.h"
#include "windows.h"
#include <psapi.h>
#include <processthreadsapi.h>

DWORD GetProcessPID(LPWSTR name) {
    DWORD process_id_array[1024];
    DWORD bytes_returned;
    DWORD num_processes;
    HANDLE hProcess;
    WCHAR image_name[MAX_PATH]={0,};

    DWORD i;
    EnumProcesses(process_id_array, 1024 * sizeof(process_id_array), &bytes_returned);
    num_processes = (bytes_returned/sizeof(DWORD));

    for (i = 0; i < num_processes; i++) {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, process_id_array[i]);
        if (GetModuleBaseName(hProcess, 0, image_name, 256)) {
            if (!wcscmp(image_name, name)) {
                CloseHandle(hProcess);
                return process_id_array[i];
            }
        }
        CloseHandle(hProcess);
    }
    
}


int main(int argc, char* argv[]) {
    LPWSTR name = L"notepad.exe";
    DWORD processId = GetProcessPID(name);
    char* path = argv[1];

    if (argc < 1) {
        printf("����\n");
        return 0;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        printf("Pid�� �������� ����\n");
        return 0;
    }
    printf("%d\n", processId);
    PVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(path)+1, MEM_COMMIT, PAGE_READWRITE);
    
    if (!pDllPath) {
        printf("�޸� �Ҵ� ����");
        return 0;
    }

    if (!WriteProcessMemory(hProcess, pDllPath, path, strlen(path) + 1, 0)) {
        printf("�޸� ���� ����");
        return 0;
    }

    HANDLE hLoadThread = CreateRemoteThread(hProcess, 0,0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllPath, 0,0);

    WaitForSingleObject(hLoadThread , INFINITE);

    CloseHandle(hLoadThread);
    CloseHandle(hProcess);

    return 0;
}