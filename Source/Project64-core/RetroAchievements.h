#ifndef __RETROACHIEVEMENTS_H_
#define __RETROACHIEVEMENTS_H_

#include "../../RAInterface/RA_Interface.h"

void RA_Init(HWND hMainWindow);
void RA_RebuildMenu();
void RA_ProcessInputs();

void RA_IdentifyGame(const char* sFilename, uint8_t* pData, size_t nSize);

#endif __RETROACHIEVEMENTS_H_
