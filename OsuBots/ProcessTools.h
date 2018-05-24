#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <Shobjidl.h>
#include <string>

#include <iostream>

using namespace std;

class ProcessTools
{
public:
	static DWORD getProcessID(const wchar_t* processName);
	static HWND getWindowHandle(DWORD processID);
	static HWND getWindowTitleHandle(LPCSTR windowTitle);
	static string getWindowTextString(HWND windowHandle);
	static string promptToChooseFileAndGetPath(LPCWSTR customTitle = L"");
	static bool saveConsoleBuffer(CONSOLE_SCREEN_BUFFER_INFO &csbi, PCHAR_INFO consoleBuffer);
	static bool restoreConsoleBuffer(CONSOLE_SCREEN_BUFFER_INFO &csbi, PCHAR_INFO consoleBuffer);
	//static int readFromMemory(DWORD processID, void* memoryAddress);
	//static bool readFromMemory(HANDLE hProcess, void* memoryAddress, void* output, int size);
	//static int writeToMemory(DWORD processID, void* memoryAddress, byte* input, int size);
	/*static int writeToMemory(DWORD processID, int memoryAddress, int input);*/
	//static MODULEENTRY32 getModule(DWORD processID, const wchar_t* processName);
};