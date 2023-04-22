#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <vector>

#include "enginedef.h"
#include "serverdef.h"
#include "asusermsg.h"

CUserMsgHookManager g_UserMsgHookManager;

int CUserMsgHookManager::GetUserMsgId(const char *name)
{
	return GET_USER_MSG_ID(PLID, name, 0);
}

void CUserMsgHookManager::MessageBegin(int msgDest, int msgType, const float *origin, edict_t *pEdict)
{
	m_Hooked = true;
	m_msgDest = msgDest;
	m_msgType = msgType;
	if (origin)
		m_msgOrigin = Vector(origin[0], origin[1], origin[2]);
	else
		m_msgOrigin = g_vecZero;
	m_msgEntity = pEdict;
	m_msgArgCount = 0;
}

void CUserMsgHookManager::ClearMessage()
{
	m_Hooked = false;
	m_Blocked = false;
	m_msgDest = 0;
	m_msgType = 0;
	m_msgOrigin = g_vecZero;
	m_msgEntity = NULL;
	m_msgArgCount = 0;
}

void CUserMsgHookManager::SendBufferedMessage()
{
	g_engfuncs.pfnMessageBegin(g_UserMsgHookManager.GetMsgDest(), g_UserMsgHookManager.GetMsgType(), g_UserMsgHookManager.GetMsgOrigin(), g_UserMsgHookManager.GetMsgEdict());
	for (int i = 0; i < m_msgArgCount; ++i)
	{
		switch (m_msgArgs[i].m_iArgType)
		{
		case UserMsgArg_Byte:
		{
			g_engfuncs.pfnWriteByte(m_msgArgs[i].v.m_iData);
			break;
		}
		case UserMsgArg_Char:
		{
			g_engfuncs.pfnWriteChar(m_msgArgs[i].v.m_iData);
			break;
		}
		case UserMsgArg_Short:
		{
			g_engfuncs.pfnWriteShort(m_msgArgs[i].v.m_iData);
			break;
		}
		case UserMsgArg_Long:
		{
			g_engfuncs.pfnWriteShort(m_msgArgs[i].v.m_iData);
			break;
		}
		case UserMsgArg_Angle:
		{
			g_engfuncs.pfnWriteAngle(m_msgArgs[i].v.m_flData);
			break;
		}
		case UserMsgArg_Coord:
		{
			g_engfuncs.pfnWriteCoord(m_msgArgs[i].v.m_flData);
			break;
		}
		case UserMsgArg_String:
		{
			g_engfuncs.pfnWriteString(m_msgArgs[i].m_string.c_str());
			break;
		}
		case UserMsgArg_Entity:
		{
			g_engfuncs.pfnWriteEntity(m_msgArgs[i].v.m_iData);
			break;
		}
		}
	}
	g_engfuncs.pfnMessageEnd();
}

void CUserMsgHookManager::WriteByte(int iValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_Byte;
	m_msgArgs[m_msgArgCount].v.m_iData = iValue;
	m_msgArgCount++;
}

void CUserMsgHookManager::WriteChar(int iValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_Byte;
	m_msgArgs[m_msgArgCount].v.m_iData = iValue;
	m_msgArgCount++;
}

void CUserMsgHookManager::WriteShort(int iValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_Short;
	m_msgArgs[m_msgArgCount].v.m_iData = iValue;
	m_msgArgCount++;
}

void CUserMsgHookManager::WriteLong(int iValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_Long;
	m_msgArgs[m_msgArgCount].v.m_iData = iValue;
	m_msgArgCount++;
}

void CUserMsgHookManager::WriteAngle(float flValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_Angle;
	m_msgArgs[m_msgArgCount].v.m_flData = flValue;
	m_msgArgCount++;
}

void CUserMsgHookManager::WriteCoord(float flValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_Coord;
	m_msgArgs[m_msgArgCount].v.m_flData = flValue;
	m_msgArgCount++;
}

void CUserMsgHookManager::WriteString(const char *szValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_String;
	m_msgArgs[m_msgArgCount].m_string = szValue;
	m_msgArgCount++;
}

void CUserMsgHookManager::WriteEntity(int iValue)
{
	if (m_msgArgCount + 1 >= MAX_USERMSG_ARG_COUNT)
	{
		ClearMessage();
		return;
	}
	m_msgArgs[m_msgArgCount].m_iArgType = UserMsgArg_Entity;
	m_msgArgs[m_msgArgCount].v.m_iData = iValue;
	m_msgArgCount++;
}

bool CUserMsgHookManager::IsHooked(int msgType) const
{
	return m_Hooked && (msgType == m_msgType);
}

bool CUserMsgHookManager::IsHooked() const
{
	return m_Hooked;
}

bool CUserMsgHookManager::IsHookRegistered(int msgType) const
{
	return !m_UserMsgCallbacks[msgType].empty();
}

bool CUserMsgHookManager::IsHookEnabled(int msgType) const
{
	return m_msgHookEnabled[msgType];
}

bool CUserMsgHookManager::IsHookGlobalEnabled() const
{
	return m_msgHookGlobalEnabled;
}

void CUserMsgHookManager::Init()
{
	m_msgHookGlobalEnabled = true;

	for (int i = 0; i < 256; ++i)
	{
		m_msgHookEnabled[i] = false;
	}

	m_Hooked = false;
	m_Blocked = false;
	m_msgDest = 0;
	m_msgType = 0;
	m_msgOrigin = g_vecZero;
	m_msgEntity = NULL;
	m_msgArgCount = 0;
}

void CUserMsgHookManager::Shutdown()
{
	for (int msgType = 0; msgType < 256; ++msgType)
	{
		auto &callbacks = m_UserMsgCallbacks[msgType];
		for (auto itor = callbacks.begin(); itor != callbacks.end(); itor ++)
		{
			auto callback = *itor;

			ASEXT_DereferenceCASFunction(callback);
		}
		m_UserMsgCallbacks[msgType].clear();
		m_msgHookEnabled[msgType] = false;
	}
}

bool CUserMsgHookManager::RegisterUserMsgHook(int msgType, aslScriptFunction *aslfn)
{
	if (msgType < 0 || msgType > 255)
		return false;

	auto callback = ASEXT_CreateCASFunction(aslfn, ASEXT_GetServerManager()->curModule, 1);

	m_UserMsgCallbacks[msgType].emplace_back(callback);
	m_msgHookEnabled[msgType] = true;

	return true;
}

bool CUserMsgHookManager::EnableUserMsgHook(int msgType, bool bEnabled)
{
	if (IsHooked())
		return false;
		
	if (msgType < 0 || msgType > 255)
		return false;

	if (m_UserMsgCallbacks[msgType].empty())
		return false;

	m_msgHookEnabled[msgType] = bEnabled;

	return true;
}

bool CUserMsgHookManager::EnableUserMsgHookGlobal(bool bEnabled)
{
	m_msgHookGlobalEnabled = bEnabled;

	return true;
}

bool CUserMsgHookManager::BlockCurrentUserMsg()
{
	if (!m_Hooked)
		return false;

	m_Blocked = true;

	return true;
}

bool CUserMsgHookManager::CallUserMsgHook()
{
	return CallUserMsgHook(GetMsgDest(), GetMsgType(), GetMsgOrigin(), GetMsgEdict());
}

bool CUserMsgHookManager::CallUserMsgHook(int msgDest, int msgType, const Vector &origin, edict_t *pEdict)
{
	if (m_UserMsgCallbacks[msgType].empty())
		return false;

	for (auto &callback : m_UserMsgCallbacks[msgType])
	{
		if (ASEXT_CallCASBaseCallable && (*ASEXT_CallCASBaseCallable))
		{
			if ((*ASEXT_CallCASBaseCallable)(callback, 0, msgDest, msgType, &origin, pEdict))
			{
				return m_Blocked;
			}
		}
	}

	return false;
}