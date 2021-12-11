#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>


//Pass process name as argument
DWORD GetProcID(const char* procName)
{
    DWORD procID = 0;
    //Create snapshot of current processes
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    //Check to make sure process handle is not invalid(-1)
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        //Receive each process entry from snapshot
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        //Get first process
        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                //String insensitive comparison
                if (!_strcmpi(procEntry.szExeFile, procName))
                {
                    //Grab process id
                    procID = procEntry.th32ProcessID;
                    //Break loop
                    break;

                }
                //Continue to receive process entry
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    //Close handle
    CloseHandle(hSnap);
    return procID;
}

//Getting base module address
uintptr_t GetModuleBaseAddress(DWORD procId, const char* modName)
{
    uintptr_t modBaseAddr = 0;
    //Creating snapshot of modules
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

    //Iterating through modules
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        //Finding the module we need
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_stricmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    //Close handle and return module base address
    CloseHandle(hSnap);
    return modBaseAddr;
}

//Patching winapi function
void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
    //Overwriting to return 0 
    DWORD oldprotect;
    VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    WriteProcessMemory(hProcess, dst, src, size, nullptr);
    VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

int main()
{

    SetConsoleTitleA("Smuggler");
    //Setting dll and procname
    std::cout << "   _____                             __             " << std::endl;
    std::cout << "  / ___/____ ___  __  ______ _____ _/ /__  _____    " << std::endl;
    std::cout << "  \\__ \\/ __ `__ \\/ / / / __ `/ __ `/ / _ \\/ ___/    " << std::endl;
    std::cout << " ___/ / / / / / / /_/ / /_/ / /_/ / /  __/ /        " << std::endl;
    std::cout << "/____/_/ /_/ /_/\\__,_/\\__, /\\__, /_/\\___/_/         " << std::endl;
    std::cout << "                     /____//____/                   " << std::endl;


    const char* dllPath = "C:\\Users\\colle\\Desktop\\hook.dll";
    const char* procName = "Thetan Arena.exe";
    DWORD procID = 0;

    while (!procID)
    {
        //Calling GetProcID with our procname
        std::cout << "[+] - Fetching Process ID" << std::endl;
        Sleep(100);
        procID = GetProcID(procName);
        Sleep(30);
    }

    //Call OpenProcess with all access (read/write)
    std::cout << "[+] - Creating Handle" << std::endl;
    Sleep(100);
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);


    std::cout << "[+] - Patching IsDebuggerPresent" << std::endl;
    Sleep(500);
    PatchEx((BYTE*)IsDebuggerPresent, (BYTE*)"\xB8\x0\x0\x0\x0\xC3", 6, hProc);
    //If the process is not null and not an invalid handle
    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        //Allocate memory in an external process, passing a process, max path for a string path, mem commit for committing real or reserved memory and readwrite
        //Not putting dll into memory, just putting the path in
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        //Writing the dll path into memory to create a remote thread in the target process to call LoadLibraryA 
        if (loc)
        {
            WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
        }
        
        //Create remote thread with our handle, start load library winapi call
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

        if (hThread)
        {
            //Closing thread
            std::cout << "[+] - Closing Thread" << std::endl;
            Sleep(100);
            CloseHandle(hThread);
        }

    }

    if (hProc)
    {
        std::cout << "[+] - Closing Process Handle" << std::endl;
        Sleep(100);
        CloseHandle(hProc);
    }
    std::cout << "[+] - Dll has been smuggled in" << std::endl;
    system("pause");
    return 0;
}

