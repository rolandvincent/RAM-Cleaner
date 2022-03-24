#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <chrono>
#include <conio.h>

void SetWorkingSet(DWORD processID)
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | 
        PROCESS_VM_READ | PROCESS_SET_LIMITED_INFORMATION | PROCESS_SET_QUOTA,
        FALSE, processID);

    if (NULL != hProcess)
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
            &cbNeeded))
        {
            GetModuleBaseName(hProcess, hMod, szProcessName,
                sizeof(szProcessName) / sizeof(TCHAR));
        }
        int working = SetProcessWorkingSetSize(hProcess, -1, -1);
        if (working != 1) {
            _tprintf(TEXT("Failed\t: %s  (PID: %u)\n"), szProcessName, processID);
            CloseHandle(hProcess);
            return;
        }
        CloseHandle(hProcess);
    }

    _tprintf(TEXT("Success\t: %s  (PID: %u)\n"), szProcessName, processID);
}

int main()
{
    std::cout << "Created by Roland Vincent\n";
    std::cout << "C++ Application\n\n";
    std::cout << "[] Starting RAM Cleaner\n";
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return 1;

    cProcesses = cbNeeded / sizeof(DWORD);

    for (i = 0; i < cProcesses; i++)
        if (aProcesses[i] != 0)
            SetWorkingSet(aProcesses[i]);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "[] Finished in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms\n";
    std::cout << "[Press any key to continue]"; _getch();
    return 0;
}