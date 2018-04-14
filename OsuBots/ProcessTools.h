#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <string>

using namespace std;

class ProcessTools
{
public:
	static DWORD getProcessID(const wchar_t* processName);
	static HWND getWindowHandle(DWORD processID);
	static string getWindowTextString(HWND windowHandle);
	//static int readFromMemory(DWORD processID, void* memoryAddress);
	//static bool readFromMemory(HANDLE hProcess, void* memoryAddress, void* output, int size);
	//static int writeToMemory(DWORD processID, void* memoryAddress, byte* input, int size);
	/*static int writeToMemory(DWORD processID, int memoryAddress, int input);*/
	//static MODULEENTRY32 getModule(DWORD processID, const wchar_t* processName);
};

//TODO: optimization of sigscanner by starting from module base address
//		optimization of searching signatures by assigning different thread