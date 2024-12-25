#pragma once
#include <iostream>
#include <fstream>
#include <urlmon.h>
#include <ShlObj.h>
#include <Psapi.h>
#include <d3d9.h>
#include <tchar.h>

#include "variables.h"
#pragma comment(lib, "urlmon.lib")

class Injection {
public:
	static void Inject() noexcept {
		std::string destinationPath = GetAppDataPath() + "\\" + _name;
		if (!isAlreadyInject())
			if (DownloadDLL(_url, destinationPath.c_str())) InjectDLL(destinationPath);
			else std::cerr << "falha ao instalar a dll.\n" << std::endl;
		else std::cout << "dll ja esta injetada.\n" << std::endl;
	}
private:
	static bool isAlreadyInject() {
		DWORD procId;
		HWND windowfm = FindWindowA("grcWindow", 0);
		GetWindowThreadProcessId(windowfm, &procId);
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId);
		if (hProc && hProc != INVALID_HANDLE_VALUE) {
			HMODULE hMods[1024];
			DWORD cbNeeded;
			if (EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded)) {
				for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
					char szModName[MAX_PATH];
					if (GetModuleFileNameExA(hProc, hMods[i], szModName, sizeof(szModName))) {
						if (strstr(szModName, _name)) {
							CloseHandle(hProc);
							return true;
						}
					}
				}
			}
			CloseHandle(hProc);
		}
		return false;
	}

	static std::string GetAppDataPath() {
		PWSTR appDataPath;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appDataPath))) {
			std::wstring appdataPathWString(appDataPath);
			CoTaskMemFree(appDataPath);
			return std::string(appdataPathWString.begin(), appdataPathWString.end());
		}
		else return "";
	}

	static bool DownloadDLL(const char* url, const char* destinationPath) {
		HRESULT hr = URLDownloadToFileA(NULL, url, destinationPath, 0, NULL);
		if (SUCCEEDED(hr)) return true;
		else {
			std::cerr << "URLDownloadToFileA falhou com o codigo de erro: " << hr << std::endl;
			return false;
		}
	}

	static void InjectDLL(const std::string& destinationPath) {
		char path[MAX_PATH + 1];
		strcpy_s(path, destinationPath.c_str());
		DWORD procId;
		HWND windowfm = FindWindowA("grcWindow", 0);
		GetWindowThreadProcessId(windowfm, &procId);


		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
		if (hProc && hProc != INVALID_HANDLE_VALUE) {
			void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			WriteProcessMemory(hProc, loc, path, strlen(path) + 1, 0);
			HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
			if (hThread) CloseHandle(hThread);
			std::cout << "Injetado. \n";
		}
	}
};
