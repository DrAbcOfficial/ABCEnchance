#ifndef __FMOD_SOUNDEING_

//Fuck WinAPI
#ifdef PlaySound
#undef PlaySound
#endif

#include <fmod.hpp>
#include <fmod_errors.h>

#define __USE_FMOD_SYSTEM_H_
#define __USE_FMOD_SOUND_H_
#define __USE_FMOD_CHANNEL_H_

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

        operator FMOD_SYSTEM*();
    private:
        FMOD_SYSTEM* m_pFModSystem;
    };
    CFModSystem* GetSystem();
#endif // __USE_FMOD_SYSTEM_H_
#ifdef __USE_FMOD_SOUND_H_
    class CFModSound {
    public:
        CFModSound();
        CFModSound(FMOD_SOUND* sound);
        ~CFModSound();
        /*
           'Sound' API
        */

        FMOD_RESULT Release();
        FMOD_RESULT GetSystemObject(FMOD_SYSTEM** system);

        /*
             Standard sound manipulation functions.
        */

        FMOD_RESULT Lock(unsigned int offset, unsigned int length, void** ptr1, void** ptr2, unsigned int* len1, unsigned int* len2);
        FMOD_RESULT Unlock(void* ptr1, void* ptr2, unsigned int len1, unsigned int len2);
        FMOD_RESULT SetDefaults(float frequency, float volume, float pan, int priority);
        FMOD_RESULT GetDefaults(float* frequency, float* volume, float* pan, int* priority);
        FMOD_RESULT SetVariations(float frequencyvar, float volumevar, float panvar);
        FMOD_RESULT GetVariations(float* frequencyvar, float* volumevar, float* panvar);
        FMOD_RESULT Set3DMinMaxDistance(float min, float max);
        FMOD_RESULT Get3DMinMaxDistance(float* min, float* max);
        FMOD_RESULT Set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
        FMOD_RESULT Get3DConeSettings(float* insideconeangle, float* outsideconeangle, float* outsidevolume);
        FMOD_RESULT Set3DCustomRolloff(FMOD_VECTOR* points, int numpoints);
        FMOD_RESULT Get3DCustomRolloff(FMOD_VECTOR** points, int* numpoints);
        FMOD_RESULT SetSubSound(int index, FMOD_SOUND* subsound);
        FMOD_RESULT GetSubSound(int index, FMOD_SOUND** subsound);
        FMOD_RESULT GetSubSoundParent(FMOD_SOUND** parentsound);
        FMOD_RESULT SetSubSoundSentence(int* subsoundlist, int numsubsounds);
        FMOD_RESULT GetName(char* name, int namelen);
        FMOD_RESULT GetLength(unsigned int* length, FMOD_TIMEUNIT lengthtype);
        FMOD_RESULT GetFormat(int* type, int* format, int* channels, int* bits);
        FMOD_RESULT GetNumSubSounds(int* numsubsounds);
        FMOD_RESULT GetNumTags(int* numtags, int* numtagsupdated);
        FMOD_RESULT GetTag(const char* name, int index, FMOD_TAG* tag);
        FMOD_RESULT GetOpenState(FMOD_OPENSTATE* openstate, unsigned int* percentbuffered, FMOD_BOOL* starving, FMOD_BOOL* diskbusy);
        FMOD_RESULT ReadData(void* buffer, unsigned int lenbytes, unsigned int* read);
        FMOD_RESULT SeekData(unsigned int pcm);

        FMOD_RESULT SetSoundGroup(FMOD_SOUNDGROUP* soundgroup);
        FMOD_RESULT GetSoundGroup(FMOD_SOUNDGROUP** soundgroup);

        /*
             Synchronization point API.  These points can come from markers embedded in wav files, and can also generate channel callbacks.
        */

        FMOD_RESULT GetNumSyncPoints(int* numsyncpoints);
        FMOD_RESULT GetSyncPoint(int index, FMOD_SYNCPOINT** point);
        FMOD_RESULT GetSyncPointInfo(FMOD_SYNCPOINT* point, char* name, int namelen, unsigned int* offset, FMOD_TIMEUNIT offsettype);
        FMOD_RESULT AddSyncPoint(unsigned int offset, FMOD_TIMEUNIT offsettype, const char* name, FMOD_SYNCPOINT** point);
        FMOD_RESULT DeleteSyncPoint(FMOD_SYNCPOINT* point);

        /*
             Functions also in Channel class but here they are the 'default' to save having to change it in Channel all the time.
        */

        FMOD_RESULT SetMode(FMOD_MODE mode);
        FMOD_RESULT GetMode(FMOD_MODE* mode);
        FMOD_RESULT SetLoopCount(int loopcount);
        FMOD_RESULT GetLoopCount(int* loopcount);
        FMOD_RESULT SetLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
        FMOD_RESULT GetLoopPoints(unsigned int* loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int* loopend, FMOD_TIMEUNIT loopendtype);

        /*
             For MOD/S3M/XM/IT/MID sequenced formats only.
        */

        FMOD_RESULT GetMusicNumChannels(int* numchannels);
        FMOD_RESULT SetMusicChannelVolume(int channel, float volume);
        FMOD_RESULT GetMusicChannelVolume(int channel, float* volume);
        FMOD_RESULT SetMusicSpeed(float speed);
        FMOD_RESULT GetMusicSpeed(float* speed);

        /*
             Userdata set/get.
        */

        FMOD_RESULT SetUserData(void* userdata);
        FMOD_RESULT GetUserData(void** userdata);

        FMOD_RESULT GetMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int* memoryused, FMOD_MEMORY_USAGE_DETAILS* memoryused_details);

        FMOD_BOOL Valid();
        void Set(FMOD_SOUND* sound);
        operator FMOD_SOUND* ();
    private:
        FMOD_SOUND* m_pFModSound = nullptr;
    };
#endif // __USE_FMOD_SOUND_H_
#ifdef __USE_FMOD_CHANNEL_H_
    class CFModChannel {
    public:
        CFModChannel();
        CFModChannel(FMOD_CHANNEL* channel);
        ~CFModChannel();
        /*
            'Channel' API
        */
        FMOD_RESULT GetSystemObject(FMOD_SYSTEM** system);

        FMOD_RESULT Stop();
        FMOD_RESULT SetPaused(FMOD_BOOL paused);
        FMOD_RESULT GetPaused(FMOD_BOOL* paused);
        FMOD_RESULT SetVolume(float volume);
        FMOD_RESULT GetVolume(float* volume);
        FMOD_RESULT SetFrequency(float frequency);
        FMOD_RESULT GetFrequency(float* frequency);
        FMOD_RESULT SetPan(float pan);
        FMOD_RESULT GetPan(float* pan);
        FMOD_RESULT SetDelay(FMOD_DELAYTYPE delaytype, unsigned int delayhi, unsigned int delaylo);
        FMOD_RESULT GetDelay(FMOD_DELAYTYPE delaytype, unsigned int* delayhi, unsigned int* delaylo);
        FMOD_RESULT SetSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
        FMOD_RESULT GetSpeakerMix(float* frontleft, float* frontright, float* center, float* lfe, float* backleft, float* backright, float* sideleft, float* sideright);
        FMOD_RESULT SetSpeakerLevels(FMOD_SPEAKER speaker, float* levels, int numlevels);
        FMOD_RESULT GetSpeakerLevels(FMOD_SPEAKER speaker, float* levels, int numlevels);
        FMOD_RESULT SetInputChannelMix(float* levels, int numlevels);
        FMOD_RESULT GetInputChannelMix(float* levels, int numlevels);
        FMOD_RESULT SetMute(FMOD_BOOL mute);
        FMOD_RESULT GetMute(FMOD_BOOL* mute);
        FMOD_RESULT SetPriority(int priority);
        FMOD_RESULT GetPriority(int* priority);
        FMOD_RESULT SetPosition(unsigned int position, FMOD_TIMEUNIT postype);
        FMOD_RESULT GetPosition(unsigned int* position, FMOD_TIMEUNIT postype);
        FMOD_RESULT SetReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES* prop);
        FMOD_RESULT GetReverbProperties(FMOD_REVERB_CHANNELPROPERTIES* prop);
        FMOD_RESULT SetLowPassGain(float gain);
        FMOD_RESULT GetLowPassGain(float* gain);

        FMOD_RESULT SetChannelGroup(FMOD_CHANNELGROUP* channelgroup);
        FMOD_RESULT GetChannelGroup(FMOD_CHANNELGROUP** channelgroup);
        FMOD_RESULT SetCallback(FMOD_CHANNEL_CALLBACK callback);

        /*
             3D functionality.
        */

        FMOD_RESULT Set3DAttributes(const FMOD_VECTOR* pos, const FMOD_VECTOR* vel);
        FMOD_RESULT Get3DAttributes(FMOD_VECTOR* pos, FMOD_VECTOR* vel);
        FMOD_RESULT Set3DMinMaxDistance(float mindistance, float maxdistance);
        FMOD_RESULT Get3DMinMaxDistance(float* mindistance, float* maxdistance);
        FMOD_RESULT Set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
        FMOD_RESULT Get3DConeSettings(float* insideconeangle, float* outsideconeangle, float* outsidevolume);
        FMOD_RESULT Set3DConeOrientation(FMOD_VECTOR* orientation);
        FMOD_RESULT Get3DConeOrientation(FMOD_VECTOR* orientation);
        FMOD_RESULT Set3DCustomRolloff(FMOD_VECTOR* points, int numpoints);
        FMOD_RESULT Get3DCustomRolloff(FMOD_VECTOR** points, int* numpoints);
        FMOD_RESULT Set3DOcclusion(float directocclusion, float reverbocclusion);
        FMOD_RESULT Get3DOcclusion(float* directocclusion, float* reverbocclusion);
        FMOD_RESULT Set3DSpread(float angle);
        FMOD_RESULT Get3DSpread(float* angle);
        FMOD_RESULT Set3DPanLevel(float level);
        FMOD_RESULT Get3DPanLevel(float* level);
        FMOD_RESULT Set3DDopplerLevel(float level);
        FMOD_RESULT Get3DDopplerLevel(float* level);
        FMOD_RESULT Set3DDistanceFilter(FMOD_BOOL custom, float customLevel, float centerFreq);
        FMOD_RESULT Get3DDistanceFilter(FMOD_BOOL* custom, float* customLevel, float* centerFreq);

        /*
             DSP functionality only for channels playing sounds created with FMOD_SOFTWARE.
        */

        FMOD_RESULT GetDSPHead(FMOD_DSP** dsp);
        FMOD_RESULT AddDSP(FMOD_DSP* dsp, FMOD_DSPCONNECTION** connection);

        /*
             Information only functions.
        */

        FMOD_RESULT IsPlaying(FMOD_BOOL* isplaying);
        FMOD_RESULT IsVirtual(FMOD_BOOL* isvirtual);
        FMOD_RESULT GetAudibility(float* audibility);
        FMOD_RESULT GetCurrentSound(FMOD_SOUND** sound);
        FMOD_RESULT GetSpectrum(float* spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT GetWaveData(float* wavearray, int numvalues, int channeloffset);
        FMOD_RESULT GetIndex(int* index);

        /*
             Functions also found in Sound class but here they can be set per channel.
        */

        FMOD_RESULT SetMode(FMOD_MODE mode);
        FMOD_RESULT GetMode(FMOD_MODE* mode);
        FMOD_RESULT SetLoopCount(int loopcount);
        FMOD_RESULT GetLoopCount(int* loopcount);
        FMOD_RESULT SetLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
        FMOD_RESULT GetLoopPoints(unsigned int* loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int* loopend, FMOD_TIMEUNIT loopendtype);

        /*
             Userdata set/get.
        */

        FMOD_RESULT SetUserData(void* userdata);
        FMOD_RESULT GetUserData(void** userdata);

        FMOD_RESULT GetMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int* memoryused, FMOD_MEMORY_USAGE_DETAILS* memoryused_details);

        FMOD_BOOL Valid();
        void Set(FMOD_CHANNEL* channel);
        operator FMOD_CHANNEL*();
private:
        FMOD_CHANNEL* m_pFModChennel = nullptr;
    };
#endif // __USE_FMOD_CHANNEL_H_
    void InitFModLibrary();
    void FreeFModLibrary();
}

#define __FMOD_SOUNDEING_
#endif // !__FMOD_SOUNDEING_