#pragma once

#include <Windows.h>

class SigScanner
{
public:
	static DWORD findPattern(HANDLE processHandle, const unsigned char pattern[], const char* mask, const int offset, size_t begin = 0);
	//static void* patternScan(char* base, size_t size, char* pattern, char* mask);
	//static void* patternScanEx(HANDLE osuProcessHandle, uintptr_t begin, uintptr_t end, char* pattern, char* mask, int offset);
	//static void* patternScanEx(DWORD processID, uintptr_t begin, uintptr_t end, char* pattern, char* mask, int offset);
	//static void* patternScanExModule(DWORD processID, wchar_t* module, char* pattern, char* mask, int offset);
};

