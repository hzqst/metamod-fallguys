# Fall Guys in Sven Co-op feature expansions

Most features are provided via angelscript interfaces.

## AngelScript interfaces

### Set Brush Entity as **Super Pusher**

```
g_EntityFuncs.SetEntitySuperPusher(self.edict(), true);
```

* Players and monsters will be pushed backward along with another players who is trying to block **Super Pusher**.

* `void Touch( CBaseEntity@ pOther )` will get called when **Super Pusher** impacts or hits any player or monster positively.

### Custom Footsteps

//You have to enable custom footsteps for current map.

//Custom footsteps will be automatically disabled after level changes.
```
void MapInit()
{
   //...
   g_Hooks.RegisterHook(Hooks::Player::PlayerMovePlaySoundFX, @PlayerMovePlaySoundFX);

	g_EngineFuncs.EnableCustomStepSound(true);
}
```

```

//Play whatever footstep sound you want here

//Use uiFlags |= 1 to block original sound;

HookReturnCode PlayerMovePlaySoundFX( CBasePlayer@ pPlayer, int playerindex, Vector origin, int type, const string& in sound, float vol, float att, int flags, int pitch, uint& out uiFlags )
{
	if(sound.StartsWith("player/pl_step"))
	{
		g_SoundSystem.EmitSoundDyn( pPlayer.edict(), CHAN_BODY, g_szFootStepPlasticSound[Math.RandomLong(0, 6)], 1.0, 1.0, 0, 90 + Math.RandomLong(0, 20) );

		uiFlags |= 1;
		return HOOK_HANDLED;
	}
	else if(sound.StartsWith("player/pl_wade") || sound.StartsWith("player/pl_slosh"))
	{
		g_SoundSystem.EmitSoundDyn( pPlayer.edict(), CHAN_BODY, g_szFootStepSlipperySound[Math.RandomLong(0, 6)], 1.0, 1.0, 0, 90 + Math.RandomLong(0, 20) );

		uiFlags |= 1;
		return HOOK_HANDLED;
	}
	else if(sound.StartsWith("player/pl_jump"))
	{
		g_SoundSystem.EmitSoundDyn( pPlayer.edict(), CHAN_BODY, g_szJumpPlasticSound[Math.RandomLong(0, 6)], 1.0, 1.0, 0, 90 + Math.RandomLong(0, 20) );

		uiFlags |= 1;
		return HOOK_HANDLED;
	}
	else if(sound == "player/pl_fallpain3.wav")
	{
		g_SoundSystem.EmitSoundDyn( pPlayer.edict(), CHAN_BODY, g_szImpactPlasticSound[Math.RandomLong(0, 2)], 1.0, 1.0, 0, 90 + Math.RandomLong(0, 20) );

		uiFlags |= 1;
		return HOOK_HANDLED;
	}

    return HOOK_CONTINUE;
}
```

### Server-Side Level of Detail

```

//Constant

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

### SemiClip

* semiclip: The collision or phys-interaction (including firebullet, traceline) between EntityA and EntityB will be completely disabled.

* PlayerMove-only-semiclip: Only player movement will be affected. traceline will not be affected.

```

//pEntity is semiclip to "pPlayer" and stays clipping to any other players. their bullet / traceline will just go through each other.

g_EntityFuncs.SetEntitySemiClip(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) );

//pEntity is PlayerMove-only-semiclip to "pPlayer" and stays clipping to any other players. their bullet / traceline / phys-interaction will not be affected.

g_EntityFuncs.SetEntityPMSemiClip(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) );

```

```

//pEntity is semiclip to "pPlayer" and "pPlayer2". their bullet / traceline will just go through each other.

g_EntityFuncs.SetEntitySemiClip(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) |  (1 << (pPlayer2.entindex() - 1)) );

//pEntity is semiclip to "pPlayer" and "pPlayer2". their bullet / traceline / phys-interaction will not be affected.

g_EntityFuncs.SetEntityPMSemiClip(pEntity.edict(), (1 << (pPlayer.entindex() - 1)) |  (1 << (pPlayer2.entindex() - 1)) );

```

```

//Use 0 to completely deactivated semiclip

g_EntityFuncs.SetEntitySemiClip(pEntity.edict(), 0 );

//Use 0 to completely deactivated PlayerMove-only-semiclip

g_EntityFuncs.SetEntityPMSemiClip(pEntity.edict(), 0 );

```

```

//Use -1 to activated semiclip between pEntity and all 32 players on server.

g_EntityFuncs.SetEntitySemiClip(pEntity.edict(), -1 );

//Use -1 to activated PlayerMove-only-semiclip between pEntity and all 32 players on server.

g_EntityFuncs.SetEntityPMSemiClip(pEntity.edict(), -1 );

```

```

//Easier-to-use API

//pEntity is semiclip to "pPlayer". their bullet / traceline / hitscan will just go through each other.

g_EntityFuncs.SetEntitySemiClipToPlayer(pEntity.edict(), pPlayer.entindex() );// the second arg must be a player index.
g_EntityFuncs.SetEntitySemiClipToEntityIndex(pEntity.edict(), pPlayer.entindex() );//same as SetEntitySemiClipToPlayer, the second arg can be non-player entity index.
g_EntityFuncs.SetEntitySemiClipToEntity(pEntity.edict(), pPlayer.edict() );//same as SetEntitySemiClipToPlayer, the second arg can be non-player entity.

//pEntity is PlayerMove-only-semiclip to "pPlayer". their bullet / traceline / hitscan interaction will not be affected.

g_EntityFuncs.SetEntityPMSemiClipToPlayer(pEntity.edict(), pPlayer.entindex() );// the second arg must be a player index.
g_EntityFuncs.SetEntityPMSemiClipToEntityIndex(pEntity.edict(), pPlayer.entindex() );//same as SetEntityPMSemiClipToPlayer, the second arg can be non-player entity index.
g_EntityFuncs.SetEntityPMSemiClipToEntity(pEntity.edict(), pPlayer.edict() );//same as SetEntityPMSemiClipToPlayer, the second arg can be non-player entity.

//Deactivate semiclip between "pEntity" and "pPlayer"
g_EntityFuncs.UnsetEntitySemiClipToEntity(pEntity.edict(), pPlayer.edict());

//Deactivate PlayerMove-only-semiclip between "pEntity" and "pPlayer"
g_EntityFuncs.UnsetEntityPMSemiClipToEntity(pEntity.edict(), pPlayer.edict());

//Deactivate semiclip between "pEntity" and any other entity. (if was semiclip before).
g_EntityFuncs.UnsetEntitySemiClipToAll(pEntity.edict());

//Deactivate PlayerMove-only-semiclip between "pEntity" and any other entity. (if was semiclip before).
g_EntityFuncs.UnsetEntityPMSemiClipToAll(pEntity.edict());

```

### Create a physic object

Physic objects run physic simulation (gravity, movement, collision) in Bullet Engine instead of GoldSrc hull clipping.

```

//You have to enable physic world for current map first before using any feature from Bullet Engine.

//PhysicWorld will be automatically disabled after level changes

void MapInit()
{

//...
g_EngineFuncs.EnablePhysicWorld(true);

}

```

The following code creates a physic box with size of (32 x 32 x 32) units

```

//Constant

const int PhysicShapeDirection_X = 0;
const int PhysicShapeDirection_Y = 1;
const int PhysicShapeDirection_Z = 2;

const int PhysicShape_Box = 1;
const int PhysicShape_Sphere = 2;
const int PhysicShape_Capsule = 3;
const int PhysicShape_Cylinder = 4;
const int PhysicShape_MultiSphere = 5;

const int PhysicObject_HasClippingHull = 1;
const int PhysicObject_HasImpactImpulse = 2;
const int PhysicObject_Freeze = 4;

```

```

//pEntity will collide with players
pEntity.pev.solid = SOLID_BBOX;

//or

//pEntity neither collides with players nor other physic objects. but still collides with world.
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

### Entity follow (similar to trigger_setorigin but save entity count and reduce potential latency)

```

//Constant

const int FollowEnt_CopyOriginX = 1;
const int FollowEnt_CopyOriginY = 2;
const int FollowEnt_CopyOriginZ = 4;
const int FollowEnt_CopyAnglesP = 8;
const int FollowEnt_CopyAnglesY = 0x10;
const int FollowEnt_CopyAnglesR = 0x20;
const int FollowEnt_CopyOrigin = (FollowEnt_CopyOriginX | FollowEnt_CopyOriginY | FollowEnt_CopyOriginZ);
const int FollowEnt_CopyAngles = (FollowEnt_CopyAnglesP | FollowEnt_CopyAnglesY | FollowEnt_CopyAnglesR);
const int FollowEnt_CopyNoDraw = 0x40;
const int FollowEnt_CopyRenderMode = 0x80;
const int FollowEnt_CopyRenderAmt = 0x100;
const int FollowEnt_ApplyLinearVelocity = 0x200;
const int FollowEnt_ApplyAngularVelocity = 0x400;

```

```

Vector vecOriginOffset = Vector(0, 0, 0);
Vector vecAnglesOffset = Vector(0, 0, 0);

int flags = FollowEnt_CopyOrigin | FollowEnt_CopyAngles;

//pEntity's pev.origin and pev.angles will be copy-pasted from pCopyFromEntity
g_EntityFuncs.SetEntityFollow(pEntity.edict(), pCopyFromEntity.edict(), flags, vecOriginOffset, vecAnglesOffset);

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

### Get some important information from sound file

```
//From FMOD.h header file.

/*
[ENUM]
[
    [DESCRIPTION]   
    These definitions describe the type of song being played.

    [REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation Portable, PlayStation 3, Wii, iPhone, 3GS, NGP, Android

    [SEE_ALSO]      
    Sound::getFormat
]
*/
typedef enum
{
    FMOD_SOUND_TYPE_UNKNOWN,         /* 3rd party / unknown plugin format. */
    FMOD_SOUND_TYPE_AIFF,            /* AIFF. */
    FMOD_SOUND_TYPE_ASF,             /* Microsoft Advanced Systems Format (ie WMA/ASF/WMV). */
    FMOD_SOUND_TYPE_AT3,             /* Sony ATRAC 3 format */
    FMOD_SOUND_TYPE_CDDA,            /* Digital CD audio. */
    FMOD_SOUND_TYPE_DLS,             /* Sound font / downloadable sound bank. */
    FMOD_SOUND_TYPE_FLAC,            /* FLAC lossless codec. */
    FMOD_SOUND_TYPE_FSB,             /* FMOD Sample Bank. */
    FMOD_SOUND_TYPE_GCADPCM,         /* Nintendo GameCube/Wii ADPCM */
    FMOD_SOUND_TYPE_IT,              /* Impulse Tracker. */
    FMOD_SOUND_TYPE_MIDI,            /* MIDI. extracodecdata is a pointer to an FMOD_MIDI_EXTRACODECDATA structure. */
    FMOD_SOUND_TYPE_MOD,             /* Protracker / Fasttracker MOD. */
    FMOD_SOUND_TYPE_MPEG,            /* MP2/MP3 MPEG. */
    FMOD_SOUND_TYPE_OGGVORBIS,       /* Ogg vorbis. */
    FMOD_SOUND_TYPE_PLAYLIST,        /* Information only from ASX/PLS/M3U/WAX playlists */
    FMOD_SOUND_TYPE_RAW,             /* Raw PCM data. */
    FMOD_SOUND_TYPE_S3M,             /* ScreamTracker 3. */
    FMOD_SOUND_TYPE_SF2,             /* Sound font 2 format. */
    FMOD_SOUND_TYPE_USER,            /* User created sound. */
    FMOD_SOUND_TYPE_WAV,             /* Microsoft WAV. */
    FMOD_SOUND_TYPE_XM,              /* FastTracker 2 XM. */
    FMOD_SOUND_TYPE_XMA,             /* Xbox360 XMA */
    FMOD_SOUND_TYPE_VAG,             /* PlayStation Portable ADPCM VAG format. */
    FMOD_SOUND_TYPE_AUDIOQUEUE,      /* iPhone hardware decoder, supports AAC, ALAC and MP3. extracodecdata is a pointer to an FMOD_AUDIOQUEUE_EXTRACODECDATA structure. */
    FMOD_SOUND_TYPE_XWMA,            /* Xbox360 XWMA */
    FMOD_SOUND_TYPE_BCWAV,           /* 3DS BCWAV container format for DSP ADPCM and PCM */
    FMOD_SOUND_TYPE_AT9,             /* NGP ATRAC 9 format */
    FMOD_SOUND_TYPE_VORBIS,          /* Raw vorbis */
    FMOD_SOUND_TYPE_MEDIA_FOUNDATION,/* Microsoft Media Foundation wrappers, supports ASF/WMA */

    FMOD_SOUND_TYPE_MAX,             /* Maximum number of sound types supported. */
    FMOD_SOUND_TYPE_FORCEINT = 65536 /* Makes sure this enum is signed 32bit. */
} FMOD_SOUND_TYPE;


/*
[ENUM]
[
    [DESCRIPTION]   
    These definitions describe the native format of the hardware or software buffer that will be used.

    [REMARKS]
    This is the format the native hardware or software buffer will be or is created in.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation Portable, PlayStation 3, Wii, iPhone, 3GS, NGP, Android

    [SEE_ALSO]
    System::createSound
    Sound::getFormat
]
*/
typedef enum
{
    FMOD_SOUND_FORMAT_NONE,             /* Unitialized / unknown. */
    FMOD_SOUND_FORMAT_PCM8,             /* 8bit integer PCM data. */
    FMOD_SOUND_FORMAT_PCM16,            /* 16bit integer PCM data. */
    FMOD_SOUND_FORMAT_PCM24,            /* 24bit integer PCM data. */
    FMOD_SOUND_FORMAT_PCM32,            /* 32bit integer PCM data. */
    FMOD_SOUND_FORMAT_PCMFLOAT,         /* 32bit floating point PCM data. */
    FMOD_SOUND_FORMAT_GCADPCM,          /* Compressed Nintendo 3DS/Wii DSP data. */
    FMOD_SOUND_FORMAT_IMAADPCM,         /* Compressed IMA ADPCM data. */
    FMOD_SOUND_FORMAT_VAG,              /* Compressed PlayStation Portable ADPCM data. */
    FMOD_SOUND_FORMAT_HEVAG,            /* Compressed PSVita ADPCM data. */
    FMOD_SOUND_FORMAT_XMA,              /* Compressed Xbox360 XMA data. */
    FMOD_SOUND_FORMAT_MPEG,             /* Compressed MPEG layer 2 or 3 data. */
    FMOD_SOUND_FORMAT_CELT,             /* Compressed CELT data. */
    FMOD_SOUND_FORMAT_AT9,              /* Compressed PSVita ATRAC9 data. */
    FMOD_SOUND_FORMAT_XWMA,             /* Compressed Xbox360 xWMA data. */
    FMOD_SOUND_FORMAT_VORBIS,           /* Compressed Vorbis data. */

    FMOD_SOUND_FORMAT_MAX,              /* Maximum number of sound formats supported. */   
    FMOD_SOUND_FORMAT_FORCEINT = 65536  /* Makes sure this enum is signed 32bit. */
} FMOD_SOUND_FORMAT;
```

```

//bool GetSoundInfo(const string& in szSoundName, SoundEngine_SoundInfo & out SoundInfo)

SoundEngine_SoundInfo info;

if(g_SoundEngine.GetSoundInfo("weapons/357_shot1.wav", info))
{
	g_Game.AlertMessage( at_console, "Sound %1, type=%2, format=%3, channels=%4, bits=%5, length=%6\n", "weapons/357_shot1.wav", info.type, info.format, info.channels, info.bits, info.length);

	//The units of info.length is Milliseconds

}

```
