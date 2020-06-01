#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

HANDLE hProcSnap = NULL;
PROCESSENTRY32 procEntry32;
DWORD pID = NULL;
HANDLE hProc = NULL;
HANDLE hModuleSnap = NULL;
MODULEENTRY32 modEntry32;

bool attachProc(char* procName)
{
	procEntry32.dwSize = sizeof(PROCESSENTRY32);

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to take snapshot of process list." << std::endl;
		return false;
	}

	while (Process32Next(hProcSnap, &procEntry32))
	{
		std::cout << procEntry32.szExeFile << std::endl;
		if (!strcmp(procName, procEntry32.szExeFile))
		{
			std::cout << "Found process " << procEntry32.szExeFile << " with process ID " << procEntry32.th32ProcessID << std::endl;
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry32.th32ProcessID);
			pID = procEntry32.th32ProcessID;

			if (hProc == NULL)
			{
				std::cout << "Failed getting handle to process." << std::endl;
			}
			CloseHandle(hProcSnap);
			return true;
		}
	}

	std::cout << "Couldn't find " << procName << " in the process snapshot." << std::endl;
	CloseHandle(hProcSnap);
	return false;
}

template <class dataType>
void wpm(dataType valToWrite, DWORD addressToWrite)
{
	WriteProcessMemory(hProc, (PVOID)addressToWrite, &valToWrite, sizeof(dataType), 0);
}

template <class dataType>
dataType rpm(DWORD addressToRead)
{
	dataType rpmBuffer;

	ReadProcessMemory(hProc, (PVOID)addressToRead, &rpmBuffer, sizeof(dataType), 0);

	return rpmBuffer;
}

int main()
{
	// Define Memory Addresses
	DWORD healthOffset = 0x00000001;

	// Attach To The Process
	attachProc((char*)"Process.exe");

	while (true)
	{
		// Modify The Memory Addresses Values
		wpm<int>(100, healthOffset);
	}
}