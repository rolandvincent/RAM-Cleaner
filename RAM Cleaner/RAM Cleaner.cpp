#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <chrono>
#include <cstdint>

void SetWorkingSet(DWORD processID)
{
    char szProcessName[MAX_PATH] = "<unknown>";

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
                sizeof(szProcessName) / sizeof(char));
        }
        int working = SetProcessWorkingSetSize(hProcess, SIZE_MAX, SIZE_MAX);
        if (working != 1) {
            std::cout << "Failed\t: " << szProcessName << "  (PID: " << processID << ')' << std::endl;
            CloseHandle(hProcess);
            return;
        }
        CloseHandle(hProcess);
    }
    std::cout << "Success\t: " << szProcessName << "  (PID: " << processID << ')' << std::endl;
   
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cout << "Created by Roland Vincent\n";
    std::cout << "C++ Application\n\n";
    std::cout << "[] Starting RAM Cleaner" << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    DWORD aProcesses[1024], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return 1;

    cProcesses = cbNeeded / sizeof(DWORD);

    for (DWORD i = 0; i < cProcesses; i++)
        if (aProcesses[i] != 0)
            SetWorkingSet(aProcesses[i]);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "[] Finished in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;
    std::cout << "[Press any key to continue]"; std::cin.get();
    return 0;
}
