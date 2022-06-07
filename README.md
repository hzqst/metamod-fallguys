# metamod-fallguys

This is a metamod plugin for Fall Guys maps in Sven Co-op 5.25+ (Steam version) (Windows or Linux)

# What it does?

1. You will be pushed backward along with *the player who is trying to block you* instead of getting blocked when being pushed by brush entity with `pev.sequence == 114514`

2. You will receive `void Touch( CBaseEntity@ pOther )` call when being pushed by brush entity with `pev.sequence == 114514`, and when it's a push event, `pev.sequence` will be changed to `1919810` temporarily.

3. You can hook AddToFullPack by using following code : 

```
g_Hooks.RegisterHook(Hooks::Player::PlayerAddToFullPack, @PlayerAddToFullPack);// register at initialization
```

```
HookReturnCode PlayerAddToFullPack( entity_state_t@ state, int e, edict_t @ent, edict_t@ host, int hostflags, int player, uint& out uiFlags )
{
   //if uiFlags is set to 1, the entity will not be transmitted to client presented by edict_t@ host.
    return HOOK_HANDLED;
}
```

# Installation

1. Copy everything from `build` directory into `\steamapps\common\Sven Co-op\Sven Co-op\svencoop` *(Warning: `svencoop_addon` and `svencoop_downloads` are not supported yet)*

# Build Instruction (Windows)

1. Youl should have Visual Studio 2017 or 2019 with vc141 or vc142 toolset installed.

2. run `build-all-win32.bat`

3. `metamod.dll` and `fallguys.dll` should be generated at `build/addons/metamod/dlls` if no error(s) occours.

# Build Instruction (Linux)

1. Youl should have GNU C++ Build environment installed. Install with `sudo apt-get install make build-essential gcc gcc-multilib g++-multilib` if something is missing.

2. `make OPT=opt install` or `./build-all-linux.sh` (Remember to give execution privilege if permission denied)

3. `metamod.so` and `fallguys.so` should be generated at `build/addons/metamod/dlls` if no error(s) occours.
