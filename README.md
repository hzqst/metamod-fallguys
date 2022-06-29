# metamod-fallguys

This is a metamod plugin for [Fall Guys in Sven Co-op](https://github.com/hzqst/sven-fallguys) as feature expansion.

# What it does ?

## Better Player vs Brush Entities Interaction

1. Brush entity with `pev.movetype = MOVETYPE_PUSH` , `pev.solid = SOLID_BSP` and `pev.sequence == 114514` is **Super Pusher**.

2. Point entity with `(pev.solid == SOLID_SLIDEBOX or pev.solid == SOLID_BBOX)` and `(pev.movetype == MOVETYPE_STEP or pev.movetype == MOVETYPE_WALK)` is **Pushee**

3. **Pushee** will be pushed backward along with **another Pushee who is trying to block it** instead of getting blocked by **Super Pusher**.

4. `void Touch( CBaseEntity@ pOther )` will get called when **Super Pusher** impacts or hits any **Pushee** positively. `pev.sequence` of **Super Pusher** will be set to `1919810` temporarily in this scenario.

## AngelScript interface expansion

### Hook AddToFullPack

```
g_Hooks.RegisterHook(Hooks::Player::PlayerAddToFullPack, @PlayerAddToFullPack);// register at initialization
```

```
HookReturnCode PlayerAddToFullPack( entity_state_t@ state, int e, edict_t @ent, edict_t@ host, int hostflags, int player, uint& out uiFlags )
{
   //if uiFlags is set to 1, the entity will not be transmitted to client represented by edict_t@ host.
    return HOOK_HANDLED;
}
```

### Hook post call of PlayerPostThink (aka PlayerPostThink_Post)

```
g_Hooks.RegisterHook(Hooks::Player::PlayerPostThinkPost, @PlayerPostThinkPost);// register at initialization
```

```
//You can override player's pev.sequence or some other shit which is updated on every frame in PlayerPostThink
HookReturnCode PlayerPostThinkPost(CBasePlayer@ pPlayer)
{
   return HOOK_CONTINUE;
}
```

### Hook PlayerTouchTrigger

The PlayerTouchTrigger get called when player touches a trigger entity (pev.solid == SOLID_TRIGGER) positively.

pPlayer.pev.velocity will be set to impactvelocity temporarily in this scenario.

Any changes to pPlayer.pev.velocity will be dropped and ignored.

```
g_Hooks.RegisterHook(Hooks::Player::PlayerTouchTrigger, @PlayerTouchTrigger);

```

```

HookReturnCode PlayerTouchTrigger( CBasePlayer@ pPlayer, CBaseEntity@ pOther )
{
    return HOOK_CONTINUE;
}
```

### Hook PlayerTouchImpact

The PlayerTouchImpact get called when player touches a solid entity (pev.solid == SOLID_BBOX or pev.solid == SOLID_SLIDEBOX or pev.solid == SOLID_BSP) positively.

pPlayer.pev.velocity will be set to impactvelocity temporarily in this scenario.

Any changes to pPlayer.pev.velocity will be dropped and ignored.

```
g_Hooks.RegisterHook(Hooks::Player::PlayerTouchImpact, @PlayerTouchImpact);

```

```

HookReturnCode PlayerTouchImpact( CBasePlayer@ pPlayer, CBaseEntity@ pOther )
{
    return HOOK_CONTINUE;
}
```

### Get player's view entity (e.g trigger_camera)

```
//edict_t@ GetViewEntity(edict_t@ pClient)
//Warning: pClient must be a valid client edict

edict_t@viewent = g_EngineFuncs.GetViewEntity(pPlayer.edict());
```

### You are welcome to request for any new hook which is not implemented in Sven Co-op yet.

# Installation

1. Copy everything from `build` directory into `\steamapps\common\Sven Co-op\Sven Co-op\svencoop` *(Warning: `svencoop_addon` and `svencoop_downloads` are not supported)*

# Build Requirements (Windows)

1. [Visual Studio 2017 / 2019 / 2022, with vc141 / vc142 / vc143 toolset](https://visualstudio.microsoft.com/)

2. [CMake](https://cmake.org/download/)

3. [Git for Windows](https://gitforwindows.org/)

# Build Instruction (Windows)

1. `git clone https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys`

2. Run `build-win32-all.bat`

3. Be patient, `metamod.dll` and `fallguys.dll` will be generated at `build/addons/metamod/dlls` if no error(s) occours.

# Build Requirements (Linux)

1. GNU C++ Build environment. Install with `sudo apt-get install make build-essential gcc gcc-multilib g++-multilib` if missing.

2. CMake. Install with `suto apt-get install cmake` if missing.

3. Git client. Install with `suto apt-get install git` if missing.

# Build Instruction (Linux)

1. `git clone https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys`

2. `sudo chmod +777 build-all-linux.sh` to make it executable, then `./build-all-linux.sh`

3. Be patient, `metamod.so` and `fallguys.so` will be generated at `build/addons/metamod/dlls` if no error(s) occours.