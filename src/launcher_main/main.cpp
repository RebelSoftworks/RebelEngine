//========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
//
// This software is proprietary and confidential.
//
// You are granted a limited, non-exclusive, non-transferable license to use and
// modify this software for personal or internal purposes only. You may distribute
// compiled binary builds of the software provided that such distribution does not
// expose, include, or facilitate access to the source code.
//
// You may not distribute, publish, disclose, or otherwise make available the
// source code, in whole or in part, whether original or modified. All rights
// not expressly granted herein are reserved by the copyright holder.
//
// This software is provided "as is", without warranty of any kind, express or
// implied.
//
//=============================================================================//

#include "tier0/types.h"
#include "tier0/platform.h"
#include "steam/steam_api.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <cwchar>
#include <vector>

// use the god damn dedicated graphics!
#ifdef WIN32
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }
extern "C" { __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; }
#endif

#define DEFAULT_GAME_DIR "vr"

DLL_EXPORT bool BSecureAllowed(unsigned char* a1, int a2, int a3)
{
	return true;
}

DLL_EXPORT int BinaryProperties_GetValue(int a1, void* a2)
{
	return 0;
}

DLL_EXPORT int CountFilesCompletedTrustCheck()
{
	return 0;
}

DLL_EXPORT int CountFilesNeedTrustCheck()
{
	return 0;
}

DLL_EXPORT int CountItemsToReport()
{
	return 0;
}

DLL_EXPORT int GetTotalFilesLoaded()
{
	return 0;
}

[[nodiscard]] static void* LoadSteam(const char* szBaseDir)
{
	void* hSteam = nullptr;
#ifndef POSIX
	hSteam = (void*)LoadLibraryExW(L"steam_api64.dll", NULL, 0);
#else
	hSteam = (void*)dlopen("libsteam_api.so", RTLD_NOW);
#endif // !POSIX
	return hSteam;
}

[[nodiscard]] void InstallSteamProxy(const char* szBaseDir)
{
	void* hSteam = LoadSteam(szBaseDir);
	if (!hSteam)
		Error("Could not find Steam\n");
}

static char* GetBaseDir(const char* pszBuffer)
{
	static char	basedir[MAX_PATH];
	char szBuffer[MAX_PATH];
	size_t j;
	char* pBuffer = NULL;

	strcpy(szBuffer, pszBuffer);

	pBuffer = strrchr(szBuffer, '\\');
	if (pBuffer)
	{
		*(pBuffer + 1) = '\0';
	}

	strcpy(basedir, szBuffer);

	j = strlen(basedir);
	if (j > 0)
	{
		if ((basedir[j - 1] == '\\') ||
			(basedir[j - 1] == '/'))
		{
			basedir[j - 1] = 0;
		}
	}

	return basedir;
}

typedef void(*Source2Main_t)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd, const char* szBaseDir, const char* szGame);

int main(int argc, char** argv) 
{
	char* pPath = getenv("PATH");
	char moduleName[MAX_PATH];
	char szBuffer[4096];

	GetModuleFileNameA(NULL, moduleName, MAX_PATH);

	char* pRootDir = GetBaseDir(moduleName);
	_snprintf(szBuffer, sizeof(szBuffer), "PATH=%s\\bin\\win64\\;%s\\vr\\bin\\win64\\;%s", pRootDir, pPath);
	szBuffer[sizeof(szBuffer) - 1] = '\0';
	_putenv(szBuffer);

	_snprintf(szBuffer, sizeof(szBuffer), "%s\\bin\\win64\\engine2.dll", pRootDir);
	szBuffer[sizeof(szBuffer) - 1] = '\0';

	// this is the most important thing!
	// praise the king, gabe newell!
	InstallSteamProxy(GetBaseDir(moduleName));

	HMODULE launcher = LoadLibraryEx(szBuffer, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	if (!launcher)
	{
		MessageBoxW(NULL, L"Engine2 was not found", L"Rebel Engine Launcher Error", MB_OK);
		return 0;
	}

	Source2Main_t pSource2Main = (Source2Main_t)GetProcAddress(launcher, "Source2Main");
	if (!pSource2Main)
	{
		MessageBoxW(NULL, L"Could not find Source2Main from engine2", L"Rebel Engine Launcher Error", MB_OK);
		return 1;
	}
	
	const char* szGameName = DEFAULT_GAME_DIR;

	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "-game"))
		{
			if (i < argc - 1)
				szGameName = argv[i + 1];
			break;
		}
	}

	pSource2Main(GetModuleHandle(NULL), NULL, GetCommandLine(), 0, GetBaseDir(szBuffer), szGameName);
	return 0;
}