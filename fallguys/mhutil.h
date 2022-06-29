#pragma once

void *MH_GetModuleBase(const char *name);

size_t MH_GetModuleSize(void *hModule);

void *MH_SearchPattern(void *pStartSearch, size_t dwSearchLen, const char *pPattern, size_t dwPatternLen);

bool MH_IsAddressInModule(void *lpAddress, void *hModule);