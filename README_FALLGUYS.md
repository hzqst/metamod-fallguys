# Fall Guys in Sven Co-op feature expansions

Most features are provided via angelscript interfaces.

### Better Player vs Brush Entities Interaction

1. Players and monsters will be pushed backward along with another players who is trying to block **Super Pusher**.

2. `void Touch( CBaseEntity@ pOther )` will get called when **Super Pusher** impacts or hits any player or monster positively.

### Introduce physic objects that calculate their movements in Bullet Engine instead of GoldSrc's Hull Clipping.

## AngelScript interfaces

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

//modelindex is calculated at runtime for each players separately.
//scale is calculated at runtime for each players separately.

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

//body is calculated at runtime for each players separately.

```

###

### Semi Visible

//Entity set as Semi Visible can only be seen by specified player(s)

```

//pEntity is visible to "pPlayer" and invisbie to any other players.

g_EntityFuncs.SetEntitySemiVisible(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) );

```

```

//pEntity is visible to "pPlayer" and "pPlayer2" and invisbie to any other players.

g_EntityFuncs.SetEntitySemiVisible(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) |  (1 << (pPlayer2.entindex() - 1)) );

```

```

//Set second arg to 0 to turn Semi Visible off

g_EntityFuncs.SetEntitySemiVisible(pEntity.edict(), 0 );

```

### Create a physic object

Physic objects calculates their simulation (gravity, movement, collision) in Bullet Engine instead of GoldSrc hull clipping.

The following demo creates a physic box with size of (32 x 32 x 32) units

```

//pEntity will collide with players
pEntity.pev.solid = SOLID_BBOX;

//or

//pEntity neither collides with players nor other physic objects.
pEntity.pev.solid = SOLID_NOT;

//Must be MOVETYPE_NOCLIP, otherwise client interpolation will not work
pEntity.pev.movetype = MOVETYPE_NOCLIP;

//Initialize a PhysicShapeParams that will be passed to CreatePhysicObject
PhysicShapeParams shapeParams;
shapeParams.type = PhysicShape_Box;
shapeParams.size = Vector(32, 32, 32);

//Initialize a PhysicObjectParams that will be passed to CreatePhysicObject
PhysicObjectParams objectParams;
objectParams.mass = m_flMass;
objectParams.linearfriction = 1;
objectParams.rollingfriction = 1;
objectParams.restitution = 0.5;
objectParams.ccdradius = 0;
objectParams.ccdthreshold = 0;

//Call g_EntityFuncs.CreatePhysicObject to create a physic object. (this must be done before setting Level of Detail).
g_EntityFuncs.CreatePhysicObject(pEntity.edict(), shapeParams, objectParams);

```

* You should add `$flags 512` in the `.qc` or check `HLAM -> Model Flags -> Hitbox Collision` for this studiomodel to force engine to use hitbox as collision shape instead of axis-locked box in playermove simulation.

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

//Do not use, this dramatically hurts your performance.

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
//You can override player's pev.sequence or whatever you want which is updated on every frame in PlayerPostThink
HookReturnCode PlayerPostThinkPost(CBasePlayer@ pPlayer)
{
   return HOOK_CONTINUE;
}
```

### Hook PlayerTouchTrigger

The PlayerTouchTrigger get called when player touches a trigger entity (pev.solid == SOLID_TRIGGER) positively.

pPlayer.pev.velocity will be set to impact velocity temporarily in the hook handler.

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

### Hook PlayerTouchPlayer

The PlayerTouchPlayer get called when player touches or impacts a solid player positively.

pPlayer.pev.velocity will be set to impact velocity temporarily in the hook handler.

Any changes to pPlayer.pev.velocity will be dropped and ignored.

```
g_Hooks.RegisterHook(Hooks::Player::PlayerTouchPlayer, @PlayerTouchPlayer);

```

```

HookReturnCode PlayerTouchPlayer( CBasePlayer@ pPlayer, CBasePlayer@ pOther )
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

edict_t@viewent = g_EngineFuncs.GetViewEntity(pPlayer.edict());
```
