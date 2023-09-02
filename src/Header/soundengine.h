#ifndef __FMOD_SOUNDEING_
#include <fmod.hpp>
#include <fmod_errors.h>

//Fuck WinAPI
#ifdef PlaySound
#undef PlaySound
#endif

#define __USE_FMOD_SYSTEM_H_
#define SYSTEM_MAXCHANNEL 6

namespace FModEngine {
#ifdef __USE_FMOD_SYSTEM_H_
    class CFModSystem {
    public:
        CFModSystem();
        ~CFModSystem();

        FMOD_RESULT Create();
        FMOD_RESULT Release();

        FMOD_RESULT SetOutput(FMOD_OUTPUTTYPE output);
        FMOD_RESULT GetOutput(FMOD_OUTPUTTYPE* output);
        FMOD_RESULT GetNumDrivers(int* numdrivers);
        FMOD_RESULT GetDriverInfo(int id, char* name, int namelen, FMOD_GUID* guid);
        FMOD_RESULT GetDriverInfoW(int id, short* name, int namelen, FMOD_GUID* guid);
        FMOD_RESULT GetDriverCaps(int id, FMOD_CAPS* caps, int* controlpaneloutputrate, FMOD_SPEAKERMODE* controlpanelspeakermode);
        FMOD_RESULT SetDriver(int driver);
        FMOD_RESULT GetDriver(int* driver);
        FMOD_RESULT SetHardwareChannels(int numhardwarechannels);
        FMOD_RESULT SetSoftwareChannels(int numsoftwarechannels);
        FMOD_RESULT GetSoftwareChannels(int* numsoftwarechannels);
        FMOD_RESULT SetSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod);
        FMOD_RESULT GetSoftwareFormat(int* samplerate, FMOD_SOUND_FORMAT* format, int* numoutputchannels, int* maxinputchannels, FMOD_DSP_RESAMPLER* resamplemethod, int* bits);
        FMOD_RESULT SetDSPBufferSize(unsigned int bufferlength, int numbuffers);
        FMOD_RESULT GetDSPBufferSize(unsigned int* bufferlength, int* numbuffers);
        FMOD_RESULT SetFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, FMOD_FILE_ASYNCREADCALLBACK userasyncread, FMOD_FILE_ASYNCCANCELCALLBACK userasynccancel, int blockalign);
        FMOD_RESULT AttachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek);
        FMOD_RESULT SetAdvancedSettings(FMOD_ADVANCEDSETTINGS* settings);
        FMOD_RESULT GetAdvancedSettings(FMOD_ADVANCEDSETTINGS* settings);
        FMOD_RESULT SetSpeakerMode(FMOD_SPEAKERMODE speakermode);
        FMOD_RESULT GetSpeakerMode(FMOD_SPEAKERMODE* speakermode);
        FMOD_RESULT SetCallback(FMOD_SYSTEM_CALLBACK callback);

        FMOD_RESULT SetPluginPath(const char* path);
        FMOD_RESULT LoadPlugin(const char* filename, unsigned int* handle, unsigned int priority);
        FMOD_RESULT UnloadPlugin(unsigned int handle);
        FMOD_RESULT GetNumPlugins(FMOD_PLUGINTYPE plugintype, int* numplugins);
        FMOD_RESULT GetPluginHandle(FMOD_PLUGINTYPE plugintype, int index, unsigned int* handle);
        FMOD_RESULT GetPluginInfo(unsigned int handle, FMOD_PLUGINTYPE* plugintype, char* name, int namelen, unsigned int* version);
        FMOD_RESULT SetOutputByPlugin(unsigned int handle);
        FMOD_RESULT GetOutputByPlugin(unsigned int* handle);
        FMOD_RESULT CreateDSPByPlugin(unsigned int handle, FMOD_DSP** dsp);
        FMOD_RESULT RegisterCodec(FMOD_CODEC_DESCRIPTION* description, unsigned int* handle, unsigned int priority);
        FMOD_RESULT RegisterDSP(FMOD_DSP_DESCRIPTION* description, unsigned int* handle);

        FMOD_RESULT Init(int maxchannels, FMOD_INITFLAGS flags, void* extradriverdata);
        FMOD_RESULT Close();

        FMOD_RESULT Update();

        FMOD_RESULT Set3DSettings(float dopplerscale, float distancefactor, float rolloffscale);
        FMOD_RESULT Get3DSettings(float* dopplerscale, float* distancefactor, float* rolloffscale);
        FMOD_RESULT Set3DNumListeners(int numlisteners);
        FMOD_RESULT Get3DNumListeners(int* numlisteners);
        FMOD_RESULT Set3DListenerAttributes(int listener, const FMOD_VECTOR* pos, const FMOD_VECTOR* vel, const FMOD_VECTOR* forward, const FMOD_VECTOR* up);
        FMOD_RESULT Get3DListenerAttributes(int listener, FMOD_VECTOR* pos, FMOD_VECTOR* vel, FMOD_VECTOR* forward, FMOD_VECTOR* up);
        FMOD_RESULT Set3DRolloffCallback(FMOD_3D_ROLLOFFCALLBACK callback);
        FMOD_RESULT Set3DSpeakerPosition(FMOD_SPEAKER speaker, float x, float y, FMOD_BOOL active);
        FMOD_RESULT Get3DSpeakerPosition(FMOD_SPEAKER speaker, float* x, float* y, FMOD_BOOL* active);

        FMOD_RESULT SetStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
        FMOD_RESULT GetStreamBufferSize(unsigned int* filebuffersize, FMOD_TIMEUNIT* filebuffersizetype);

        FMOD_RESULT GetVersion(unsigned int* version);
        FMOD_RESULT GetOutputHandle(void** handle);
        FMOD_RESULT GetChannelsPlaying(int* channels);
        FMOD_RESULT GetHardwareChannels(int* numhardwarechannels);
        FMOD_RESULT GetCPUUsage(float* dsp, float* stream, float* geometry, float* update, float* total);
        FMOD_RESULT GetSoundRAM(int* currentalloced, int* maxalloced, int* total);
        FMOD_RESULT GetNumCDROMDrives(int* numdrives);
        FMOD_RESULT GetCDROMDriveName(int drive, char* drivename, int drivenamelen, char* scsiname, int scsinamelen, char* devicename, int devicenamelen);
        FMOD_RESULT GetSpectrum(float* spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT GetWaveData(float* wavearray, int numvalues, int channeloffset);

        FMOD_RESULT CreateSound(const char* name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo, FMOD_SOUND** sound);
        FMOD_RESULT CreateStream(const char* name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo, FMOD_SOUND** sound);
        FMOD_RESULT CreateDSP(FMOD_DSP_DESCRIPTION* description, FMOD_DSP** dsp);
        FMOD_RESULT CreateDSPByType(FMOD_DSP_TYPE type, FMOD_DSP** dsp);
        FMOD_RESULT CreateChannelGroup(const char* name, FMOD_CHANNELGROUP** channelgroup);
        FMOD_RESULT CreateSoundGroup(const char* name, FMOD_SOUNDGROUP** soundgroup);
        FMOD_RESULT CreateReverb(FMOD_REVERB** reverb);

        FMOD_RESULT PlaySound(FMOD_CHANNELINDEX channelid, FMOD_SOUND* sound, FMOD_BOOL paused, FMOD_CHANNEL** channel);
        FMOD_RESULT PlayDSP(FMOD_CHANNELINDEX channelid, FMOD_DSP* dsp, FMOD_BOOL paused, FMOD_CHANNEL** channel);
        FMOD_RESULT GetChannel(int channelid, FMOD_CHANNEL** channel);
        FMOD_RESULT GetMasterChannelGroup(FMOD_CHANNELGROUP** channelgroup);
        FMOD_RESULT GetMasterSoundGroup(FMOD_SOUNDGROUP** soundgroup);

        FMOD_RESULT SetReverbProperties(const FMOD_REVERB_PROPERTIES* prop);
        FMOD_RESULT GetReverbProperties(FMOD_REVERB_PROPERTIES* prop);
        FMOD_RESULT SetReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop);
        FMOD_RESULT GetReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop);

        FMOD_RESULT GetDSPHead(FMOD_DSP** dsp);
        FMOD_RESULT AddDSP(FMOD_DSP* dsp, FMOD_DSPCONNECTION** connection);
        FMOD_RESULT LockDSP();
        FMOD_RESULT UnlockDSP();
        FMOD_RESULT GetDSPClock(unsigned int* hi, unsigned int* lo);

        FMOD_RESULT GetRecordNumDrivers(int* numdrivers);
        FMOD_RESULT GetRecordDriverInfo(int id, char* name, int namelen, FMOD_GUID* guid);
        FMOD_RESULT GetRecordDriverInfoW(int id, short* name, int namelen, FMOD_GUID* guid);
        FMOD_RESULT GetRecordDriverCaps(int id, FMOD_CAPS* caps, int* minfrequency, int* maxfrequency);
        FMOD_RESULT GetRecordPosition(int id, unsigned int* position);

        FMOD_RESULT RecordStart(int id, FMOD_SOUND* sound, FMOD_BOOL loop);
        FMOD_RESULT RecordStop(int id);
        FMOD_RESULT IsRecording(int id, FMOD_BOOL* recording);

        FMOD_RESULT CreateGeometry(int maxpolygons, int maxvertices, FMOD_GEOMETRY** geometry);
        FMOD_RESULT SetGeometrySettings(float maxworldsize);
        FMOD_RESULT GetGeometrySettings(float* maxworldsize);
        FMOD_RESULT LoadGeometry(const void* data, int datasize, FMOD_GEOMETRY** geometry);
        FMOD_RESULT GetGeometryOcclusion(const FMOD_VECTOR* listener, const FMOD_VECTOR* source, float* direct, float* reverb);

        FMOD_RESULT SetNetworkProxy(const char* proxy);
        FMOD_RESULT GetNetworkProxy(char* proxy, int proxylen);
        FMOD_RESULT SetNetworkTimeout(int timeout);
        FMOD_RESULT GetNetworkTimeout(int* timeout);

        FMOD_RESULT SetUserData(void* userdata);
        FMOD_RESULT GetUserData(void** userdata);

        FMOD_RESULT GetMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int* memoryused, FMOD_MEMORY_USAGE_DETAILS* memoryused_details);

        FMOD_RESULT StopSound(FMOD_CHANNEL* channel);
        FMOD_RESULT FreeSound(FMOD_SOUND* sound);
        FMOD_RESULT GetLength(FMOD_SOUND* sound, unsigned int* length, FMOD_TIMEUNIT lengthtype);
    private:
        FMOD_SYSTEM* m_pFModSystem;
    };
    CFModSystem* GetSystem();
#endif // __USE_FMOD_SYSTEM_H_

    void InitFModLibrary();
    void FreeFModLibrary();
}

#define __FMOD_SOUNDEING_
#endif // !__FMOD_SOUNDEING_