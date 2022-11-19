# metamod-fallguys

This is a group of metamod plugins for [Fall Guys in Sven Co-op](https://github.com/hzqst/sven-fallguys) as feature expansions.

The metamod core is based on [metamod-p](https://github.com/Bots-United/metamod-p)

# Installation

1. Download from [GitHub Release](https://github.com/hzqst/metamod-fallguys/releases), then unzip it.

1. Copy everything from `build` directory into `\steamapps\common\Sven Co-op\svencoop` *(Warning: `svencoop_addon` and `svencoop_downloads` are not supported)*

It should be something like :
```
-- Sven Co-op (or Sven Co-op Dedicated Server)
---- svencoop
------ addons
-------- metamod
---------- dlls
------------ ascurl.dll (ascurl.so)
------------ asext.dll (asext.so)
------------ fallguys.dll (fallguys.so)
---------- plugins.ini
---- svencoop_addons
---- svencoop_downloads
---- svencoop.exe (svends.exe / svends_run.sh / svencoop.sh)
```

2. You should either 

* Use `-dll addons/metamod/dlls/metamod.dll`(Windows) or `-dll addons/metamod/dlls/metamod.so` (linux) as launch parameter to launch the game, 

* Or edit `Sven Co-op/svencoop/liblist.gam` , change

```
gamedll "dlls/server.dll"
gamedll_linux "dlls/server.so"
```

to
 
```
gamedll "addons/metamod/dlls/metamod.dll"
gamedll_linux "addons/metamod/dlls/metamod.so"
```


# Build Requirements (Windows)

1. [Visual Studio 2017 / 2019 / 2022, with vc141 / vc142 / vc143 toolset](https://visualstudio.microsoft.com/)

2. [CMake](https://cmake.org/download/)

3. [Git for Windows](https://gitforwindows.org/)

# Build Instruction (Windows)

1. `git clone https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys`

2. Run `build-win32-all.bat`

3. Be patient, `metamod.dll`, `asext.dll` and `fallguys.dll` will be generated at `build/addons/metamod/dlls` if no error(s) occours.

# Build Requirements (Linux)

1. GNU C++ Build environment. Install with `sudo apt-get install make build-essential gcc gcc-multilib g++-multilib` if missing.

2. CMake. Install with `suto apt-get install cmake` if missing.

3. Git client. Install with `suto apt-get install git` if missing.

# Build Instruction (Linux)

1. `git clone https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys`

2. `sudo chmod +777 build-all-linux.sh` to make it executable, then `./build-all-linux.sh`

3. Be patient, `metamod.so`, `asext.so` and `fallguys.so` will be generated at `build/addons/metamod/dlls` if no error(s) occours.

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

### You are welcome to request for any new hook which is not implemented in Sven Co-op yet.

## libcurl in AngelScript (aka ascurl)

* Take advantage of `libcurl.dll` or `libcurl.so` via AngelScript

### constants

```
const int ASCURL_METHOD_GET = 0;
const int ASCURL_METHOD_POST = 1;
const int ASCURL_METHOD_PUT = 2;
```

### Create a new HTTP Request

```
//int g_EngineFuncs.CreateHTTPRequest(const string& in url, bool async, int method, int conn_timeout_ms, int timeout_ms)

int request_id = g_EngineFuncs.CreateHTTPRequest("http://curl.se", 
				true,//false=sync, true=async
				ASCURL_METHOD_PUT,
				1000,//connect timeout in ms
				5000 //transfer timeout in ms
			);
```

### Set post field of http request (mostly POST)

```
//bool g_EngineFuncs.SetHTTPRequestPostField(int request_id, const string& in post_fields )

g_EngineFuncs.SetHTTPRequestPostField(request_id, "xyz=114514");//post_fields must be urlencoded before passed to SetHTTPRequestPostField 
```

```
//bool g_EngineFuncs.SetHTTPRequestPostFieldEx(int request_id, const string& in post_fields, int sizeof_post_fields )

g_EngineFuncs.SetHTTPRequestPostFieldEx(request_id, "xyz=114514");//post_fields must be urlencoded before passed to SetHTTPRequestPostField , sizeof_post_fields is used as size of post_fields. post_fields.length() is ignored
```

### Append request header to http request

```
//bool g_EngineFuncs.AppendHTTPRequestHeader(int request_id, const string& in header )

g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Content-Type: text/plain");
```

### Append form data to http request (mostly POST)

```
//bool AppendHTTPRequestFormString(int request_id, const string& in form, const string& in content )

g_EngineFuncs.AppendHTTPRequestFormString(request_id, "filename", "test.jpg");
```

```
//bool AppendHTTPRequestFormBlob(int request_id, const string& in form, const BLOB& in blob )

g_EngineFuncs.AppendHTTPRequestFormBlob(request_id, "filedata", pBlob);
```

### Upload using PUT method

```
//bool SetHTTPRequestUploadBlob(int request_id, const BLOB& in blob )

g_EngineFuncs.SetHTTPRequestUploadBlob(request_id, pBlob);
```

### Set asynchronous callback for http request

```
//void HTTPResponseCallback(int request_id)
//bool SetHTTPRequestCallback(int request_id, HTTPResponseCallback @callback )

g_EngineFuncs.SetHTTPRequestCallback(request_id, function(int reqid){
	int response_code = 0;
	string response_header;
	string response_body;
	g_EngineFuncs.GetHTTPResponse(reqid, response_code, response_header, response_body);

	if(response_code >= 200)
	{
		g_Game.AlertMessage(at_aiconsole,  "%1", response_header );

		g_EngineFuncs.DestroyHTTPRequest(reqid);
	}
});
```

### Send http request to server

```
//bool g_EngineFuncs.SendHTTPRequest(int request_id)

g_EngineFuncs.SendHTTPRequest(request_id);
```

### Get response header and body of http requset

```

//bool g_EngineFuncs.GetHTTPResponse(int request_id, int& out out_response, string& out out_header, string& out out_body)

int response_code = 0;
string response_header;
string response_body;
g_EngineFuncs.GetHTTPResponse(reqid, response_code, response_header, response_body);


```

### Destroy a http request

```
//bool g_EngineFuncs.DestroyHTTPRequest(int request_id)

g_EngineFuncs.DestroyHTTPRequest(request_id);
```

### Some util functions for hashing and encryption

```
//bool g_EngineFuncs.hmac_sha1(const string& in password, const string& in message, string& out outhash)//outhash is non-readable binary string
//bool g_EngineFuncs.hmac_md5(const string& in password, const string& in message, string& out outhash)//outhash is non-readable binary string
//bool g_EngineFuncs.md5(const string& in data, string& out outhash)//outhash is non-readable binary string
//bool g_EngineFuncs.base64_encode(const string& in hash, string& out outstr)//outstr is human-readable string
```

### Synchronous HTTP Client Example

```
int request_id = g_EngineFuncs.CreateHTTPRequest("http://www.baidu.com/", 
	false,
	ASCURL_METHOD_GET,
	1000,//connect timeout in ms
	5000 //transfer timeout in ms
);

g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
g_EngineFuncs.SendHTTPRequest(request_id);

int response_code = 0;
string response_header;
string response_body;
while(g_EngineFuncs.GetHTTPResponse(request_id, response_code, response_header, response_body))
{
	//if response_code == 100, keeps looping
	if(response_code >= 200)
		break;
}
g_EngineFuncs.DestroyHTTPRequest(request_id);
```

### Asynchronous HTTP Client Example

```
int request_id = g_EngineFuncs.CreateHTTPRequest("http://www.baidu.com/", 
	true,
	ASCURL_METHOD_GET,
	1000,//connect timeout in ms
	5000 //transfer timeout in ms
);

g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
g_EngineFuncs.SetHTTPRequestCallback(request_id, function(int reqid){
	int response_code = 0;
	string response_header;
	string response_body;
	g_EngineFuncs.GetHTTPResponse(reqid, response_code, response_header, response_body);

	if(response_code >= 200)
	{
		g_Game.AlertMessage(at_aiconsole,  "%1", response_header );
		g_Game.AlertMessage(at_aiconsole,  "%1", response_body );

		g_EngineFuncs.DestroyHTTPRequest(reqid);
	}
});
g_EngineFuncs.SendHTTPRequest(request_id);
```

### Synchronous AliyunOSS File Uploader Example

```
int get_weekday(int day, int month, int year) {
	int a, y, m, R;
	a = ( 14 - month ) / 12;
	y = year - a;
	m = month + 12 * a - 2;
	R = 7000 + ( day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12 );
	return R % 7;
}

string UnixTimeToGMT(time_t t)
{
	const int MILLISECONDS_IN_SECOND = 1000;
	const int MILLISECONDS_IN_MINUTE = 60000;
	const int MILLISECONDS_IN_HALF_HOUR = 1800000;
	const int MILLISECONDS_IN_HOUR = 3600000;
	const int MILLISECONDS_IN_DAY = 86400000;
	const int SECONDS_IN_MINUTE = 60;
	const int SECONDS_IN_HALF_HOUR = 1800;
	const int SECONDS_IN_HOUR = 3600;
	const int SECONDS_IN_DAY = 86400;
	const int SECONDS_IN_YEAR = 31536000;
	const int SECONDS_IN_LEAP_YEAR = 31622400;
	const int AVERAGE_SECONDS_IN_YEAR = 31557600;
	const int SECONDS_IN_4_YEAR = 126230400;
	const int MINUTES_IN_HOUR = 60;
	const int MINUTES_IN_DAY = 1440;
	const int MINUTES_IN_WEEK = 10080;
	const int MINUTES_IN_MONTH = 40320;
	const int HOURS_IN_DAY = 24;
	const int MONTHS_IN_YEAR = 12;
	const int DAYS_IN_WEEK = 7;
	const int DAYS_IN_LEAP_YEAR = 366;
	const int DAYS_IN_YEAR = 365;
	const int DAYS_IN_4_YEAR = 1461;
	const int FIRST_YEAR_UNIX = 1970;
	const int MAX_DAY_MONTH = 31;
	const int OADATE_UNIX_EPOCH = 25569;
	const int _TBIAS_DAYS = 25567;
	const int _TBIAS_YEAR = 1900;

	time_t _secs = t;
	int _mon, _year;
	int _days;
	int i;

	_days = _TBIAS_DAYS;

	_days += _secs / SECONDS_IN_DAY; _secs = _secs % SECONDS_IN_DAY;
	int hour = _secs / SECONDS_IN_HOUR; _secs %= SECONDS_IN_HOUR;
	int minute = _secs / SECONDS_IN_MINUTE;
	int second = _secs % SECONDS_IN_MINUTE;
	int month = 0;
	int day = 0;
	array<int>	lmos = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
	array<int>	mos = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

	_year = _days / DAYS_IN_YEAR;
	int mos_i;
	mos_i = ( ((_year & 3) != 0) || (_year == 0) ) ? mos[0] : lmos[0];
	i = ((_year - 1) / 4) + mos_i + DAYS_IN_YEAR*_year;
	while (_days < i){
		-_year;
		mos_i = ( ((_year & 3) != 0) || (_year == 0) ) ? mos[0] : lmos[0];
		i = ((_year - 1) / 4) + mos_i + DAYS_IN_YEAR*_year;
	}
	_days -= i;
	int year = _year + _TBIAS_YEAR;

	if( ((_year & 3) != 0) || (_year == 0) ) {
		// mos
		for(_mon = MONTHS_IN_YEAR; _days < mos[--_mon];);
		month = _mon + 1;
		day = _days - mos[_mon] + 1;
	} else {
		for(_mon = MONTHS_IN_YEAR; _days < lmos[--_mon];);
		month = _mon + 1;
		day = _days - lmos[_mon] + 1;
	}

	//0~11

	int week = get_weekday(day, month, year);

	array<string> weeknames = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	array<string> monthnames = { "", "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

	string fullhour = (hour < 10) ? ("0"+hour) : hour;
	string fullmin = (minute < 10) ? ("0"+minute) : minute;
	string fullsec = (second < 10) ? ("0"+second) : second;
	return weeknames[week] + ", " + day + " " + monthnames[month] + " " + year + " " + fullhour + ":" + fullmin + ":" + fullsec + " GMT";
}

string szSoundCacheFile = "maps/soundcache/"+g_Engine.mapname+".txt";
size_t nFileSize = 0;
File@ pFile = g_FileSystem.OpenFile(szSoundCacheFile, OpenFile::READ);
if (pFile !is null && pFile.IsOpen())
{
	nFileSize = pFile.GetSize();

	BLOB @pBlob = pFile.ReadBlob();

	string AccessKeyId = "************";
	string AccessKeySecret = "************************";

	string Host = "****bucket****.oss-cn-shanghai.aliyuncs.com";
	string CanonicalizedResource = "/****bucket****/"+szSoundCacheFile;
	string szFileMD5 = "";
	string szFileType = "text/plain";

	string GMTDate = UnixTimeToGMT(UnixTimestamp());
	string CanonicalizedOSSHeaders = "";
	string Signature;
	string SignatureBase64;
	g_EngineFuncs.hmac_sha1(AccessKeySecret,
				"PUT" + "\n"
				+ szFileMD5 + "\n" 
				+ szFileType + "\n" 
				+ GMTDate + "\n" 
				+ CanonicalizedOSSHeaders
				+ CanonicalizedResource, Signature);
	g_EngineFuncs.base64_encode(Signature, SignatureBase64);
	string Authorization = "OSS " + AccessKeyId + ":" + SignatureBase64;

	int request_id = g_EngineFuncs.CreateHTTPRequest("http://"+Host+"/"+szSoundCacheFile, 
		false,//false=sync, true=async
		ASCURL_METHOD_PUT,
		1000,//connect timeout in ms
		5000 //transfer timeout in ms
	);

	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Host: " + Host);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Content-Type: " + szFileType);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Content-Length: " + nFileSize);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Date: " + GMTDate);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Authorization: " + Authorization);
	g_EngineFuncs.SetHTTPRequestUploadBlob(request_id, pBlob);
	g_EngineFuncs.SendHTTPRequest(request_id);

	int response_code = 0;
	string response_header;
	string response_body;
	while(g_EngineFuncs.GetHTTPResponse(request_id, response_code, response_header, response_body))
	{
		if(response_code >= 200)
			break;
	}
	g_EngineFuncs.DestroyHTTPRequest(request_id);

	pFile.Close();
}
```

## Third-Party AngelScript Extension

You can register your own hooks or methods in AngelScript engine.

```
#include "asext_api.h"// metamod-fallguys\fallguys\asext_api.h it's there

#define IMPORT_FUNCTION_DEFINE(name) fn##name name;

#define IMPORT_FUNCTION_DLSYM(dll, name) name = (decltype(name))DLSYM((DLHANDLE)dll##Handle, #name);\
if (!name)\
{\
	LOG_ERROR(PLID, "Failed to get " #name " from " #dll " dll !");\
	return FALSE;\
}

IMPORT_FUNCTION_DEFINE(ASEXT_RegisterDocInitCallback);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterDirInitCallback);

IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectMethod);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectType);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectProperty);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterFuncDef);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterHook);

IMPORT_FUNCTION_DEFINE(ASEXT_CreateDirectory);
IMPORT_FUNCTION_DEFINE(ASEXT_CStringAssign);
IMPORT_FUNCTION_DEFINE(ASEXT_CStringdtor);
IMPORT_FUNCTION_DEFINE(ASEXT_GetServerManager);
IMPORT_FUNCTION_DEFINE(ASEXT_CreateCASFunction);
IMPORT_FUNCTION_DEFINE(ASEXT_CASRefCountedBaseClass_InternalRelease);

fnASEXT_CallHook *ASEXT_CallHook = NULL;

fnASEXT_CallCASBaseCallable *ASEXT_CallCASBaseCallable = NULL;

```

//Initialization in Meta_Attach (meta_api.cpp)

`C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME /* now */,
	META_FUNCTIONS* pFunctionTable, meta_globals_t* pMGlobals,
	gamedll_funcs_t* pGamedllFuncs){`

```
	//....
	//Load asext dll
	void *asextHandle = NULL;
#ifdef _WIN32
	LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.dll", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#else
	LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.so", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#endif
	if (!asextHandle)
	{
		LOG_ERROR(PLID, "asext dll not found!");
		return FALSE;
	}

	//Load asext API
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallHook);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallCASBaseCallable);

	IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterDocInitCallback);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterDirInitCallback);

	IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterObjectMethod);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterObjectType);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterObjectProperty);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterFuncDef);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterHook);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_CreateDirectory);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_CStringAssign);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_CStringdtor);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_GetServerManager);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_CreateCASFunction);
	IMPORT_FUNCTION_DLSYM(asext, ASEXT_CASRefCountedBaseClass_InternalRelease);

```

### Register your own AngelScript methods

```
int SC_SERVER_DECL CASEngineFuncs__TestFunc(void* pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	return 114514;
}

//Must be registered before AS initialization, Meta_Attach is okay

	ASEXT_RegisterDocInitCallback([](void *pASDoc) {

		ASEXT_RegisterObjectMethod(pASDoc,
			"A Test Function", "CEngineFuncs", "int TestFunc()",
			(void *)CASEngineFuncs__TestFunc, 3);

	});
	
```

Now you can call this from game, you will get test = 114514 :
```
int test = g_EngineFuncs.TestFunc();
```

### Register your own hooks

```

//Global var

void *g_PlayerPostThinkPostHook = NULL;

```

```

//Must be registered before AS initialization, Meta_Attach is okay

g_PlayerPostThinkPostHook = ASEXT_RegisterHook("Post call of gEntityInterface.pfnPlayerPostThink", StopMode_CALL_ALL, 2, ASHookFlag_MapScript | ASHookFlag_Plugin, "Player", "PlayerPostThinkPost", "CBasePlayer@ pPlayer");

```

```
//Where you need to call AS hooks
void NewPlayerPostThink_Post(edict_t *pEntity)
{
	if(ASEXT_CallHook)//The second arg must be zero, the third, 4th, 5th, 6th... args are the real args pass to AngelScript VM.
		(*ASEXT_CallHook)(g_PlayerPostThinkPostHook, 0, pEntity->pvPrivateData);

	SET_META_RESULT(MRES_IGNORED);
}
```


//Now you can register hook from game :
```
    g_Hooks.RegisterHook(Hooks::Player::PlayerPostThinkPost, @PlayerPostThinkPost);
```
