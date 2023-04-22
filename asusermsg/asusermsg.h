#pragma once

#include <string>
#include <vector>

const int UserMsgArg_Byte = 1;
const int UserMsgArg_Char = 2;
const int UserMsgArg_Short = 3;
const int UserMsgArg_Long = 4;
const int UserMsgArg_Angle = 5;
const int UserMsgArg_Coord = 6;
const int UserMsgArg_String = 7;
const int UserMsgArg_Entity = 8;

class CUserMsgArg
{
public:
	CUserMsgArg()
	{
		m_iArgType = 0;
		v.m_iData = 0;
	}

	int m_iArgType;
	union
	{
		float m_flData;
		int m_iData;
	}v;
	std::string m_string;
};

#define MAX_USERMSG_ARG_COUNT 256

class CUserMsgHookManager
{
public:
	CUserMsgHookManager()
	{
		Init();
	}
	bool RegisterUserMsgHook(int msgType, aslScriptFunction *aslfn);

	bool EnableUserMsgHook(int msgType, bool bEnabled);

	bool EnableUserMsgHookGlobal(bool bEnabled);

	bool BlockCurrentUserMsg();

	bool CallUserMsgHook();

	bool CallUserMsgHook(int msgDest, int msgType, const Vector &origin, edict_t *pEdict);

	bool IsHooked() const;

	bool IsHooked(int msgType) const;

	bool IsHookRegistered(int msgType) const;

	bool IsHookEnabled(int msgType) const;

	bool IsHookGlobalEnabled() const;

	void Init();

	void Shutdown();

	int GetUserMsgId(const char *name);
	void MessageBegin(int msgDest, int msgType, const float *origin, edict_t *pEdict);
	void ClearMessage();
	void SendBufferedMessage();
	void WriteByte(int iValue);
	void WriteChar(int iValue);
	void WriteShort(int iValue);
	void WriteLong(int iValue);
	void WriteAngle(float flValue);
	void WriteCoord(float flValue);
	void WriteString(const char *szValue);
	void WriteEntity(int iValue);

	int GetMsgDest() const
	{
		return m_msgDest;
	}

	int GetMsgType() const
	{
		return m_msgType;
	}
	edict_t * GetMsgEdict() const
	{
		return m_msgEntity;
	}
	Vector GetMsgOrigin() const
	{
		return m_msgOrigin;
	}
	int GetArgCount() const
	{
		return m_msgArgCount;
	}
	int GetArgType(int index) const
	{
		return m_msgArgs[index].m_iArgType;
	}
	int GetArgInteger(int index) const
	{
		return m_msgArgs[index].v.m_iData;
	}
	float GetArgFloat(int index) const
	{
		return m_msgArgs[index].v.m_flData;
	}
	const char* GetArgString(int index) const
	{
		return m_msgArgs[index].m_string.c_str();
	}

	void GetArgString(int index, CString *str) const
	{
		str->assign(m_msgArgs[index].m_string.c_str(), m_msgArgs[index].m_string.length());
	}
private:
	bool m_msgHookGlobalEnabled;
	bool m_Hooked;
	bool m_Blocked;

	int m_msgDest;
	int m_msgType;
	Vector m_msgOrigin;
	edict_t *m_msgEntity;

	bool m_msgHookEnabled[MAX_USERMSG_ARG_COUNT];
	CUserMsgArg m_msgArgs[MAX_USERMSG_ARG_COUNT];
	int m_msgArgCount;

	std::vector<CASFunction *> m_UserMsgCallbacks[256];
};

void RegisterAngelScriptMethods();

extern CUserMsgHookManager g_UserMsgHookManager;