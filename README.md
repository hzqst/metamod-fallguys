# metamod-fallguys

This is a metamod plugin for [Fall Guys in Sven Co-op](https://github.com/hzqst/sven-fallguys) as feature expansion.

# What it does ?

## Better Player vs Brush Entities Interaction

1. Players and monsters will be pushed backward along with another players who is trying to block **Super Pusher**.

2. `void Touch( CBaseEntity@ pOther )` will get called when **Super Pusher** impacts or hits any player or monster positively.

## AngelScript interface expansion

### Set Brush Entity as **Super Pusher**

```
g_EntityFuncs.SetEntitySuperPusher(self.edict(), true);
```

### Server-Side Level of Detail

```

const int LOD_BODY = 1;
const int LOD_MODELINDEX = 2;
const int LOD_SCALE = 4;
const int LOD_SCALE_INTERP = 8;

```

```

g_EntityFuncs.SetEntityLevelOfDetail(pEntity.edict(),
		LOD_MODELINDEX | LOD_SCALE_INTERP, //modelindex LoD
		g_iPlayerArrowSprite1ModelIndex, 0.15,      //LoD 0
		g_iPlayerArrowSprite2ModelIndex, 0.15, 300, //Lod 1
		g_iPlayerArrowSprite3ModelIndex, 0.75, 700, //Lod 2
		g_iPlayerArrowSprite4ModelIndex, 0.75, 1000 //Lod 3
	);
   
//pEntity 's modelindex will be changed to g_iPlayerArrowSprite1ModelIndex when it's distance to player ranges from 0 to 300 units
//pEntity 's modelindex will be changed to g_iPlayerArrowSprite2ModelIndex when it's distance to player ranges from 300 to 700 units
//pEntity 's modelindex will be changed to g_iPlayerArrowSprite3ModelIndex when it's distance to player ranges from 700 to 1000 units
//pEntity 's modelindex will be changed to g_iPlayerArrowSprite3ModelIndex when it's distance to player ranges from 1000 to +inf units

//pEntity 's scale will be changed to 0.15 when it's distance to player ranges from 0 to 300 units
//pEntity 's scale will be changed to 0.15 + (0.75 - 0.15) * (distance - 300) / (700 - 300) when it's distance to player ranges from 300 to 700 units
//pEntity 's scale will be changed to 0.75 when it's distance to player ranges from 700 to 1000 units
//pEntity 's scale will be changed to 0.75 when it's distance to player ranges from 1000 to +inf units

modelindex and scale are calculated at runtime for each players separately.

```

```
			g_EntityFuncs.SetEntityLevelOfDetail(pEntity.edict(), 
				LOD_BODY,
				0, 0.0, //LoD 0
				pEntity.pev.iuser1, 0, pEntity.pev.fuser1, //LoD 1
				pEntity.pev.iuser2, 0, pEntity.pev.fuser2, //LoD 2
				pEntity.pev.iuser3, 0, pEntity.pev.fuser3 //LoD 3
			);
         
//pEntity 's body will be changed to 0 when it's distance to player ranges from 0 to fuser1 units
//pEntity 's body will be changed to pEntity.pev.iuser1 when it's distance to player ranges from pEntity.pev.fuser1 to pEntity.pev.fuser2 units
//pEntity 's body will be changed to pEntity.pev.iuser2 when it's distance to player ranges from pEntity.pev.fuser2 to pEntity.pev.fuser3 units
//pEntity 's body will be changed to pEntity.pev.iuser3 when it's distance to player ranges from pEntity.pev.fuser3 to +inf units

```

###

### Partial Viewer

```
//Entity set as Partial Viewer can only be seen by pPlayer

g_EntityFuncs.SetEntityPartialViewer(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) );

//Entity set as Partial Viewer can only be seen by pPlayer and pPlayer2

g_EntityFuncs.SetEntityPartialViewer(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) |  (1 << (pPlayer2.entindex() - 1)) );

//Set second arg to 0 to turn Partial Viewer off for pEntity

g_EntityFuncs.SetEntityPartialViewer(pEntity.edict(), 0 );

```

### Create PhysicBox

PhysicBox does it's physic simulation (gravity, movement, collision) in Bullet Engine instead of GoldSrc hull clipping.


```

//pEntity will collide with players
pEntity.pev.solid = SOLID_BBOX;

//or

//pEntity will not collide with players
pEntity.pev.solid = SOLID_NOT;

//Must be noclip, otherwise client interpolation will be disable.
pEntity.pev.movetype = MOVETYPE_NOCLIP;

//Must be called before setting LevelOfDetail
g_EntityFuncs.CreatePhysicBox(pEntity.edict(),
			m_flMass,
			m_flLinearFriction,
			m_flRollingFriction,
			m_flRestitution,
			m_flCCDRadius,
			m_flCCDThreshold,
			bIsPushable);

//The box will be pushable to players if bIsPushable is true
//The half extent of box is (pEntity.pev.mins + pEntity.pev.maxs) * 0.5
//The studiomodel of this box should have modelflags bit 512 set (Hitbox Collision in HLAM -> Model Flags) otherwise the collision with players will be glitchy.
```

### Detect who is currently running player move code

```
void Touch( CBaseEntity@ pOther )
{
   int playerIndex = g_EngineFuncs.GetRunPlayerMovePlayerIndex();
      if(playerIndex == pOther.entindex())
      {
         //pOther is running player move code and trying to impact me (stepping towards me)
         //pOther is basically stopped moving because of hitting me and the impact velocity is pOther.pev.velocity
         Vector vecImpactVelocity = pOther.pev.velocity;
         
      }
   }
}
```

### Hook AddToFullPack

//Not recommended to use, this can slow down your server's framerate.

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

The PlayerTouchImpact get called when player touches or impacts a solid entity (pev.solid == SOLID_BBOX or pev.solid == SOLID_SLIDEBOX or pev.solid == SOLID_BSP) positively.

pPlayer.pev.velocity will be set to impact velocity temporarily in the hook handler.

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
