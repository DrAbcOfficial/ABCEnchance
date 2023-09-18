#include <metahook.h>
#include "soundengine.h"

namespace FModEngine {
	static HMODULE g_hFmod = nullptr;
	static CFModSystem* g_pFModSystem;
#define FMOD_FUNCTION_DEFINE(name) static decltype(name) * g_pfn##name
#ifdef __USE_FMOD_SYSTEM_H_
		/*
		FMOD System factory functions.  Use this to create an FMOD System Instance.  below you will see FMOD_System_Init/Close to get started.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_Create);
		FMOD_FUNCTION_DEFINE(FMOD_System_Release);

		/*
			'System' API
		*/
		/*
			 Pre-init functions.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_SetOutput);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetOutput);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetNumDrivers);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetDriverInfo);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetDriverInfoW);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetDriverCaps);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetDriver);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetDriver);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetHardwareChannels);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetSoftwareChannels);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetSoftwareChannels);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetSoftwareFormat);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetSoftwareFormat);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetDSPBufferSize);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetDSPBufferSize);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetFileSystem);
		FMOD_FUNCTION_DEFINE(FMOD_System_AttachFileSystem);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetAdvancedSettings);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetAdvancedSettings);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetSpeakerMode);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetSpeakerMode);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetCallback);
		/*
			 Plug-in support
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_SetPluginPath);
		FMOD_FUNCTION_DEFINE(FMOD_System_LoadPlugin);
		FMOD_FUNCTION_DEFINE(FMOD_System_UnloadPlugin);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetNumPlugins);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetPluginHandle);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetPluginInfo);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetOutputByPlugin);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetOutputByPlugin);
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateDSPByPlugin);
		FMOD_FUNCTION_DEFINE(FMOD_System_RegisterCodec);
		FMOD_FUNCTION_DEFINE(FMOD_System_RegisterDSP);
		/*
			 Init/Close
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_Init);
		FMOD_FUNCTION_DEFINE(FMOD_System_Close);
		/*
			 General post-init system functions
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_Update);
		FMOD_FUNCTION_DEFINE(FMOD_System_Set3DSettings);
		FMOD_FUNCTION_DEFINE(FMOD_System_Get3DSettings);
		FMOD_FUNCTION_DEFINE(FMOD_System_Set3DNumListeners);
		FMOD_FUNCTION_DEFINE(FMOD_System_Get3DNumListeners);
		FMOD_FUNCTION_DEFINE(FMOD_System_Set3DListenerAttributes);
		FMOD_FUNCTION_DEFINE(FMOD_System_Get3DListenerAttributes);
		FMOD_FUNCTION_DEFINE(FMOD_System_Set3DRolloffCallback);
		FMOD_FUNCTION_DEFINE(FMOD_System_Set3DSpeakerPosition);
		FMOD_FUNCTION_DEFINE(FMOD_System_Get3DSpeakerPosition);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetStreamBufferSize);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetStreamBufferSize);
		/*
			 System information functions.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_GetVersion);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetOutputHandle);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetChannelsPlaying);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetHardwareChannels);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetCPUUsage);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetSoundRAM);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetNumCDROMDrives);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetCDROMDriveName);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetSpectrum);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetWaveData);
		/*
			 Sound/DSP/Channel/FX creation and retrieval.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateSound);
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateStream);
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateDSP);
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateDSPByType);
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateChannelGroup);
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateSoundGroup);
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateReverb);
		FMOD_FUNCTION_DEFINE(FMOD_System_PlaySound);
		FMOD_FUNCTION_DEFINE(FMOD_System_PlayDSP);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetChannel);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetMasterChannelGroup);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetMasterSoundGroup);
		/*
			 Reverb API
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_SetReverbProperties);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetReverbProperties);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetReverbAmbientProperties);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetReverbAmbientProperties);
		/*
			 System level DSP access.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_GetDSPHead);
		FMOD_FUNCTION_DEFINE(FMOD_System_AddDSP);
		FMOD_FUNCTION_DEFINE(FMOD_System_LockDSP);
		FMOD_FUNCTION_DEFINE(FMOD_System_UnlockDSP);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetDSPClock);
		/*
			 Recording API.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_GetRecordNumDrivers);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetRecordDriverInfo);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetRecordDriverInfoW);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetRecordDriverCaps);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetRecordPosition);
		FMOD_FUNCTION_DEFINE(FMOD_System_RecordStart);
		FMOD_FUNCTION_DEFINE(FMOD_System_RecordStop);
		FMOD_FUNCTION_DEFINE(FMOD_System_IsRecording);
		/*
			 Geometry API.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_CreateGeometry);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetGeometrySettings);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetGeometrySettings);
		FMOD_FUNCTION_DEFINE(FMOD_System_LoadGeometry);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetGeometryOcclusion);
		/*
			 Network functions.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_SetNetworkProxy);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetNetworkProxy);
		FMOD_FUNCTION_DEFINE(FMOD_System_SetNetworkTimeout);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetNetworkTimeout);
		/*
			 Userdata set/get.
		*/
		FMOD_FUNCTION_DEFINE(FMOD_System_SetUserData);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetUserData);
		FMOD_FUNCTION_DEFINE(FMOD_System_GetMemoryInfo);
#endif
#ifdef  __USE_FMOD_SOUND_H_
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Release);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetSystemObject);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Lock);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Unlock);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetDefaults);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetDefaults);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetVariations);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetVariations);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Set3DMinMaxDistance);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Get3DMinMaxDistance);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Set3DConeSettings);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Get3DConeSettings);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Set3DCustomRolloff);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_Get3DCustomRolloff);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetSubSound);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetSubSound);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetSubSoundParent);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetSubSoundSentence);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetName);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetLength);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetFormat);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetNumSubSounds);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetNumTags);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetTag);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetOpenState);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_ReadData);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SeekData);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetSoundGroup);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetSoundGroup);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetNumSyncPoints);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetSyncPoint);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetSyncPointInfo);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_AddSyncPoint);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_DeleteSyncPoint);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetMode);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetMode);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetLoopCount);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetLoopCount);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetLoopPoints);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetLoopPoints);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetMusicNumChannels);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetMusicChannelVolume);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetMusicChannelVolume);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetMusicSpeed);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetMusicSpeed);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_SetUserData);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetUserData);
		FMOD_FUNCTION_DEFINE(FMOD_Sound_GetMemoryInfo);
#endif //  __USE_FMOD_SOUND_H_
#ifdef __USE_FMOD_CHANNEL_H_
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetSystemObject);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Stop);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetPaused);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetPaused);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetVolume);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetVolume);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetFrequency);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetFrequency);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetPan);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetPan);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetDelay);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetDelay);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetSpeakerMix);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetSpeakerMix);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetSpeakerLevels);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetSpeakerLevels);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetInputChannelMix);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetInputChannelMix);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetMute);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetMute);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetPriority);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetPriority);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetPosition);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetPosition);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetReverbProperties);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetReverbProperties);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetLowPassGain);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetLowPassGain);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetChannelGroup);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetChannelGroup);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetCallback);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DAttributes);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DAttributes);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DMinMaxDistance);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DMinMaxDistance);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DConeSettings);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DConeSettings);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DConeOrientation);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DConeOrientation);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DCustomRolloff);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DCustomRolloff);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DOcclusion);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DOcclusion);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DSpread);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DSpread);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DPanLevel);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DPanLevel);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DDopplerLevel);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DDopplerLevel);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Set3DDistanceFilter);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_Get3DDistanceFilter);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetDSPHead);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_AddDSP);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_IsPlaying);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_IsVirtual);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetAudibility);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetCurrentSound);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetSpectrum);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetWaveData);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetIndex);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetMode);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetMode);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetLoopCount);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetLoopCount);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetLoopPoints);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetLoopPoints);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_SetUserData);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetUserData);
		FMOD_FUNCTION_DEFINE(FMOD_Channel_GetMemoryInfo);
#endif // __USE_FMOD_CHANNEL_H_

	void InitFModLibrary() {
		g_hFmod = GetModuleHandle("svencoop/fmodex.dll");
		if (!g_hFmod)
			g_hFmod = LoadLibrary("svencoop/fmodex.dll");
		if (g_hFmod) {
#define FMOD_DLSYM_CLIEN(name) g_pfn##name = (decltype(g_pfn##name))GetProcAddress(g_hFmod, #name);
#ifdef __USE_FMOD_SYSTEM_H_
			/*
			FMOD System factory functions.  Use this to create an FMOD System Instance.  below you will see FMOD_System_Init/Close to get started.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_Create);
			FMOD_DLSYM_CLIEN(FMOD_System_Release);

			/*
				'System' API
			*/
			/*
				 Pre-init functions.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_SetOutput);
			FMOD_DLSYM_CLIEN(FMOD_System_GetOutput);
			FMOD_DLSYM_CLIEN(FMOD_System_GetNumDrivers);
			FMOD_DLSYM_CLIEN(FMOD_System_GetDriverInfo);
			FMOD_DLSYM_CLIEN(FMOD_System_GetDriverInfoW);
			FMOD_DLSYM_CLIEN(FMOD_System_GetDriverCaps);
			FMOD_DLSYM_CLIEN(FMOD_System_SetDriver);
			FMOD_DLSYM_CLIEN(FMOD_System_GetDriver);
			FMOD_DLSYM_CLIEN(FMOD_System_SetHardwareChannels);
			FMOD_DLSYM_CLIEN(FMOD_System_SetSoftwareChannels);
			FMOD_DLSYM_CLIEN(FMOD_System_GetSoftwareChannels);
			FMOD_DLSYM_CLIEN(FMOD_System_SetSoftwareFormat);
			FMOD_DLSYM_CLIEN(FMOD_System_GetSoftwareFormat);
			FMOD_DLSYM_CLIEN(FMOD_System_SetDSPBufferSize);
			FMOD_DLSYM_CLIEN(FMOD_System_GetDSPBufferSize);
			FMOD_DLSYM_CLIEN(FMOD_System_SetFileSystem);
			FMOD_DLSYM_CLIEN(FMOD_System_AttachFileSystem);
			FMOD_DLSYM_CLIEN(FMOD_System_SetAdvancedSettings);
			FMOD_DLSYM_CLIEN(FMOD_System_GetAdvancedSettings);
			FMOD_DLSYM_CLIEN(FMOD_System_SetSpeakerMode);
			FMOD_DLSYM_CLIEN(FMOD_System_GetSpeakerMode);
			FMOD_DLSYM_CLIEN(FMOD_System_SetCallback);
			/*
				 Plug-in support
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_SetPluginPath);
			FMOD_DLSYM_CLIEN(FMOD_System_LoadPlugin);
			FMOD_DLSYM_CLIEN(FMOD_System_UnloadPlugin);
			FMOD_DLSYM_CLIEN(FMOD_System_GetNumPlugins);
			FMOD_DLSYM_CLIEN(FMOD_System_GetPluginHandle);
			FMOD_DLSYM_CLIEN(FMOD_System_GetPluginInfo);
			FMOD_DLSYM_CLIEN(FMOD_System_SetOutputByPlugin);
			FMOD_DLSYM_CLIEN(FMOD_System_GetOutputByPlugin);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateDSPByPlugin);
			FMOD_DLSYM_CLIEN(FMOD_System_RegisterCodec);
			FMOD_DLSYM_CLIEN(FMOD_System_RegisterDSP);
			/*
				 Init/Close
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_Init);
			FMOD_DLSYM_CLIEN(FMOD_System_Close);
			/*
				 General post-init system functions
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_Update);
			FMOD_DLSYM_CLIEN(FMOD_System_Set3DSettings);
			FMOD_DLSYM_CLIEN(FMOD_System_Get3DSettings);
			FMOD_DLSYM_CLIEN(FMOD_System_Set3DNumListeners);
			FMOD_DLSYM_CLIEN(FMOD_System_Get3DNumListeners);
			FMOD_DLSYM_CLIEN(FMOD_System_Set3DListenerAttributes);
			FMOD_DLSYM_CLIEN(FMOD_System_Get3DListenerAttributes);
			FMOD_DLSYM_CLIEN(FMOD_System_Set3DRolloffCallback);
			FMOD_DLSYM_CLIEN(FMOD_System_Set3DSpeakerPosition);
			FMOD_DLSYM_CLIEN(FMOD_System_Get3DSpeakerPosition);
			FMOD_DLSYM_CLIEN(FMOD_System_SetStreamBufferSize);
			FMOD_DLSYM_CLIEN(FMOD_System_GetStreamBufferSize);
			/*
				 System information functions.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_GetVersion);
			FMOD_DLSYM_CLIEN(FMOD_System_GetOutputHandle);
			FMOD_DLSYM_CLIEN(FMOD_System_GetChannelsPlaying);
			FMOD_DLSYM_CLIEN(FMOD_System_GetHardwareChannels);
			FMOD_DLSYM_CLIEN(FMOD_System_GetCPUUsage);
			FMOD_DLSYM_CLIEN(FMOD_System_GetSoundRAM);
			FMOD_DLSYM_CLIEN(FMOD_System_GetNumCDROMDrives);
			FMOD_DLSYM_CLIEN(FMOD_System_GetCDROMDriveName);
			FMOD_DLSYM_CLIEN(FMOD_System_GetSpectrum);
			FMOD_DLSYM_CLIEN(FMOD_System_GetWaveData);
			/*
				 Sound/DSP/Channel/FX creation and retrieval.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_CreateSound);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateStream);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateDSP);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateDSPByType);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateChannelGroup);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateSoundGroup);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateReverb);
			FMOD_DLSYM_CLIEN(FMOD_System_PlaySound);
			FMOD_DLSYM_CLIEN(FMOD_System_PlayDSP);
			FMOD_DLSYM_CLIEN(FMOD_System_GetChannel);
			FMOD_DLSYM_CLIEN(FMOD_System_GetMasterChannelGroup);
			FMOD_DLSYM_CLIEN(FMOD_System_GetMasterSoundGroup);
			/*
				 Reverb API
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_SetReverbProperties);
			FMOD_DLSYM_CLIEN(FMOD_System_GetReverbProperties);
			FMOD_DLSYM_CLIEN(FMOD_System_SetReverbAmbientProperties);
			FMOD_DLSYM_CLIEN(FMOD_System_GetReverbAmbientProperties);
			/*
				 System level DSP access.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_GetDSPHead);
			FMOD_DLSYM_CLIEN(FMOD_System_AddDSP);
			FMOD_DLSYM_CLIEN(FMOD_System_LockDSP);
			FMOD_DLSYM_CLIEN(FMOD_System_UnlockDSP);
			FMOD_DLSYM_CLIEN(FMOD_System_GetDSPClock);
			/*
				 Recording API.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_GetRecordNumDrivers);
			FMOD_DLSYM_CLIEN(FMOD_System_GetRecordDriverInfo);
			FMOD_DLSYM_CLIEN(FMOD_System_GetRecordDriverInfoW);
			FMOD_DLSYM_CLIEN(FMOD_System_GetRecordDriverCaps);
			FMOD_DLSYM_CLIEN(FMOD_System_GetRecordPosition);
			FMOD_DLSYM_CLIEN(FMOD_System_RecordStart);
			FMOD_DLSYM_CLIEN(FMOD_System_RecordStop);
			FMOD_DLSYM_CLIEN(FMOD_System_IsRecording);
			/*
				 Geometry API.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_CreateGeometry);
			FMOD_DLSYM_CLIEN(FMOD_System_SetGeometrySettings);
			FMOD_DLSYM_CLIEN(FMOD_System_GetGeometrySettings);
			FMOD_DLSYM_CLIEN(FMOD_System_LoadGeometry);
			FMOD_DLSYM_CLIEN(FMOD_System_GetGeometryOcclusion);
			/*
				 Network functions.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_SetNetworkProxy);
			FMOD_DLSYM_CLIEN(FMOD_System_GetNetworkProxy);
			FMOD_DLSYM_CLIEN(FMOD_System_SetNetworkTimeout);
			FMOD_DLSYM_CLIEN(FMOD_System_GetNetworkTimeout);
			/*
				 Userdata set/get.
			*/
			FMOD_DLSYM_CLIEN(FMOD_System_SetUserData);
			FMOD_DLSYM_CLIEN(FMOD_System_GetUserData);
			FMOD_DLSYM_CLIEN(FMOD_System_GetMemoryInfo);
#endif // FMOD_SYSTEM
#ifdef __USE_FMOD_SOUND_H_
			FMOD_DLSYM_CLIEN(FMOD_Sound_Release);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetSystemObject);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Lock);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Unlock);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetDefaults);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetDefaults);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetVariations);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetVariations);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Set3DMinMaxDistance);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Get3DMinMaxDistance);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Set3DConeSettings);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Get3DConeSettings);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Set3DCustomRolloff);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Get3DCustomRolloff);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetSubSound);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetSubSound);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetSubSoundParent);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetSubSoundSentence);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetName);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetLength);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetFormat);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetNumSubSounds);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetNumTags);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetTag);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetOpenState);
			FMOD_DLSYM_CLIEN(FMOD_Sound_ReadData);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SeekData);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetSoundGroup);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetSoundGroup);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetNumSyncPoints);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetSyncPoint);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetSyncPointInfo);
			FMOD_DLSYM_CLIEN(FMOD_Sound_AddSyncPoint);
			FMOD_DLSYM_CLIEN(FMOD_Sound_DeleteSyncPoint);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetMode);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetMode);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetLoopCount);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetLoopCount);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetLoopPoints);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetLoopPoints);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetMusicNumChannels);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetMusicChannelVolume);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetMusicChannelVolume);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetMusicSpeed);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetMusicSpeed);
			FMOD_DLSYM_CLIEN(FMOD_Sound_SetUserData);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetUserData);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetMemoryInfo);
#endif // __USE_FMOD_SOUND_H_
#ifdef __USE_FMOD_CHANNEL_H_
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetSystemObject);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Stop);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetPaused);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetPaused);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetVolume);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetVolume);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetFrequency);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetFrequency);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetPan);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetPan);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetDelay);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetDelay);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetSpeakerMix);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetSpeakerMix);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetSpeakerLevels);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetSpeakerLevels);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetInputChannelMix);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetInputChannelMix);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetMute);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetMute);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetPriority);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetPriority);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetPosition);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetPosition);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetReverbProperties);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetReverbProperties);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetLowPassGain);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetLowPassGain);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetChannelGroup);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetChannelGroup);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetCallback);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DAttributes);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DAttributes);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DMinMaxDistance);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DMinMaxDistance);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DConeSettings);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DConeSettings);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DConeOrientation);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DConeOrientation);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DCustomRolloff);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DCustomRolloff);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DOcclusion);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DOcclusion);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DSpread);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DSpread);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DPanLevel);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DPanLevel);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DDopplerLevel);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DDopplerLevel);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Set3DDistanceFilter);
			FMOD_DLSYM_CLIEN(FMOD_Channel_Get3DDistanceFilter);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetDSPHead);
			FMOD_DLSYM_CLIEN(FMOD_Channel_AddDSP);
			FMOD_DLSYM_CLIEN(FMOD_Channel_IsPlaying);
			FMOD_DLSYM_CLIEN(FMOD_Channel_IsVirtual);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetAudibility);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetCurrentSound);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetSpectrum);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetWaveData);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetIndex);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetMode);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetMode);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetLoopCount);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetLoopCount);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetLoopPoints);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetLoopPoints);
			FMOD_DLSYM_CLIEN(FMOD_Channel_SetUserData);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetUserData);
			FMOD_DLSYM_CLIEN(FMOD_Channel_GetMemoryInfo);
#endif // __USE_FMOD_CHANNEL_H_

		}
		g_pFModSystem = new CFModSystem();
		g_pFModSystem->Init(SYSTEM_MAXCHANNEL, FMOD_INIT_NORMAL, 0);
	}
	void FreeFModLibrary() {
		FreeLibrary(g_hFmod);
		delete g_pFModSystem;
	}

#ifdef __USE_FMOD_SYSTEM_H_
	CFModSystem* GetSystem() {
		return g_pFModSystem;
	}
	/*
	* CFModSystem
	*/
	CFModSystem::CFModSystem(){
		Create();
	}
	CFModSystem::~CFModSystem() {
		Close();
		Release();
	}
	FMOD_RESULT CFModSystem::Create() {
		return g_pfnFMOD_System_Create(&m_pFModSystem);
	}
	FMOD_RESULT CFModSystem::Release() {
		return g_pfnFMOD_System_Release(m_pFModSystem);
	}
	FMOD_RESULT CFModSystem::SetOutput(FMOD_OUTPUTTYPE output) {
		return g_pfnFMOD_System_SetOutput(m_pFModSystem, output);
	}
	FMOD_RESULT CFModSystem::GetOutput(FMOD_OUTPUTTYPE* output) {
		return g_pfnFMOD_System_GetOutput(m_pFModSystem, output);
	}
	FMOD_RESULT CFModSystem::GetNumDrivers(int* numdrivers) {
		return g_pfnFMOD_System_GetNumDrivers(m_pFModSystem, numdrivers);
	}
	FMOD_RESULT CFModSystem::GetDriverInfo(int id, char* name, int namelen, FMOD_GUID* guid) {
		return g_pfnFMOD_System_GetDriverInfo(m_pFModSystem, id, name, namelen, guid);
	}
	FMOD_RESULT CFModSystem::GetDriverInfoW(int id, short* name, int namelen, FMOD_GUID* guid) {
		return g_pfnFMOD_System_GetDriverInfoW(m_pFModSystem, id, name, namelen, guid);
	}
	FMOD_RESULT CFModSystem::GetDriverCaps(int id, FMOD_CAPS* caps, int* controlpaneloutputrate, FMOD_SPEAKERMODE* controlpanelspeakermode) {
		return g_pfnFMOD_System_GetDriverCaps(m_pFModSystem, id, caps, controlpaneloutputrate, controlpanelspeakermode);
	}
	FMOD_RESULT CFModSystem::SetDriver(int driver) {
		return g_pfnFMOD_System_SetDriver(m_pFModSystem, driver);
	}
	FMOD_RESULT CFModSystem::GetDriver(int* driver) {
		return g_pfnFMOD_System_GetDriver(m_pFModSystem, driver);
	}
	FMOD_RESULT CFModSystem::SetHardwareChannels(int numhardwarechannels) {
		return g_pfnFMOD_System_SetHardwareChannels(m_pFModSystem, numhardwarechannels);
	}
	FMOD_RESULT CFModSystem::SetSoftwareChannels(int numsoftwarechannels) {
		return g_pfnFMOD_System_SetSoftwareChannels(m_pFModSystem, numsoftwarechannels);
	}
	FMOD_RESULT CFModSystem::GetSoftwareChannels(int* numsoftwarechannels) {
		return g_pfnFMOD_System_GetSoftwareChannels(m_pFModSystem, numsoftwarechannels);
	}
	FMOD_RESULT CFModSystem::SetSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod) {
		return g_pfnFMOD_System_SetSoftwareFormat(m_pFModSystem, samplerate, format, numoutputchannels, maxinputchannels, resamplemethod);
	}
	FMOD_RESULT CFModSystem::GetSoftwareFormat(int* samplerate, FMOD_SOUND_FORMAT* format, int* numoutputchannels, int* maxinputchannels, FMOD_DSP_RESAMPLER* resamplemethod, int* bits) {
		return g_pfnFMOD_System_GetSoftwareFormat(m_pFModSystem, samplerate, format, numoutputchannels, maxinputchannels, resamplemethod, bits);
	}
	FMOD_RESULT CFModSystem::SetDSPBufferSize(unsigned int bufferlength, int numbuffers) {
		return g_pfnFMOD_System_SetDSPBufferSize(m_pFModSystem, bufferlength, numbuffers);
	}
	FMOD_RESULT CFModSystem::GetDSPBufferSize(unsigned int* bufferlength, int* numbuffers) {
		return g_pfnFMOD_System_GetDSPBufferSize(m_pFModSystem, bufferlength, numbuffers);
	}
	FMOD_RESULT CFModSystem::SetFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, FMOD_FILE_ASYNCREADCALLBACK userasyncread, FMOD_FILE_ASYNCCANCELCALLBACK userasynccancel, int blockalign) {
		return g_pfnFMOD_System_SetFileSystem(m_pFModSystem, useropen, userclose, userread, userseek, userasyncread, userasynccancel, blockalign);
	}
	FMOD_RESULT CFModSystem::AttachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek) {
		return g_pfnFMOD_System_AttachFileSystem(m_pFModSystem, useropen, userclose, userread, userseek);
	}
	FMOD_RESULT CFModSystem::SetAdvancedSettings(FMOD_ADVANCEDSETTINGS* settings) {
		return g_pfnFMOD_System_SetAdvancedSettings(m_pFModSystem, settings);
	}
	FMOD_RESULT CFModSystem::GetAdvancedSettings(FMOD_ADVANCEDSETTINGS* settings) {
		return g_pfnFMOD_System_GetAdvancedSettings(m_pFModSystem, settings);
	}
	FMOD_RESULT CFModSystem::SetSpeakerMode(FMOD_SPEAKERMODE speakermode) {
		return g_pfnFMOD_System_SetSpeakerMode(m_pFModSystem, speakermode);
	}
	FMOD_RESULT CFModSystem::GetSpeakerMode(FMOD_SPEAKERMODE* speakermode) {
		return g_pfnFMOD_System_GetSpeakerMode(m_pFModSystem, speakermode);
	}
	FMOD_RESULT CFModSystem::SetCallback(FMOD_SYSTEM_CALLBACK callback) {
		return g_pfnFMOD_System_SetCallback(m_pFModSystem, callback);
	}
	FMOD_RESULT CFModSystem::SetPluginPath(const char* path) {
		return g_pfnFMOD_System_SetPluginPath(m_pFModSystem, path);
	}
	FMOD_RESULT CFModSystem::LoadPlugin(const char* filename, unsigned int* handle, unsigned int priority) {
		return g_pfnFMOD_System_LoadPlugin(m_pFModSystem, filename, handle, priority);
	}
	FMOD_RESULT CFModSystem::UnloadPlugin(unsigned int handle) {
		return g_pfnFMOD_System_UnloadPlugin(m_pFModSystem, handle);
	}
	FMOD_RESULT CFModSystem::GetNumPlugins(FMOD_PLUGINTYPE plugintype, int* numplugins) {
		return g_pfnFMOD_System_GetNumPlugins(m_pFModSystem, plugintype, numplugins);
	}
	FMOD_RESULT CFModSystem::GetPluginHandle(FMOD_PLUGINTYPE plugintype, int index, unsigned int* handle) {
		return g_pfnFMOD_System_GetPluginHandle(m_pFModSystem, plugintype, index, handle);
	}
	FMOD_RESULT CFModSystem::GetPluginInfo(unsigned int handle, FMOD_PLUGINTYPE* plugintype, char* name, int namelen, unsigned int* version) {
		return g_pfnFMOD_System_GetPluginInfo(m_pFModSystem, handle, plugintype, name, namelen, version);
	}
	FMOD_RESULT CFModSystem::SetOutputByPlugin(unsigned int handle) {
		return g_pfnFMOD_System_SetOutputByPlugin(m_pFModSystem, handle);
	}
	FMOD_RESULT CFModSystem::GetOutputByPlugin(unsigned int* handle) {
		return g_pfnFMOD_System_GetOutputByPlugin(m_pFModSystem, handle);
	}
	FMOD_RESULT CFModSystem::CreateDSPByPlugin(unsigned int handle, FMOD_DSP** dsp) {
		return g_pfnFMOD_System_CreateDSPByPlugin(m_pFModSystem, handle, dsp);
	}
	FMOD_RESULT CFModSystem::RegisterCodec(FMOD_CODEC_DESCRIPTION* description, unsigned int* handle, unsigned int priority) {
		return g_pfnFMOD_System_RegisterCodec(m_pFModSystem, description, handle, priority);
	}
	FMOD_RESULT CFModSystem::RegisterDSP(FMOD_DSP_DESCRIPTION* description, unsigned int* handle) {
		return g_pfnFMOD_System_RegisterDSP(m_pFModSystem, description, handle);
	}
	FMOD_RESULT CFModSystem::Init(int maxchannels, FMOD_INITFLAGS flags, void* extradriverdata) {
		return g_pfnFMOD_System_Init(m_pFModSystem, maxchannels, flags, extradriverdata);
	}
	FMOD_RESULT CFModSystem::Close() {
		return g_pfnFMOD_System_Close(m_pFModSystem);
	}
	FMOD_RESULT CFModSystem::Update() {
		return g_pfnFMOD_System_Update(m_pFModSystem);
	}
	FMOD_RESULT CFModSystem::Set3DSettings(float dopplerscale, float distancefactor, float rolloffscale) {
		return g_pfnFMOD_System_Set3DSettings(m_pFModSystem, dopplerscale, distancefactor, rolloffscale);
	}
	FMOD_RESULT CFModSystem::Get3DSettings(float* dopplerscale, float* distancefactor, float* rolloffscale) {
		return g_pfnFMOD_System_Get3DSettings(m_pFModSystem, dopplerscale, distancefactor, rolloffscale);
	}
	FMOD_RESULT CFModSystem::Set3DNumListeners(int numlisteners) {
		return g_pfnFMOD_System_Set3DNumListeners(m_pFModSystem, numlisteners);
	}
	FMOD_RESULT CFModSystem::Get3DNumListeners(int* numlisteners) {
		return g_pfnFMOD_System_Get3DNumListeners(m_pFModSystem, numlisteners);
	}
	FMOD_RESULT CFModSystem::Set3DListenerAttributes(int listener, const FMOD_VECTOR* pos, const FMOD_VECTOR* vel, const FMOD_VECTOR* forward, const FMOD_VECTOR* up) {
		return g_pfnFMOD_System_Set3DListenerAttributes(m_pFModSystem, listener, pos, vel, forward, up);
	}
	FMOD_RESULT CFModSystem::Get3DListenerAttributes(int listener, FMOD_VECTOR* pos, FMOD_VECTOR* vel, FMOD_VECTOR* forward, FMOD_VECTOR* up) {
		return g_pfnFMOD_System_Get3DListenerAttributes(m_pFModSystem, listener, pos, vel, forward, up);
	}
	FMOD_RESULT CFModSystem::Set3DRolloffCallback(FMOD_3D_ROLLOFFCALLBACK callback) {
		return g_pfnFMOD_System_Set3DRolloffCallback(m_pFModSystem, callback);
	}
	FMOD_RESULT CFModSystem::Set3DSpeakerPosition(FMOD_SPEAKER speaker, float x, float y, FMOD_BOOL active) {
		return g_pfnFMOD_System_Set3DSpeakerPosition(m_pFModSystem, speaker, x, y, active);
	}
	FMOD_RESULT CFModSystem::Get3DSpeakerPosition(FMOD_SPEAKER speaker, float* x, float* y, FMOD_BOOL* active) {
		return g_pfnFMOD_System_Get3DSpeakerPosition(m_pFModSystem, speaker, x, y, active);
	}
	FMOD_RESULT CFModSystem::SetStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype) {
		return g_pfnFMOD_System_SetStreamBufferSize(m_pFModSystem, filebuffersize, filebuffersizetype);
	}
	FMOD_RESULT CFModSystem::GetStreamBufferSize(unsigned int* filebuffersize, FMOD_TIMEUNIT* filebuffersizetype) {
		return g_pfnFMOD_System_GetStreamBufferSize(m_pFModSystem, filebuffersize, filebuffersizetype);
	}
	FMOD_RESULT CFModSystem::GetVersion(unsigned int* version) {
		return g_pfnFMOD_System_GetVersion(m_pFModSystem, version);
	}
	FMOD_RESULT CFModSystem::GetOutputHandle(void** handle) {
		return g_pfnFMOD_System_GetOutputHandle(m_pFModSystem, handle);
	}
	FMOD_RESULT CFModSystem::GetChannelsPlaying(int* channels) {
		return g_pfnFMOD_System_GetChannelsPlaying(m_pFModSystem, channels);
	}
	FMOD_RESULT CFModSystem::GetHardwareChannels(int* numhardwarechannels) {
		return g_pfnFMOD_System_GetHardwareChannels(m_pFModSystem, numhardwarechannels);
	}
	FMOD_RESULT CFModSystem::GetCPUUsage(float* dsp, float* stream, float* geometry, float* update, float* total) {
		return g_pfnFMOD_System_GetCPUUsage(m_pFModSystem, dsp, stream, geometry, update, total);
	}
	FMOD_RESULT CFModSystem::GetSoundRAM(int* currentalloced, int* maxalloced, int* total) {
		return g_pfnFMOD_System_GetSoundRAM(m_pFModSystem, currentalloced, maxalloced, total);
	}
	FMOD_RESULT CFModSystem::GetNumCDROMDrives(int* numdrives) {
		return g_pfnFMOD_System_GetNumCDROMDrives(m_pFModSystem, numdrives);
	}
	FMOD_RESULT CFModSystem::GetCDROMDriveName(int drive, char* drivename, int drivenamelen, char* scsiname, int scsinamelen, char* devicename, int devicenamelen) {
		return g_pfnFMOD_System_GetCDROMDriveName(m_pFModSystem, drive, drivename, drivenamelen, scsiname, scsinamelen, devicename, devicenamelen);
	}
	FMOD_RESULT CFModSystem::GetSpectrum(float* spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype) {
		return g_pfnFMOD_System_GetSpectrum(m_pFModSystem, spectrumarray, numvalues, channeloffset, windowtype);
	}
	FMOD_RESULT CFModSystem::GetWaveData(float* wavearray, int numvalues, int channeloffset) {
		return g_pfnFMOD_System_GetWaveData(m_pFModSystem, wavearray, numvalues, channeloffset);
	}
	FMOD_RESULT CFModSystem::CreateSound(const char* name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo, FMOD_SOUND** sound) {
		return g_pfnFMOD_System_CreateSound(m_pFModSystem, name_or_data, mode, exinfo, sound);
	}
	FMOD_RESULT CFModSystem::CreateStream(const char* name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo, FMOD_SOUND** sound) {
		return g_pfnFMOD_System_CreateStream(m_pFModSystem, name_or_data, mode, exinfo, sound);
	}
	FMOD_RESULT CFModSystem::CreateDSP(FMOD_DSP_DESCRIPTION* description, FMOD_DSP** dsp) {
		return g_pfnFMOD_System_CreateDSP(m_pFModSystem, description, dsp);
	}
	FMOD_RESULT CFModSystem::CreateDSPByType(FMOD_DSP_TYPE type, FMOD_DSP** dsp) {
		return g_pfnFMOD_System_CreateDSPByType(m_pFModSystem, type, dsp);
	}
	FMOD_RESULT CFModSystem::CreateChannelGroup(const char* name, FMOD_CHANNELGROUP** channelgroup) {
		return g_pfnFMOD_System_CreateChannelGroup(m_pFModSystem, name, channelgroup);
	}
	FMOD_RESULT CFModSystem::CreateSoundGroup(const char* name, FMOD_SOUNDGROUP** soundgroup) {
		return g_pfnFMOD_System_CreateSoundGroup(m_pFModSystem, name, soundgroup);
	}
	FMOD_RESULT CFModSystem::CreateReverb(FMOD_REVERB** reverb) {
		return g_pfnFMOD_System_CreateReverb(m_pFModSystem, reverb);
	}
	FMOD_RESULT CFModSystem::PlaySound(FMOD_CHANNELINDEX channelid, FMOD_SOUND* sound, FMOD_BOOL paused, FMOD_CHANNEL** channel) {
		return g_pfnFMOD_System_PlaySound(m_pFModSystem, channelid, sound, paused, channel);
	}
	FMOD_RESULT CFModSystem::PlayDSP(FMOD_CHANNELINDEX channelid, FMOD_DSP* dsp, FMOD_BOOL paused, FMOD_CHANNEL** channel) {
		return g_pfnFMOD_System_PlayDSP(m_pFModSystem, channelid, dsp, paused, channel);
	}
	FMOD_RESULT CFModSystem::GetChannel(int channelid, FMOD_CHANNEL** channel) {
		return g_pfnFMOD_System_GetChannel(m_pFModSystem, channelid, channel);
	}
	FMOD_RESULT CFModSystem::GetMasterChannelGroup(FMOD_CHANNELGROUP** channelgroup) {
		return g_pfnFMOD_System_GetMasterChannelGroup(m_pFModSystem, channelgroup);
	}
	FMOD_RESULT CFModSystem::GetMasterSoundGroup(FMOD_SOUNDGROUP** soundgroup) {
		return g_pfnFMOD_System_GetMasterSoundGroup(m_pFModSystem, soundgroup);
	}
	FMOD_RESULT CFModSystem::SetReverbProperties(const FMOD_REVERB_PROPERTIES* prop) {
		return g_pfnFMOD_System_SetReverbProperties(m_pFModSystem, prop);
	}
	FMOD_RESULT CFModSystem::GetReverbProperties(FMOD_REVERB_PROPERTIES* prop) {
		return g_pfnFMOD_System_GetReverbProperties(m_pFModSystem, prop);
	}
	FMOD_RESULT CFModSystem::SetReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop) {
		return g_pfnFMOD_System_SetReverbAmbientProperties(m_pFModSystem, prop);
	}
	FMOD_RESULT CFModSystem::GetReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop) {
		return g_pfnFMOD_System_GetReverbAmbientProperties(m_pFModSystem, prop);
	}
	FMOD_RESULT CFModSystem::GetDSPHead(FMOD_DSP** dsp) {
		return g_pfnFMOD_System_GetDSPHead(m_pFModSystem, dsp);
	}
	FMOD_RESULT CFModSystem::AddDSP(FMOD_DSP* dsp, FMOD_DSPCONNECTION** connection) {
		return g_pfnFMOD_System_AddDSP(m_pFModSystem, dsp, connection);
	}
	FMOD_RESULT CFModSystem::LockDSP() {
		return g_pfnFMOD_System_LockDSP(m_pFModSystem);
	}
	FMOD_RESULT CFModSystem::UnlockDSP() {
		return g_pfnFMOD_System_UnlockDSP(m_pFModSystem);
	}
	FMOD_RESULT CFModSystem::GetDSPClock(unsigned int* hi, unsigned int* lo) {
		return g_pfnFMOD_System_GetDSPClock(m_pFModSystem, hi, lo);
	}
	FMOD_RESULT CFModSystem::GetRecordNumDrivers(int* numdrivers) {
		return g_pfnFMOD_System_GetRecordNumDrivers(m_pFModSystem, numdrivers);
	}
	FMOD_RESULT CFModSystem::GetRecordDriverInfo(int id, char* name, int namelen, FMOD_GUID* guid) {
		return g_pfnFMOD_System_GetRecordDriverInfo(m_pFModSystem, id, name, namelen, guid);
	}
	FMOD_RESULT CFModSystem::GetRecordDriverInfoW(int id, short* name, int namelen, FMOD_GUID* guid) {
		return g_pfnFMOD_System_GetRecordDriverInfoW(m_pFModSystem, id, name, namelen, guid);
	}
	FMOD_RESULT CFModSystem::GetRecordDriverCaps(int id, FMOD_CAPS* caps, int* minfrequency, int* maxfrequency) {
		return g_pfnFMOD_System_GetRecordDriverCaps(m_pFModSystem, id, caps, minfrequency, maxfrequency);
	}
	FMOD_RESULT CFModSystem::GetRecordPosition(int id, unsigned int* position) {
		return g_pfnFMOD_System_GetRecordPosition(m_pFModSystem, id, position);
	}
	FMOD_RESULT CFModSystem::RecordStart(int id, FMOD_SOUND* sound, FMOD_BOOL loop) {
		return g_pfnFMOD_System_RecordStart(m_pFModSystem, id, sound, loop);
	}
	FMOD_RESULT CFModSystem::RecordStop(int id) {
		return g_pfnFMOD_System_RecordStop(m_pFModSystem, id);
	}
	FMOD_RESULT CFModSystem::IsRecording(int id, FMOD_BOOL* recording) {
		return g_pfnFMOD_System_IsRecording(m_pFModSystem, id, recording);
	}
	FMOD_RESULT CFModSystem::CreateGeometry(int maxpolygons, int maxvertices, FMOD_GEOMETRY** geometry) {
		return g_pfnFMOD_System_CreateGeometry(m_pFModSystem, maxpolygons, maxvertices, geometry);
	}
	FMOD_RESULT CFModSystem::SetGeometrySettings(float maxworldsize) {
		return g_pfnFMOD_System_SetGeometrySettings(m_pFModSystem, maxworldsize);
	}
	FMOD_RESULT CFModSystem::GetGeometrySettings(float* maxworldsize) {
		return g_pfnFMOD_System_GetGeometrySettings(m_pFModSystem, maxworldsize);
	}
	FMOD_RESULT CFModSystem::LoadGeometry(const void* data, int datasize, FMOD_GEOMETRY** geometry) {
		return g_pfnFMOD_System_LoadGeometry(m_pFModSystem, data, datasize, geometry);
	}
	FMOD_RESULT CFModSystem::GetGeometryOcclusion(const FMOD_VECTOR* listener, const FMOD_VECTOR* source, float* direct, float* reverb) {
		return g_pfnFMOD_System_GetGeometryOcclusion(m_pFModSystem, listener, source, direct, reverb);
	}
	FMOD_RESULT CFModSystem::SetNetworkProxy(const char* proxy) {
		return g_pfnFMOD_System_SetNetworkProxy(m_pFModSystem, proxy);
	}
	FMOD_RESULT CFModSystem::GetNetworkProxy(char* proxy, int proxylen) {
		return g_pfnFMOD_System_GetNetworkProxy(m_pFModSystem, proxy, proxylen);
	}
	FMOD_RESULT CFModSystem::SetNetworkTimeout(int timeout) {
		return g_pfnFMOD_System_SetNetworkTimeout(m_pFModSystem, timeout);
	}
	FMOD_RESULT CFModSystem::GetNetworkTimeout(int* timeout) {
		return g_pfnFMOD_System_GetNetworkTimeout(m_pFModSystem, timeout);
	}
	FMOD_RESULT CFModSystem::SetUserData(void* userdata) {
		return g_pfnFMOD_System_SetUserData(m_pFModSystem, userdata);
	}
	FMOD_RESULT CFModSystem::GetUserData(void** userdata) {
		return g_pfnFMOD_System_GetUserData(m_pFModSystem, userdata);
	}
	FMOD_RESULT CFModSystem::GetMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int* memoryused, FMOD_MEMORY_USAGE_DETAILS* memoryused_details) {
		return g_pfnFMOD_System_GetMemoryInfo(m_pFModSystem, memorybits, event_memorybits, memoryused, memoryused_details);
	}
	CFModSystem::operator FMOD_SYSTEM* (){
		return m_pFModSystem;
	}
#endif
#ifdef __USE_FMOD_SOUND_H_
	CFModSound::CFModSound(){}
	CFModSound::CFModSound(FMOD_SOUND* sound){
		Set(sound);
	}
	CFModSound::~CFModSound(){
		Release();
	}
	/*
	Sound' API
	 */
	FMOD_RESULT CFModSound::Release() {
		m_pFModSound = nullptr;
		return g_pfnFMOD_Sound_Release(m_pFModSound);
	}
	FMOD_RESULT CFModSound::GetSystemObject(FMOD_SYSTEM** system) {
		return g_pfnFMOD_Sound_GetSystemObject(m_pFModSound, system);
	}
	FMOD_RESULT CFModSound::Lock(unsigned int offset, unsigned int length, void** ptr1, void** ptr2, unsigned int* len1, unsigned int* len2) {
		return g_pfnFMOD_Sound_Lock(m_pFModSound, offset, length, ptr1, ptr2, len1, len2);
	}
	FMOD_RESULT CFModSound::Unlock(void* ptr1, void* ptr2, unsigned int len1, unsigned int len2) {
		return g_pfnFMOD_Sound_Unlock(m_pFModSound, ptr1, ptr2, len1, len2);
	}
	FMOD_RESULT  CFModSound::SetDefaults(float frequency, float volume, float pan, int priority) {
		return g_pfnFMOD_Sound_SetDefaults(m_pFModSound, frequency, volume, pan, priority);
	}
	FMOD_RESULT  CFModSound::GetDefaults(float* frequency, float* volume, float* pan, int* priority) {
		return g_pfnFMOD_Sound_GetDefaults(m_pFModSound, frequency, volume, pan, priority);
	}
	FMOD_RESULT  CFModSound::SetVariations(float frequencyvar, float volumevar, float panvar) {
		return g_pfnFMOD_Sound_SetVariations(m_pFModSound, frequencyvar, volumevar, panvar);
	}
	FMOD_RESULT  CFModSound::GetVariations(float* frequencyvar, float* volumevar, float* panvar) {
		return g_pfnFMOD_Sound_GetVariations(m_pFModSound, frequencyvar, volumevar, panvar);
	}
	FMOD_RESULT  CFModSound::Set3DMinMaxDistance(float min, float max) {
		return g_pfnFMOD_Sound_Set3DMinMaxDistance(m_pFModSound, min, max);
	}
	FMOD_RESULT  CFModSound::Get3DMinMaxDistance(float* min, float* max) {
		return g_pfnFMOD_Sound_Get3DMinMaxDistance(m_pFModSound, min, max);
	}
	FMOD_RESULT  CFModSound::Set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume) {
		return g_pfnFMOD_Sound_Set3DConeSettings(m_pFModSound, insideconeangle, outsideconeangle, outsidevolume);
	}
	FMOD_RESULT  CFModSound::Get3DConeSettings(float* insideconeangle, float* outsideconeangle, float* outsidevolume) {
		return g_pfnFMOD_Sound_Get3DConeSettings(m_pFModSound, insideconeangle, outsideconeangle, outsidevolume);
	}
	FMOD_RESULT  CFModSound::Set3DCustomRolloff(FMOD_VECTOR* points, int numpoints) {
		return g_pfnFMOD_Sound_Set3DCustomRolloff(m_pFModSound, points, numpoints);
	}
	FMOD_RESULT  CFModSound::Get3DCustomRolloff(FMOD_VECTOR** points, int* numpoints) {
		return g_pfnFMOD_Sound_Get3DCustomRolloff(m_pFModSound, points, numpoints);
	}
	FMOD_RESULT  CFModSound::SetSubSound(int index, FMOD_SOUND* subsound) {
		return g_pfnFMOD_Sound_SetSubSound(m_pFModSound, index, subsound);
	}
	FMOD_RESULT  CFModSound::GetSubSound(int index, FMOD_SOUND** subsound) {
		return g_pfnFMOD_Sound_GetSubSound(m_pFModSound, index, subsound);
	}
	FMOD_RESULT  CFModSound::GetSubSoundParent(FMOD_SOUND** parentsound) {
		return g_pfnFMOD_Sound_GetSubSoundParent(m_pFModSound, parentsound);
	}
	FMOD_RESULT  CFModSound::SetSubSoundSentence(int* subsoundlist, int numsubsounds) {
		return g_pfnFMOD_Sound_SetSubSoundSentence(m_pFModSound, subsoundlist, numsubsounds);
	}
	FMOD_RESULT  CFModSound::GetName(char* name, int namelen) {
		return g_pfnFMOD_Sound_GetName(m_pFModSound, name, namelen);
	}
	FMOD_RESULT  CFModSound::GetLength(unsigned int* length, FMOD_TIMEUNIT lengthtype) {
		return g_pfnFMOD_Sound_GetLength(m_pFModSound, length, lengthtype);
	}
	FMOD_RESULT  CFModSound::GetFormat(int* type, int* format, int* channels, int* bits) {
		return g_pfnFMOD_Sound_GetFormat(m_pFModSound, (FMOD_SOUND_TYPE*)type, (FMOD_SOUND_FORMAT*)format, channels, bits);
	}
	FMOD_RESULT  CFModSound::GetNumSubSounds(int* numsubsounds) {
		return g_pfnFMOD_Sound_GetNumSubSounds(m_pFModSound, numsubsounds);
	}
	FMOD_RESULT  CFModSound::GetNumTags(int* numtags, int* numtagsupdated) {
		return g_pfnFMOD_Sound_GetNumTags(m_pFModSound, numtags, numtagsupdated);
	}
	FMOD_RESULT  CFModSound::GetTag(const char* name, int index, FMOD_TAG* tag) {
		return g_pfnFMOD_Sound_GetTag(m_pFModSound, name, index, tag);
	}
	FMOD_RESULT  CFModSound::GetOpenState(FMOD_OPENSTATE* openstate, unsigned int* percentbuffered, FMOD_BOOL* starving, FMOD_BOOL* diskbusy) {
		return g_pfnFMOD_Sound_GetOpenState(m_pFModSound, openstate, percentbuffered, starving, diskbusy);
	}
	FMOD_RESULT  CFModSound::ReadData(void* buffer, unsigned int lenbytes, unsigned int* read) {
		return g_pfnFMOD_Sound_ReadData(m_pFModSound, buffer, lenbytes, read);
	}
	FMOD_RESULT  CFModSound::SeekData(unsigned int pcm) {
		return g_pfnFMOD_Sound_SeekData(m_pFModSound, pcm);
	}
	FMOD_RESULT  CFModSound::SetSoundGroup(FMOD_SOUNDGROUP* soundgroup) {
		return g_pfnFMOD_Sound_SetSoundGroup(m_pFModSound, soundgroup);
	}
	FMOD_RESULT  CFModSound::GetSoundGroup(FMOD_SOUNDGROUP** soundgroup) {
		return g_pfnFMOD_Sound_GetSoundGroup(m_pFModSound, soundgroup);
	}
	FMOD_RESULT  CFModSound::GetNumSyncPoints(int* numsyncpoints) {
		return g_pfnFMOD_Sound_GetNumSyncPoints(m_pFModSound, numsyncpoints);
	}
	FMOD_RESULT  CFModSound::GetSyncPoint(int index, FMOD_SYNCPOINT** point) {
		return g_pfnFMOD_Sound_GetSyncPoint(m_pFModSound, index, point);
	}
	FMOD_RESULT  CFModSound::GetSyncPointInfo(FMOD_SYNCPOINT* point, char* name, int namelen, unsigned int* offset, FMOD_TIMEUNIT offsettype) {
		return g_pfnFMOD_Sound_GetSyncPointInfo(m_pFModSound, point, name, namelen, offset, offsettype);
	}
	FMOD_RESULT  CFModSound::AddSyncPoint(unsigned int offset, FMOD_TIMEUNIT offsettype, const char* name, FMOD_SYNCPOINT** point) {
		return g_pfnFMOD_Sound_AddSyncPoint(m_pFModSound, offset, offsettype, name, point);
	}
	FMOD_RESULT  CFModSound::DeleteSyncPoint(FMOD_SYNCPOINT* point) {
		return g_pfnFMOD_Sound_DeleteSyncPoint(m_pFModSound, point);
	}
	FMOD_RESULT  CFModSound::SetMode(FMOD_MODE mode) {
		return g_pfnFMOD_Sound_SetMode(m_pFModSound, mode);
	}
	FMOD_RESULT  CFModSound::GetMode(FMOD_MODE* mode) {
		return g_pfnFMOD_Sound_GetMode(m_pFModSound, mode);
	}
	FMOD_RESULT  CFModSound::SetLoopCount(int loopcount) {
		return g_pfnFMOD_Sound_SetLoopCount(m_pFModSound, loopcount);
	}
	FMOD_RESULT  CFModSound::GetLoopCount(int* loopcount) {
		return g_pfnFMOD_Sound_GetLoopCount(m_pFModSound, loopcount);
	}
	FMOD_RESULT  CFModSound::SetLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype) {
		return g_pfnFMOD_Sound_SetLoopPoints(m_pFModSound, loopstart, loopstarttype, loopend, loopendtype);
	}
	FMOD_RESULT  CFModSound::GetLoopPoints(unsigned int* loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int* loopend, FMOD_TIMEUNIT loopendtype) {
		return g_pfnFMOD_Sound_GetLoopPoints(m_pFModSound, loopstart, loopstarttype, loopend, loopendtype);
	}
	FMOD_RESULT  CFModSound::GetMusicNumChannels(int* numchannels) {
		return g_pfnFMOD_Sound_GetMusicNumChannels(m_pFModSound, numchannels);
	}
	FMOD_RESULT  CFModSound::SetMusicChannelVolume(int channel, float volume) {
		return g_pfnFMOD_Sound_SetMusicChannelVolume(m_pFModSound, channel, volume);
	}
	FMOD_RESULT  CFModSound::GetMusicChannelVolume(int channel, float* volume) {
		return g_pfnFMOD_Sound_GetMusicChannelVolume(m_pFModSound, channel, volume);
	}
	FMOD_RESULT  CFModSound::SetMusicSpeed(float speed) {
		return g_pfnFMOD_Sound_SetMusicSpeed(m_pFModSound, speed);
	}
	FMOD_RESULT  CFModSound::GetMusicSpeed(float* speed) {
		return g_pfnFMOD_Sound_GetMusicSpeed(m_pFModSound, speed);
	}
	FMOD_RESULT  CFModSound::SetUserData(void* userdata) {
		return g_pfnFMOD_Sound_SetUserData(m_pFModSound, userdata);
	}
	FMOD_RESULT  CFModSound::GetUserData(void** userdata) {
		return g_pfnFMOD_Sound_GetUserData(m_pFModSound, userdata);
	}
	FMOD_RESULT  CFModSound::GetMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int* memoryused, FMOD_MEMORY_USAGE_DETAILS* memoryused_details) {
		return g_pfnFMOD_Sound_GetMemoryInfo(m_pFModSound, memorybits, event_memorybits, memoryused, memoryused_details);
	}
	FMOD_BOOL CFModSound::Valid(){
		return m_pFModSound != nullptr;
	}
	void CFModSound::Set(FMOD_SOUND* sound){
		m_pFModSound = sound;
	}
	CFModSound::operator FMOD_SOUND* (){
		return m_pFModSound;
	}
#endif
#ifdef __USE_FMOD_CHANNEL_H_
	CFModChannel::CFModChannel(){}
	CFModChannel::CFModChannel(FMOD_CHANNEL* channel){
		Set(channel);
	}
	CFModChannel::~CFModChannel(){
		Stop();
	}
	FMOD_RESULT CFModChannel::GetSystemObject(FMOD_SYSTEM** system) {
		return g_pfnFMOD_Channel_GetSystemObject(m_pFModChennel, system);
	}
	FMOD_RESULT  CFModChannel::Stop() {
		m_pFModChennel = nullptr;
		return g_pfnFMOD_Channel_Stop(m_pFModChennel);
	}
	FMOD_RESULT  CFModChannel::SetPaused(FMOD_BOOL paused) {
		return g_pfnFMOD_Channel_SetPaused(m_pFModChennel, paused);
	}
	FMOD_RESULT  CFModChannel::GetPaused(FMOD_BOOL* paused) {
		return g_pfnFMOD_Channel_GetPaused(m_pFModChennel, paused);
	}
	FMOD_RESULT  CFModChannel::SetVolume(float volume) {
		return g_pfnFMOD_Channel_SetVolume(m_pFModChennel, volume);
	}
	FMOD_RESULT  CFModChannel::GetVolume(float* volume) {
		return g_pfnFMOD_Channel_GetVolume(m_pFModChennel, volume);
	}
	FMOD_RESULT  CFModChannel::SetFrequency(float frequency) {
		return g_pfnFMOD_Channel_SetFrequency(m_pFModChennel, frequency);
	}
	FMOD_RESULT  CFModChannel::GetFrequency(float* frequency) {
		return g_pfnFMOD_Channel_GetFrequency(m_pFModChennel, frequency);
	}
	FMOD_RESULT  CFModChannel::SetPan(float pan) {
		return g_pfnFMOD_Channel_SetPan(m_pFModChennel, pan);
	}
	FMOD_RESULT  CFModChannel::GetPan(float* pan) {
		return g_pfnFMOD_Channel_GetPan(m_pFModChennel, pan);
	}
	FMOD_RESULT  CFModChannel::SetDelay(FMOD_DELAYTYPE delaytype, unsigned int delayhi, unsigned int delaylo) {
		return g_pfnFMOD_Channel_SetDelay(m_pFModChennel, delaytype, delayhi, delaylo);
	}
	FMOD_RESULT  CFModChannel::GetDelay(FMOD_DELAYTYPE delaytype, unsigned int* delayhi, unsigned int* delaylo) {
		return g_pfnFMOD_Channel_GetDelay(m_pFModChennel, delaytype, delayhi, delaylo);
	}
	FMOD_RESULT  CFModChannel::SetSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright) {
		return g_pfnFMOD_Channel_SetSpeakerMix(m_pFModChennel, frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
	}
	FMOD_RESULT  CFModChannel::GetSpeakerMix(float* frontleft, float* frontright, float* center, float* lfe, float* backleft, float* backright, float* sideleft, float* sideright) {
		return g_pfnFMOD_Channel_GetSpeakerMix(m_pFModChennel, frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
	}
	FMOD_RESULT  CFModChannel::SetSpeakerLevels(FMOD_SPEAKER speaker, float* levels, int numlevels) {
		return g_pfnFMOD_Channel_SetSpeakerLevels(m_pFModChennel, speaker, levels, numlevels);
	}
	FMOD_RESULT  CFModChannel::GetSpeakerLevels(FMOD_SPEAKER speaker, float* levels, int numlevels) {
		return g_pfnFMOD_Channel_GetSpeakerLevels(m_pFModChennel, speaker, levels, numlevels);
	}
	FMOD_RESULT  CFModChannel::SetInputChannelMix(float* levels, int numlevels) {
		return g_pfnFMOD_Channel_SetInputChannelMix(m_pFModChennel, levels, numlevels);
	}
	FMOD_RESULT  CFModChannel::GetInputChannelMix(float* levels, int numlevels) {
		return g_pfnFMOD_Channel_GetInputChannelMix(m_pFModChennel, levels, numlevels);
	}
	FMOD_RESULT  CFModChannel::SetMute(FMOD_BOOL mute) {
		return g_pfnFMOD_Channel_SetMute(m_pFModChennel, mute);
	}
	FMOD_RESULT  CFModChannel::GetMute(FMOD_BOOL* mute) {
		return g_pfnFMOD_Channel_GetMute(m_pFModChennel, mute);
	}
	FMOD_RESULT  CFModChannel::SetPriority(int priority) {
		return g_pfnFMOD_Channel_SetPriority(m_pFModChennel, priority);
	}
	FMOD_RESULT  CFModChannel::GetPriority(int* priority) {
		return g_pfnFMOD_Channel_GetPriority(m_pFModChennel, priority);
	}
	FMOD_RESULT  CFModChannel::SetPosition(unsigned int position, FMOD_TIMEUNIT postype) {
		return g_pfnFMOD_Channel_SetPosition(m_pFModChennel, position, postype);
	}
	FMOD_RESULT  CFModChannel::GetPosition(unsigned int* position, FMOD_TIMEUNIT postype) {
		return g_pfnFMOD_Channel_GetPosition(m_pFModChennel, position, postype);
	}
	FMOD_RESULT  CFModChannel::SetReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES* prop) {
		return g_pfnFMOD_Channel_SetReverbProperties(m_pFModChennel, prop);
	}
	FMOD_RESULT  CFModChannel::GetReverbProperties(FMOD_REVERB_CHANNELPROPERTIES* prop) {
		return g_pfnFMOD_Channel_GetReverbProperties(m_pFModChennel, prop);
	}
	FMOD_RESULT  CFModChannel::SetLowPassGain(float gain) {
		return g_pfnFMOD_Channel_SetLowPassGain(m_pFModChennel, gain);
	}
	FMOD_RESULT  CFModChannel::GetLowPassGain(float* gain) {
		return g_pfnFMOD_Channel_GetLowPassGain(m_pFModChennel, gain);
	}
	FMOD_RESULT  CFModChannel::SetChannelGroup(FMOD_CHANNELGROUP* channelgroup) {
		return g_pfnFMOD_Channel_SetChannelGroup(m_pFModChennel, channelgroup);
	}
	FMOD_RESULT  CFModChannel::GetChannelGroup(FMOD_CHANNELGROUP** channelgroup) {
		return g_pfnFMOD_Channel_GetChannelGroup(m_pFModChennel, channelgroup);
	}
	FMOD_RESULT  CFModChannel::SetCallback(FMOD_CHANNEL_CALLBACK callback) {
		return g_pfnFMOD_Channel_SetCallback(m_pFModChennel, callback);
	}
	FMOD_RESULT  CFModChannel::Set3DAttributes(const FMOD_VECTOR* pos, const FMOD_VECTOR* vel) {
		return g_pfnFMOD_Channel_Set3DAttributes(m_pFModChennel, pos, vel);
	}
	FMOD_RESULT  CFModChannel::Get3DAttributes(FMOD_VECTOR* pos, FMOD_VECTOR* vel) {
		return g_pfnFMOD_Channel_Get3DAttributes(m_pFModChennel, pos, vel);
	}
	FMOD_RESULT  CFModChannel::Set3DMinMaxDistance(float mindistance, float maxdistance) {
		return g_pfnFMOD_Channel_Set3DMinMaxDistance(m_pFModChennel, mindistance, maxdistance);
	}
	FMOD_RESULT  CFModChannel::Get3DMinMaxDistance(float* mindistance, float* maxdistance) {
		return g_pfnFMOD_Channel_Get3DMinMaxDistance(m_pFModChennel, mindistance, maxdistance);
	}
	FMOD_RESULT  CFModChannel::Set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume) {
		return g_pfnFMOD_Channel_Set3DConeSettings(m_pFModChennel, insideconeangle, outsideconeangle, outsidevolume);
	}
	FMOD_RESULT  CFModChannel::Get3DConeSettings(float* insideconeangle, float* outsideconeangle, float* outsidevolume) {
		return g_pfnFMOD_Channel_Get3DConeSettings(m_pFModChennel, insideconeangle, outsideconeangle, outsidevolume);
	}
	FMOD_RESULT  CFModChannel::Set3DConeOrientation(FMOD_VECTOR* orientation) {
		return g_pfnFMOD_Channel_Set3DConeOrientation(m_pFModChennel, orientation);
	}
	FMOD_RESULT  CFModChannel::Get3DConeOrientation(FMOD_VECTOR* orientation) {
		return g_pfnFMOD_Channel_Get3DConeOrientation(m_pFModChennel, orientation);
	}
	FMOD_RESULT CFModChannel::Set3DCustomRolloff(FMOD_VECTOR* points, int numpoints) {
		return g_pfnFMOD_Channel_Set3DCustomRolloff(m_pFModChennel, points, numpoints);
	}
	FMOD_RESULT  CFModChannel::Get3DCustomRolloff(FMOD_VECTOR** points, int* numpoints) {
		return g_pfnFMOD_Channel_Get3DCustomRolloff(m_pFModChennel, points, numpoints);
	}
	FMOD_RESULT  CFModChannel::Set3DOcclusion(float directocclusion, float reverbocclusion) {
		return g_pfnFMOD_Channel_Set3DOcclusion(m_pFModChennel, directocclusion, reverbocclusion);
	}
	FMOD_RESULT  CFModChannel::Get3DOcclusion(float* directocclusion, float* reverbocclusion) {
		return g_pfnFMOD_Channel_Get3DOcclusion(m_pFModChennel, directocclusion, reverbocclusion);
	}
	FMOD_RESULT  CFModChannel::Set3DSpread(float angle) {
		return g_pfnFMOD_Channel_Set3DSpread(m_pFModChennel, angle);
	}
	FMOD_RESULT  CFModChannel::Get3DSpread(float* angle) {
		return g_pfnFMOD_Channel_Get3DSpread(m_pFModChennel, angle);
	}
	FMOD_RESULT  CFModChannel::Set3DPanLevel(float level) {
		return g_pfnFMOD_Channel_Set3DPanLevel(m_pFModChennel, level);
	}
	FMOD_RESULT  CFModChannel::Get3DPanLevel(float* level) {
		return g_pfnFMOD_Channel_Get3DPanLevel(m_pFModChennel, level);
	}
	FMOD_RESULT  CFModChannel::Set3DDopplerLevel(float level) {
		return g_pfnFMOD_Channel_Set3DDopplerLevel(m_pFModChennel, level);
	}
	FMOD_RESULT  CFModChannel::Get3DDopplerLevel(float* level) {
		return g_pfnFMOD_Channel_Get3DDopplerLevel(m_pFModChennel, level);
	}
	FMOD_RESULT  CFModChannel::Set3DDistanceFilter(FMOD_BOOL custom, float customLevel, float centerFreq) {
		return g_pfnFMOD_Channel_Set3DDistanceFilter(m_pFModChennel, custom, customLevel, centerFreq);
	}
	FMOD_RESULT  CFModChannel::Get3DDistanceFilter(FMOD_BOOL* custom, float* customLevel, float* centerFreq) {
		return g_pfnFMOD_Channel_Get3DDistanceFilter(m_pFModChennel, custom, customLevel, centerFreq);
	}
	FMOD_RESULT  CFModChannel::GetDSPHead(FMOD_DSP** dsp) {
		return g_pfnFMOD_Channel_GetDSPHead(m_pFModChennel, dsp);
	}
	FMOD_RESULT  CFModChannel::AddDSP(FMOD_DSP* dsp, FMOD_DSPCONNECTION** connection) {
		return g_pfnFMOD_Channel_AddDSP(m_pFModChennel, dsp, connection);
	}
	FMOD_RESULT  CFModChannel::IsPlaying(FMOD_BOOL* isplaying) {
		return g_pfnFMOD_Channel_IsPlaying(m_pFModChennel, isplaying);
	}
	FMOD_RESULT  CFModChannel::IsVirtual(FMOD_BOOL* isvirtual) {
		return g_pfnFMOD_Channel_IsVirtual(m_pFModChennel, isvirtual);
	}
	FMOD_RESULT  CFModChannel::GetAudibility(float* audibility) {
		return g_pfnFMOD_Channel_GetAudibility(m_pFModChennel, audibility);
	}
	FMOD_RESULT  CFModChannel::GetCurrentSound(FMOD_SOUND** sound) {
		return g_pfnFMOD_Channel_GetCurrentSound(m_pFModChennel, sound);
	}
	FMOD_RESULT  CFModChannel::GetSpectrum(float* spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype) {
		return g_pfnFMOD_Channel_GetSpectrum(m_pFModChennel, spectrumarray, numvalues, channeloffset, windowtype);
	}
	FMOD_RESULT  CFModChannel::GetWaveData(float* wavearray, int numvalues, int channeloffset) {
		return g_pfnFMOD_Channel_GetWaveData(m_pFModChennel, wavearray, numvalues, channeloffset);
	}
	FMOD_RESULT  CFModChannel::GetIndex(int* index) {
		return g_pfnFMOD_Channel_GetIndex(m_pFModChennel, index);
	}
	FMOD_RESULT  CFModChannel::SetMode(FMOD_MODE mode) {
		return g_pfnFMOD_Channel_SetMode(m_pFModChennel, mode);
	}
	FMOD_RESULT  CFModChannel::GetMode(FMOD_MODE* mode) {
		return g_pfnFMOD_Channel_GetMode(m_pFModChennel, mode);
	}
	FMOD_RESULT  CFModChannel::SetLoopCount(int loopcount) {
		return g_pfnFMOD_Channel_SetLoopCount(m_pFModChennel, loopcount);
	}
	FMOD_RESULT  CFModChannel::GetLoopCount(int* loopcount) {
		return g_pfnFMOD_Channel_GetLoopCount(m_pFModChennel, loopcount);
	}
	FMOD_RESULT  CFModChannel::SetLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype) {
		return g_pfnFMOD_Channel_SetLoopPoints(m_pFModChennel, loopstart, loopstarttype, loopend, loopendtype);
	}
	FMOD_RESULT  CFModChannel::GetLoopPoints(unsigned int* loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int* loopend, FMOD_TIMEUNIT loopendtype) {
		return g_pfnFMOD_Channel_GetLoopPoints(m_pFModChennel, loopstart, loopstarttype, loopend, loopendtype);
	}
	FMOD_RESULT  CFModChannel::SetUserData(void* userdata) {
		return g_pfnFMOD_Channel_SetUserData(m_pFModChennel, userdata);
	}
	FMOD_RESULT  CFModChannel::GetUserData(void** userdata) {
		return g_pfnFMOD_Channel_GetUserData(m_pFModChennel, userdata);
	}
	FMOD_RESULT  CFModChannel::GetMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int* memoryused, FMOD_MEMORY_USAGE_DETAILS* memoryused_details) {
		return g_pfnFMOD_Channel_GetMemoryInfo(m_pFModChennel, memorybits, event_memorybits, memoryused, memoryused_details);
	}
	FMOD_BOOL CFModChannel::Valid(){
		return m_pFModChennel != nullptr;
	}
	void CFModChannel::Set(FMOD_CHANNEL* channel){
		m_pFModChennel = channel;
	}
	CFModChannel::operator FMOD_CHANNEL* (){
		return m_pFModChennel;
	}
#endif // __USE_FMOD_CHANNEL_H_

}