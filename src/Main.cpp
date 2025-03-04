#include <iostream>
#include <Windows.h>
#include <iomanip>
#include "Mem.h"

DWORD p1, p2;
uintptr_t playerAddress;
BOOL s = FALSE;
uintptr_t patchAddress1, patchAddress2;
uintptr_t patchOffset1 = 0x1485339, patchOffset2 = 0x154FEE7;
BYTE patch1[] = { 0x89, 0x41, 0x24};
BYTE patch2[] = { 0x89, 0x42, 0x14};
BYTE doesnothing[] = {0x90, 0x90, 0x90};

void setColor(int colorCode) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorCode);
}

bool IsRunAsAdmin()
{
	BOOL isAdmin = FALSE;
	PSID adminGroup = NULL;
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
	{
		CheckTokenMembership(NULL, adminGroup, &isAdmin);
		FreeSid(adminGroup);
	}
	return isAdmin;
}


int main()
{
	//Initialize memory class
	Mem mem = Mem(L"AmongUs3D.exe");

	//Check if the program is running as an administrator
	if (!IsRunAsAdmin())
	{
		std::cout << "This program must be run as an administrator!" << std::endl;
		getchar();
		return 1;
	}

	//Initialize
	uintptr_t baseAddress = mem.GetModuleAddress(L"UnityPlayer.dll");
	patchAddress1 = baseAddress + patchOffset1;
	patchAddress2 = baseAddress + patchOffset2;
	uintptr_t offset[] = { 0x01C18410, 0x8, 0x0, 0x58, 0x0, 0x0, 0x88 };
	size_t numOffsets = sizeof(offset) / sizeof(uintptr_t);
	playerAddress = mem.ReadPointerChain<uintptr_t>(baseAddress, offset, numOffsets);

	//check if the player address is valid
	float x = mem.ReadMemory<float>(playerAddress + 0x10);
	float y = mem.ReadMemory<float>(playerAddress + 0x14);
	float z = mem.ReadMemory<float>(playerAddress + 0x18);

	std::cout << "*Important* Make sure you are in a game before toggling, and toggle off before joining another game!" << std::endl;
	std::cout << "Player Address: " << std::hex << playerAddress << std::endl;
	std::cout << "Player Position: " << x << ", " << y << ", " << z << std::endl;
	std::cout << "Press F1 to toggle" << std::endl;

	//Loop
	while (true)
	{
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            system("cls");
            s = !s;

            if (s)
            {
                setColor(2);
                VirtualProtect((LPVOID)patchAddress1, sizeof(patch1), PAGE_EXECUTE_READWRITE, &p1);
                VirtualProtect((LPVOID)patchAddress2, sizeof(patch2), PAGE_EXECUTE_READWRITE, &p2);

                WriteProcessMemory(mem.GetProcessHandle(), (LPVOID)patchAddress1, doesnothing, sizeof(patch1), nullptr);
                WriteProcessMemory(mem.GetProcessHandle(), (LPVOID)patchAddress2, doesnothing, sizeof(patch2), nullptr);
				mem.WriteMemory<float>(playerAddress + 0x14, -2.0f);

                VirtualProtect((LPVOID)patchAddress1, sizeof(patch1), p1, &p1);
                VirtualProtect((LPVOID)patchAddress2, sizeof(patch2), p2, &p2);

                std::cout << "[ON]" << std::flush;
            }
            else
            {
                setColor(4);

                VirtualProtect((LPVOID)patchAddress1, sizeof(patch1), PAGE_EXECUTE_READWRITE, &p1);
                VirtualProtect((LPVOID)patchAddress2, sizeof(patch2), PAGE_EXECUTE_READWRITE, &p2);

                WriteProcessMemory(mem.GetProcessHandle(), (LPVOID)patchAddress1, patch1, sizeof(patch1), nullptr);
                WriteProcessMemory(mem.GetProcessHandle(), (LPVOID)patchAddress2, patch1, sizeof(patch2), nullptr);

                VirtualProtect((LPVOID)patchAddress1, sizeof(patch1), p1, &p1);
                VirtualProtect((LPVOID)patchAddress2, sizeof(patch2), p2, &p2);

                std::cout << "[OFF]" << std::flush;
            }
        }

		Sleep(25);
	}
}