// vi: set ts=4 sw=4 :
// vim: set tw=75 :

/*
 * Copyright (c) 2001-2006 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include <extdll.h>

#include <meta_api.h>

#include "enginedef.h"
#include "serverdef.h"
#include "asusermsg.h"

void NewMessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHookEnabled(msg_type)) {
		
		g_UserMsgHookManager.MessageBegin(msg_dest, msg_type, pOrigin, ed);

		RETURN_META(MRES_SUPERCEDE);
		return;
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteByte(int iValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteByte(iValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteChar(int iValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteChar(iValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteShort(int iValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteShort(iValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteLong(int iValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteLong(iValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteAngle(float flValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteAngle(flValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteCoord(float flValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteCoord(flValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteString(const char *szValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteString(szValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewWriteEntity(int iValue)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {
		g_UserMsgHookManager.WriteEntity(iValue);
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void NewMessageEnd(void)
{
	if (g_UserMsgHookManager.IsHookGlobalEnabled() && g_UserMsgHookManager.IsHooked()) {

		if (g_UserMsgHookManager.CallUserMsgHook())
		{
			g_UserMsgHookManager.ClearMessage();
			RETURN_META(MRES_SUPERCEDE);
		}

		g_UserMsgHookManager.SendBufferedMessage();
		g_UserMsgHookManager.ClearMessage();
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}


enginefuncs_t meta_engfuncs = 
{
	NULL,						// pfnPrecacheModel()
	NULL,						// pfnPrecacheSound()
	NULL,						// pfnSetModel()
	NULL,						// pfnModelIndex()
	NULL,						// pfnModelFrames()

	NULL,						// pfnSetSize()
	NULL,						// pfnChangeLevel()
	NULL,						// pfnGetSpawnParms()
	NULL,						// pfnSaveSpawnParms()

	NULL,						// pfnVecToYaw()
	NULL,						// pfnVecToAngles()
	NULL,						// pfnMoveToOrigin()
	NULL,						// pfnChangeYaw()
	NULL,						// pfnChangePitch()

	NULL,						// pfnFindEntityByString()
	NULL,						// pfnGetEntityIllum()
	NULL,						// pfnFindEntityInSphere()
	NULL,						// pfnFindClientInPVS()
	NULL,						// pfnEntitiesInPVS()

	NULL,						// pfnMakeVectors()
	NULL,						// pfnAngleVectors()

	NULL,						// pfnCreateEntity()
	NULL,						// pfnRemoveEntity()
	NULL,						// pfnCreateNamedEntity()

	NULL,						// pfnMakeStatic()
	NULL,						// pfnEntIsOnFloor()
	NULL,						// pfnDropToFloor()

	NULL,						// pfnWalkMove()
	NULL,						// pfnSetOrigin()

	NULL,						// pfnEmitSound()
	NULL,						// pfnEmitAmbientSound()

	NULL,						// pfnTraceLine()
	NULL,						// pfnTraceToss()
	NULL,						// pfnTraceMonsterHull()
	NULL,						// pfnTraceHull()
	NULL,						// pfnTraceModel()
	NULL,						// pfnTraceTexture()
	NULL,						// pfnTraceSphere()
	NULL,						// pfnGetAimVector()

	NULL,						// pfnServerCommand()
	NULL,						// pfnServerExecute()
	NULL,						// pfnClientCommand()

	NULL,						// pfnParticleEffect()
	NULL,						// pfnLightStyle()
	NULL,						// pfnDecalIndex()
	NULL,						// pfnPointContents()

	NewMessageBegin,			// pfnMessageBegin()
	NewMessageEnd,				// pfnMessageEnd()

	NewWriteByte,				// pfnWriteByte()
	NewWriteByte,				// pfnWriteChar()
	NewWriteShort,				// pfnWriteShort()
	NewWriteLong,				// pfnWriteLong()
	NewWriteAngle,				// pfnWriteAngle()
	NewWriteCoord,				// pfnWriteCoord()
	NewWriteString,				// pfnWriteString()
	NewWriteEntity,				// pfnWriteEntity()

	NULL,						// pfnCVarRegister()
	NULL,						// pfnCVarGetFloat()
	NULL,						// pfnCVarGetString()
	NULL,						// pfnCVarSetFloat()
	NULL,						// pfnCVarSetString()

	NULL,						// pfnAlertMessage()
	NULL,						// pfnEngineFprintf()

	NULL,						// pfnPvAllocEntPrivateData()
	NULL,						// pfnPvEntPrivateData()
	NULL,						// pfnFreeEntPrivateData()

	NULL,						// pfnSzFromIndex()
	NULL,						// pfnAllocString()

	NULL, 						// pfnGetVarsOfEnt()
	NULL,						// pfnPEntityOfEntOffset()
	NULL,						// pfnEntOffsetOfPEntity()
	NULL,						// pfnIndexOfEdict()
	NULL,						// pfnPEntityOfEntIndex()
	NULL,						// pfnFindEntityByVars()
	NULL,						// pfnGetModelPtr()

	NULL,						// pfnRegUserMsg()

	NULL,						// pfnAnimationAutomove()
	NULL,						// pfnGetBonePosition()

	NULL,						// pfnFunctionFromName()
	NULL,						// pfnNameForFunction()

	NULL,						// pfnClientPrintf()
	NULL,						// pfnServerPrint()

	NULL,						// pfnCmd_Args()
	NULL,						// pfnCmd_Argv()
	NULL,						// pfnCmd_Argc()

	NULL,						// pfnGetAttachment()

	NULL,						// pfnCRC32_Init()
	NULL,						// pfnCRC32_ProcessBuffer()
	NULL,						// pfnCRC32_ProcessByte()
	NULL,						// pfnCRC32_Final()

	NULL,						// pfnRandomLong()
	NULL,						// pfnRandomFloat()

	NULL,						// pfnSetView()
	NULL,						// pfnTime()
	NULL,						// pfnCrosshairAngle()

	NULL,						// pfnLoadFileForMe()
	NULL,						// pfnFreeFile()

	NULL,						// pfnEndSection()
	NULL,						// pfnCompareFileTime()
	NULL,						// pfnGetGameDir()
	NULL,						// pfnCvar_RegisterVariable()
	NULL,						// pfnFadeClientVolume()
	NULL,						// pfnSetClientMaxspeed()
	NULL,						// pfnCreateFakeClient()
	NULL,						// pfnRunPlayerMove()
	NULL,						// pfnNumberOfEntities()

	NULL,						// pfnGetInfoKeyBuffer()
	NULL,						// pfnInfoKeyValue()
	NULL,						// pfnSetKeyValue()
	NULL,						// pfnSetClientKeyValue()

	NULL,						// pfnIsMapValid()
	NULL,						// pfnStaticDecal()
	NULL,						// pfnPrecacheGeneric()
	NULL, 						// pfnGetPlayerUserId()
	NULL,						// pfnBuildSoundMsg()
	NULL,						// pfnIsDedicatedServer()
	NULL,						// pfnCVarGetPointer()
	NULL,						// pfnGetPlayerWONId()

	NULL,						// pfnInfo_RemoveKey()
	NULL,						// pfnGetPhysicsKeyValue()
	NULL,						// pfnSetPhysicsKeyValue()
	NULL,						// pfnGetPhysicsInfoString()
	NULL,						// pfnPrecacheEvent()
	NULL,						// pfnPlaybackEvent()

	NULL,						// pfnSetFatPVS()
	NULL,						// pfnSetFatPAS()

	NULL,						// pfnCheckVisibility()

	NULL,						// pfnDeltaSetField()
	NULL,						// pfnDeltaUnsetField()
	NULL,						// pfnDeltaAddEncoder()
	NULL,						// pfnGetCurrentPlayer()
	NULL,						// pfnCanSkipPlayer()
	NULL,						// pfnDeltaFindField()
	NULL,						// pfnDeltaSetFieldByIndex()
	NULL,						// pfnDeltaUnsetFieldByIndex()

	NULL,						// pfnSetGroupMask()

	NULL,						// pfnCreateInstancedBaseline()
	NULL,						// pfnCvar_DirectSet()

	NULL,						// pfnForceUnmodified()

	NULL,						// pfnGetPlayerStats()

	NULL,						// pfnAddServerCommand()

	// Added in SDK 2.2:
	NULL,						// pfnVoice_GetClientListening()
	NULL,						// pfnVoice_SetClientListening()

	// Added for HL 1109 (no SDK update):
	NULL,						// pfnGetPlayerAuthId()

	// Added 2003/11/10 (no SDK update):
	NULL,						// pfnSequenceGet()
	NULL,						// pfnSequencePickSentence()
	NULL,						// pfnGetFileSize()
	NULL,						// pfnGetApproxWavePlayLen()
	NULL,						// pfnIsCareerMatch()
	NULL,						// pfnGetLocalizedStringLength()
	NULL,						// pfnRegisterTutorMessageShown()
	NULL,						// pfnGetTimesTutorMessageShown()
	NULL,						// pfnProcessTutorMessageDecayBuffer()
	NULL,						// pfnConstructTutorMessageDecayBuffer()
	NULL,						// pfnResetTutorMessageDecayData()

	// Added Added 2005-08-11 (no SDK update)
	NULL,						// pfnQueryClientCvarValue()
	// Added Added 2005-11-22 (no SDK update)
	NULL,						// pfnQueryClientCvarValue2()
	// Added 2009-06-17 (no SDK update)
	NULL,						// pfnEngCheckParm()
};

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, 
		int *interfaceVersion) 
{
	if(!pengfuncsFromEngine) {
		LOG_ERROR(PLID, "GetEngineFunctions called with null pengfuncsFromEngine");
		return(FALSE);
	}
	else if(*interfaceVersion != ENGINE_INTERFACE_VERSION) {
		LOG_ERROR(PLID, "GetEngineFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, ENGINE_INTERFACE_VERSION);
		// Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = ENGINE_INTERFACE_VERSION;
		return(FALSE);
	}
	memcpy(pengfuncsFromEngine, &meta_engfuncs, sizeof(enginefuncs_t));
	return TRUE;
}