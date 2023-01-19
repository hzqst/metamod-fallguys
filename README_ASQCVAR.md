# QueryCvar in AngelScript (aka asqcvar)

* Exposes "server.NewDllFunction.pfnCvarValue" and "server.NewDllFunction.pfnCvarValue2" to AngelScript engine

* Being able to read response of network message "svc_sendcvarvalue" and "svc_sendcvarvalue2" from client.

## Send a cvar query requset to client

```
	//NetworkMessages::NetworkMessageType(58) means "svc_sendcvarvalue2"

	NetworkMessage m( MSG_ONE, NetworkMessages::NetworkMessageType(58), pPlayer.edict() );
		m.WriteLong(114514);//RequestId
		m.WriteString("default_fov");
	m.End();
```

## Register an asynchronous callback for cvar query response

```
	g_EngineFuncs.SetQueryCvar2Callback(1, function(CBasePlayer@ pPlayer, int requestId, string cvarName, string value){
		
		g_Game.AlertMessage(at_aiconsole,  "%1's %2 is %3\n", pPlayer.pev.netname, cvarName, value );

	});
```

## Register global hooks

```

HookReturnCode PlayerQueryCvar(CBasePlayer @pPlayer, const string &in value)
{
	g_Game.AlertMessage(at_aiconsole,  "%1's cvar is %2\n", pPlayer.pev.netname, value );

    return HOOK_CONTINUE;
}

HookReturnCode PlayerQueryCvar2(CBasePlayer @pPlayer, int requestId, const string &in cvarName, const string &in value)
{
	g_Game.AlertMessage(at_aiconsole,  "%1's %2 is %3\n", pPlayer.pev.netname, cvarName, value );

    return HOOK_CONTINUE;
}

void MapInit()
{
    g_Hooks.RegisterHook(Hooks::Player::QueryCvar, @PlayerQueryCvar);
    g_Hooks.RegisterHook(Hooks::Player::QueryCvar2, @PlayerQueryCvar2);
}
```