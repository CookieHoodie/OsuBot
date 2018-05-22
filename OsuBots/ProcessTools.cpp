#include "ProcessTools.h"

DWORD ProcessTools::getProcessID(const wchar_t* processName)
// get processID from processName given
// processName etc. L"chrome.exe"
// return processID if found, null if not
{
	DWORD process_id = NULL;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	PROCESSENTRY32 entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szExeFile, processName) == 0) {
				process_id = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return process_id;
}

// window handle for calculating cursor
HWND ProcessTools::getWindowHandle(DWORD processID)
{
	 //This works fine until the osu is in fullscreen--the window title cannot be retrieved from this handle
	std::pair<HWND, DWORD> params = { 0, processID };

	// Enumerate the windows using a lambda to process each window
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
	{
		auto pParams = (std::pair<HWND, DWORD>*)(lParam);

		DWORD processId;
		if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
		{
			// Stop enumerating
			SetLastError(-1);
			pParams->first = hwnd;
			return FALSE;
		}

		// Continue enumerating
		return TRUE;
	}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}

// get windowHandle for retrieving window title
HWND ProcessTools::getWindowTitleHandle(LPCSTR windowTitle) {
	HWND windowHandle = FindWindowA(NULL, windowTitle);
	return windowHandle; // return NULL if not found
}

// read title in string
string ProcessTools::getWindowTextString(HWND windowHandle) {
	char ctitle[255];
	GetWindowTextA(windowHandle, ctitle, 255);
	return string(ctitle);
}

// prompt user to choose file and return the filename in string
string ProcessTools::promptToChooseFileAndGetPath(LPCWSTR customTitle) {
	IFileDialog *pfd;
	string returnStr = "";
	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) 
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pfd))))
		{
			if (customTitle != L"") {
				pfd->SetTitle(customTitle);
			}
			if (SUCCEEDED(pfd->Show(NULL)))
			{
				IShellItem *psi;
				if (SUCCEEDED(pfd->GetResult(&psi)))
				{
					PWSTR pszFilePath;
					if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
					{
						wstring wpath(pszFilePath);
						returnStr = string(wpath.begin(), wpath.end());
					}
					psi->Release();
				}
			}
			pfd->Release();
		}
		CoUninitialize();
	}
	return returnStr;
}

//int ProcessTools::writeToMemory(DWORD processID, void* memoryAddress, byte* input, int size)
//// write value in input to memoryAddress given
//// size of input determines how many bytes of data to write to the specific address
//// return 0 if write fails, other than 0 if successes
//{
//	int result = 0;
//	HANDLE handle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, processID);
//	if (handle != NULL) {
//		result = WriteProcessMemory(handle, (LPVOID)memoryAddress, (void*)input, size, 0);
//	}
//	return result;
//}
//
//bool ProcessTools::readFromMemory(HANDLE hProcess, void* memoryAddress, void* output, int size)
//// read from memoryAddress given
//{
//	bool result = false;
//	SIZE_T bytesRead;
//	ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, output, size, &bytesRead);
//	if (bytesRead != 0) {
//		result = true;
//	}
//	
//	return result;
//}

//MODULEENTRY32 ProcessTools::getModule(DWORD processID, const wchar_t* processName) {
//	MODULEENTRY32 modEntry = { 0 };
//	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
//	if (snapshot != INVALID_HANDLE_VALUE) {
//		modEntry.dwSize = sizeof(MODULEENTRY32);
//		if (Module32First(snapshot, &modEntry)) {
//			do
//			{
//				if (!wcscmp(modEntry.szModule, processName)) {
//					break;
//				}
//			} while (Module32Next(snapshot, &modEntry));
//		}
//	}
//	CloseHandle(snapshot);
//	return modEntry;
//}
//
