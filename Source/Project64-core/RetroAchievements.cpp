#include "stdafx.h"

#include "RetroAchievements.h"

#include "../../RAInterface/RA_Consoles.h"
#include "../../RAInterface/RA_Emulators.h"
#include <Project64-core/N64System/N64System.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Plugin.h>
#include <Project64-core/RA_BuildVer.h>
#include <Project64-core/Settings.h>
#include <Project64-core/N64System/Mips/SystemEvents.h>
#include <Project64-plugin-spec/Input.h>

static HWND g_hWnd = nullptr;
static const char* g_sFileBeingLoaded = nullptr;

static void CauseUnpause()
{
    g_BaseSystem->ExternalEvent(SysEvent_ResumeCPU_FromMenu);
}

static void CausePause()
{
    g_BaseSystem->ExternalEvent(SysEvent_PauseCPU_FromMenu);
}

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

void RA_RebuildMenu()
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

static void GetEstimatedGameTitle(char* sNameOut)
{
    if (g_sFileBeingLoaded != nullptr)
    {
        const char* sLastSlash = nullptr;
        const char* sFilename = g_sFileBeingLoaded;
        while (*sFilename) {
            if (*sFilename == '\\')
                sLastSlash = sFilename;

            sFilename++;
        }

        sFilename = (sLastSlash) ? (sLastSlash + 1) : g_sFileBeingLoaded;
        strncpy(sNameOut, sFilename, 256);
    }
    else
    {
        strncpy(sNameOut, "?", 256);
    }
}

void RA_IdentifyGame(const char* sFilename, uint8_t* pData, size_t nSize)
{
    g_sFileBeingLoaded = sFilename;
    RA_OnLoadNewRom(pData, nSize);
    g_sFileBeingLoaded = nullptr;
}

static void ResetEmulator()
{
    g_BaseSystem->ExternalEvent(SysEvent_ResetCPU_Hard);
}

static void LoadROM(const char* sFullPath) {}

void RA_Init(HWND hMainWindow)
{
    // initialize the DLL
    RA_Init(hMainWindow, RA_Project64, RAPROJECT64_VERSION);
    RA_SetConsoleID(N64);
    g_hWnd = hMainWindow;

    // provide callbacks to the DLL
    RA_InstallSharedFunctions(NULL, CauseUnpause, CausePause, RA_RebuildMenu, GetEstimatedGameTitle, ResetEmulator, LoadROM);

    // add a placeholder menu item and start the login process - menu will be updated when login completes
    RA_RebuildMenu();
    RA_AttemptLogin(false);

    // ensure titlebar text matches expected format
    RA_UpdateAppTitle("");
}

void RA_ProcessInputs()
{
    if (RA_IsOverlayFullyVisible())
    {
        BUTTONS Keys;
        memset(&Keys, 0, sizeof(Keys));
        g_Plugins->Control()->GetKeys(0, &Keys); // Get keys from 1st player controller.

        ControllerInput input;
        input.m_bUpPressed = Keys.U_DPAD;
        input.m_bDownPressed = Keys.D_DPAD;
        input.m_bLeftPressed = Keys.L_DPAD;
        input.m_bRightPressed = Keys.R_DPAD;
        input.m_bCancelPressed = Keys.B_BUTTON;
        input.m_bConfirmPressed = Keys.A_BUTTON;
        input.m_bQuitPressed = Keys.START_BUTTON;

        RA_NavigateOverlay(&input);
    }
}
