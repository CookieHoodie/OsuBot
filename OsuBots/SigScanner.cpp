#include "SigScanner.h"


DWORD SigScanner::findPattern(HANDLE processHandle, const unsigned char pattern[], const char* mask, const int offset, size_t begin) {
	// pattern in format: unsigned char pattern[] = { 0x90, 0xFF, 0xEE };
	// mask in format: char* mask = "xxx?xxx";
	// begin default is 0
	// this function searches the signature from begin or 0x00000000(default) to 0x7FFFFFFF
	const size_t signature_size = strlen(mask);
	const size_t read_size = 4096;
	bool hit = false;

	unsigned char chunk[read_size];

	for (size_t i = begin; i < INT_MAX; i += read_size - signature_size) {
		ReadProcessMemory(processHandle, LPCVOID(i), &chunk, read_size, NULL);

		for (size_t a = 0; a < read_size; a++) {
			hit = true;

			for (size_t j = 0; j < signature_size && hit; j++) {
				if (mask[j] != '?' && chunk[a + j] != pattern[j]) {
					hit = false;
				}
			}

			if (hit) {
				return i + a + offset;
			}
		}
	}
	
	return NULL;
}

//void* SigScanner::patternScan(char* base, size_t size, char* pattern, char* mask)
//{
//	size_t patternLength = strlen(mask);
//
//	for (unsigned int i = 0; i < size - patternLength; i++)
//	{
//		bool found = true;
//		for (unsigned int j = 0; j < patternLength; j++)
//		{
//			if (mask[j] != '?' && pattern[j] != *(base + i + j))
//			{
//				found = false;
//				break;
//			}
//		}
//		if (found)
//		{
//			return (void*)(base + i);
//		}
//	}
//	return nullptr;
//}
//
//void* SigScanner::patternScanEx(HANDLE osuProcessHandle, uintptr_t begin, uintptr_t end, char* pattern, char* mask, int offset)
//{
//	uintptr_t currentChunk = begin;
//	SIZE_T bytesRead;
//
//	while (currentChunk < end)
//	{
//		char buffer[4096];
//
//		DWORD oldprotect;
//		VirtualProtectEx(osuProcessHandle, (void*)currentChunk, sizeof(buffer), PAGE_EXECUTE_READWRITE, &oldprotect);
//		ReadProcessMemory(osuProcessHandle, (void*)currentChunk, &buffer, sizeof(buffer), &bytesRead);
//		VirtualProtectEx(osuProcessHandle, (void*)currentChunk, sizeof(buffer), oldprotect, &oldprotect);
//
//		if (bytesRead == 0)
//		{
//			return nullptr;
//		}
//
//		void* internalAddress = SigScanner::patternScan((char*)&buffer, bytesRead, pattern, mask);
//
//		if (internalAddress != nullptr)
//		{
//			//calculate from internal to external
//			uintptr_t offsetFromBuffer = (uintptr_t)internalAddress - (uintptr_t)&buffer;
//			return (void*)(currentChunk + offsetFromBuffer + offset);
//		}
//		else
//		{
//			//advance to next chunk
//			currentChunk = currentChunk + bytesRead;
//		}
//	}
//	return nullptr;
//}


//void* SigScanner::patternScanEx(DWORD processID, uintptr_t begin, uintptr_t end, char* pattern, char* mask, int offset)
//{
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
//	uintptr_t currentChunk = begin;
//	SIZE_T bytesRead;
//
//	while (currentChunk < end)
//	{
//		char buffer[4096];
//
//		DWORD oldprotect;
//		VirtualProtectEx(hProcess, (void*)currentChunk, sizeof(buffer), PAGE_EXECUTE_READWRITE, &oldprotect);
//		ReadProcessMemory(hProcess, (void*)currentChunk, &buffer, sizeof(buffer), &bytesRead);
//		VirtualProtectEx(hProcess, (void*)currentChunk, sizeof(buffer), oldprotect, &oldprotect);
//
//		if (bytesRead == 0)
//		{
//			return nullptr;
//		}
//
//		void* internalAddress = SigScanner::patternScan((char*)&buffer, bytesRead, pattern, mask);
//
//		if (internalAddress != nullptr)
//		{
//			//calculate from internal to external
//			uintptr_t offsetFromBuffer = (uintptr_t)internalAddress - (uintptr_t)&buffer;
//			return (void*)(currentChunk + offsetFromBuffer + offset);
//		}
//		else
//		{
//			//advance to next chunk
//			currentChunk = currentChunk + bytesRead;
//		}
//	}
//	return nullptr;
//}

//void* SigScanner::patternScanExModule(DWORD processID, wchar_t* module, char* pattern, char* mask, int offset)
//{
//	MODULEENTRY32 modEntry = ProcessTools::getModule(processID, module);
//
//	if (!modEntry.th32ModuleID)
//	{
//		return nullptr;
//	}
//	uintptr_t begin = (uintptr_t)modEntry.modBaseAddr;
//	uintptr_t end = begin + modEntry.modBaseSize;
//	return SigScanner::patternScanEx(processID, begin, end, pattern, mask, offset);
//}