#include "stdafx.h"

#include "RetroAchievements.h"

#include "../../RAInterface/RA_Consoles.h"
#include "../../RAInterface/RA_Emulators.h"
#include <Project64-core/RA_BuildVer.h>

static HWND g_hWnd;

static void CauseUnpause() {}
static void CausePause() {}

static int GetMenuItemIndex(HMENU hMenu, const char* pItemName)
{
    int nIndex = 0;
    char pBuffer[256];

    while (nIndex < GetMenuItemCount(hMenu))
    {
        if (GetMenuStringA(hMenu, nIndex, pBuffer, sizeof(pBuffer)-1, MF_BYPOSITION))
        {
            if (!strcmp(pItemName, pBuffer))
                return nIndex;
        }
        nIndex++;
    }

    return -1;
}

static void RebuildMenu()
{
    HMENU hMainMenu = GetMenu(g_hWnd);
    if (!hMainMenu)
        return;

    // if RetroAchievements submenu exists, destroy it
    int index = GetMenuItemIndex(hMainMenu, "&RetroAchievements");
    if (index >= 0)
        DeleteMenu(hMainMenu, index, MF_BYPOSITION);

    // append RetroAchievements menu
    AppendMenu(hMainMenu, MF_POPUP|MF_STRING, (UINT_PTR)RA_CreatePopupMenu(), TEXT("&RetroAchievements"));

    // repaint
    DrawMenuBar(g_hWnd);
}

static void GetEstimatedGameTitle(char* sNameOut) {}
static void ResetEmulator() {}
static void LoadROM(const char* sFullPath) {}

void RA_Init(HWND hMainWindow)
{
    // initialize the DLL
    RA_Init(hMainWindow, RA_Project64, RAPROJECT64_VERSION);
    RA_SetConsoleID(N64);
    g_hWnd = hMainWindow;

    // provide callbacks to the DLL
    RA_InstallSharedFunctions(NULL, CauseUnpause, CausePause, RebuildMenu, GetEstimatedGameTitle, ResetEmulator, LoadROM);

    // add a placeholder menu item and start the login process - menu will be updated when login completes
    RebuildMenu();
    RA_AttemptLogin(false);

    // ensure titlebar text matches expected format
    RA_UpdateAppTitle("");
}
