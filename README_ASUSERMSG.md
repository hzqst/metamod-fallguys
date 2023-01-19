# Hook UserMsg in AngelScript (aka asusermsg)

* Provide ability of blocking or intercepting UserMsg in Sven Co-op AngelScript engine

## Register a usermsg hook, block the old message, and resend with your own message

```

const int UserMsgArg_Byte = 1;
const int UserMsgArg_Char = 2;
const int UserMsgArg_Short = 3;
const int UserMsgArg_Long = 4;
const int UserMsgArg_Angle = 5;
const int UserMsgArg_Coord = 6;
const int UserMsgArg_String = 7;
const int UserMsgArg_Entity = 8;

int gTextMsg = 0;

void MapInit()
{
	//g_EngineFuncs.GetUserMsgId("TextMsg"); does't work in MapInit as usermsgs are not registered yet by server dll
	gTextMsg = 75;
	g_EngineFuncs.RegisterUserMsgHook(gTextMsg, function(int msgDest, int msgType, Vector origin, edict_t @pEdict){

		if(g_EngineFuncs.GetUserMsgArgCount() >= 2 && g_EngineFuncs.GetUserMsgArgType(0) == UserMsgArg_Byte && g_EngineFuncs.GetUserMsgArgType(1) == UserMsgArg_String)
		{
			string str;
			g_EngineFuncs.GetUserMsgArgString(1, str);
			if(g_EngineFuncs.GetUserMsgArgInteger(0) == 4 && str == "Electric crowbar is only for donors.\n")
			{
				//Temporarily disable the hook to avoid recursive call
				g_EngineFuncs.EnableUserMsgHookGlobal(false);

				//Resend message
				NetworkMessage m( MSG_ONE, NetworkMessages::NetworkMessageType(msgType), pEdict );
					m.WriteByte(4);
					m.WriteString("Crown is only for victories.\n");
				m.End();
				
				g_EngineFuncs.EnableUserMsgHookGlobal(true);

				//Block original message
				g_EngineFuncs.BlockCurrentUserMsg();
			}
		}
		return 1919810;

	});

}
```