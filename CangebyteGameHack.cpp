#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <vector>
#include <list>

using namespace std;


DWORD GetModule(const TCHAR* IpszModuleName, DWORD pID)
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pID);
	if (handle != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 entry;
		entry.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(handle, &entry))
		{
			do
			{
				if (!_strcmpi(IpszModuleName, entry.szModule))
				{
					CloseHandle(handle);
					return (DWORD)entry.modBaseAddr;
				}
			} while (Module32Next(handle, &entry));
		}
	}
	return 0;
}

intptr_t CreatePointerVarList(HANDLE process, intptr_t pointer, list<int> offsetList)
{
	intptr_t address = pointer;
	for (int offset : offsetList)
	{
		ReadProcessMemory(process, (void*)address, &address, sizeof(address), 0);
		address += offset;
	}
	return address;
}
// تعديل كود الاسمبلي
void changecodegame(void* destination, void* newgamecode, int size, HANDLE process)
{
	DWORD oldprotection;
	VirtualProtectEx(process, destination, size, PAGE_EXECUTE_READWRITE, &oldprotection);
	WriteProcessMemory(process, destination, newgamecode, size, 0);
	VirtualProtectEx(process, destination, size, oldprotection, &oldprotection);
}

int main()
{
	HWND gamewindow = FindWindow(NULL, "AssaultCube");
	if (gamewindow == NULL)
	{
		cout << "This game not found !";
		return 0;
	}
	DWORD PID = NULL;
	GetWindowThreadProcessId(gamewindow, &PID);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == INVALID_HANDLE_VALUE || hProcess == NULL)
	{
		cout << "feiled to open Process";
		return 0;
	}
	char modulname[] = "ac_client.exe";
	printf_s("%p\n", GetModule(modulname, PID));
	DWORD gameBaseAddress = GetModule(modulname, PID);
	intptr_t baseoffset = gameBaseAddress + 0x17E0A8;
	int health = 0xEC;
	int ammo = 0x140;
	DWORD localPlayer;
	ReadProcessMemory(hProcess, (void*)baseoffset, &localPlayer, sizeof(localPlayer), 0);

	bool muntion = false;
	while (true)
	{
		Sleep(1);
		if (GetAsyncKeyState(VK_LBUTTON) < 0)
		{
			changecodegame((void*)(gameBaseAddress + 0xC73EF), (void*)"\xFF\x00", 2, hProcess);
			/*muntion = !muntion;
			if (muntion)
			{
				changecodegame((void*)(gameBaseAddress + 0xC73EF), (void*)"\xFF\x08", 2, hProcess);
			}
			else
			{
				changecodegame((void*)(gameBaseAddress + 0xC73EF), (void*)"\xFF\x00", 2, hProcess);
			}*/
		}
	}
	//changecodegame((void*)(localPlayer + 0xC73EF), (void*)"\xFF\x08", 2, hProcess);
}
