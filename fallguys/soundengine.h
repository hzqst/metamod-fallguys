#pragma once

bool LoadFMOD();
void UnloadFMOD();

class SoundEngine_SoundInfo
{
public:
	SoundEngine_SoundInfo();

	int type;
	int format;
	int channels;
	int bits;
	unsigned int length;
};

void SoundEngine_SoundInfo_ctor(SoundEngine_SoundInfo *pthis);
void SoundEngine_SoundInfo_copyctor(SoundEngine_SoundInfo *a1, SoundEngine_SoundInfo *a2);
SoundEngine_SoundInfo * SC_SERVER_DECL SoundEngine_SoundInfo_opassign(SoundEngine_SoundInfo *a1, SC_SERVER_DUMMYARG SoundEngine_SoundInfo *a2);
void SoundEngine_SoundInfo_dtor(SoundEngine_SoundInfo *pthis);

bool SoundEngine_GetSoundInfo(const char *szSoundName, SoundEngine_SoundInfo *SoundInfo);