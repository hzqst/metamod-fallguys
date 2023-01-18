#pragma once

#include <string>

void RegisterAngelScriptMethods();
void RegisterAngelScriptHooks();

void ASQCvar_Shutdown();
bool ASQCvar_SetQueryCvar2Callback(int request_id, aslScriptFunction *aslfn);
void ASQCvar_CallQueryCvar2Callback(void *pPlayer, int request_id, const char *cvarName, const char *value);

extern void * g_QueryCvarHook;
extern void * g_QueryCvar2Hook;