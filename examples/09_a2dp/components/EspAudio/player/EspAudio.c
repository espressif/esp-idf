#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lwip/sys.h"
#include <sys/types.h>

#include "freertos/task.h"
#include "EspAudio.h"
#include "esp_system.h"
#include "Storage.h"
#include "audio_log.h"
#include "MediaHal.h"
#include "inc/DownloadTask.h"
#include "EspAudioCom.h"
#include "inc/MediaDecoderTask.h"
#include "MyString.h"
#include <string.h>
#include "Utility.h"
#include "freertos/semphr.h"
#include "nvs.h"


#define AUDIOLOOP_TSK_PRIO       8

const char * pAudioVolume = "ESPAUDIOVOL";

typedef enum {
    PlayerCtrlPrio_Unknown = 0,
    PlayerCtrlPrio_Auto    = 1,
    PlayerCtrlPrio_User    = 2,
} PlayerCtrlPrio;


typedef enum {
    PlayerWorkMode_Unknown          = 0,
    PlayerWorkMode_Decoder          = 1,
    PlayerWorkMode_Recorder         = 2,
    PlayerWorkMode_Max,
} PlayerWorkMode;


/*
 *  Set the player working mode,playback or recodring.
 *  Nonzero value indicates successed.
 */
int EspAudio_SetMode(PlayerWorkMode mode);

/*
 *  Sets the time position in seconds.
 */
int EspAudio_SetPosition (int timePosition);

/*
 *  Returns the estimated current playing position in seconds.
 *  Returns a negative value if the stream could not be found or the value is unknown.
 */
int EspAudio_GetTimePosition(uint32_t *timePos);

/*
 *  Returns the estimated total playing time of the track in seconds.
 *  Returns a negative value if the stream could not be found or the value is unknown.
 */
int EspAudio_GetTimeTotalLength(uint32_t *timeTotalLen);

void EspAudio_SetPlayStateChangeCb(void *func);

void EspAudio_DurationChangeCb(void *func);

void EspAudio_CanSetPositionCb(void *func);


/*
 *  Returns the current byte position of the decoding process.
 *  Returns a negative value if the stream could not be found or the value is unknown.
 */
int EspAudio_GetBytePosition(uint32_t *bytePos);

/*
 *  Returns the total number of bytes for the file.
 *  Returns a negative value if the stream could not be found or the value is unknown.
 */
int EspAudio_GetByteTotalLength(uint32_t *byteTotalLen);

/*
 *  Returns nonzero if the has the mute flag enabled.
 */
int EspAudio_GetMute(int *muteFlag);

/*
 *  Sets the mute flag for the specified stream.
 *  Nonzero value indicates mute flag is on.
 */
int EspAudio_SetMute(int muteFlag);


/*
 *  The codec-framework defines a callback for processing a change in playstate for the codec-framework.
 *  The callback executes on a thread owned by the codec-framework, so sink methods should only
 *  perform lightweight tasks.
 */
typedef void (*Callback_EspAudio_StateChange)(enum EspAudioState newState, void* codecSpecific, void* streamTag);

/*
 *  The codec-framework defines a callback for updating the total duration of the current uri.
 *  This is sometimes necessary as some filtergraphs determine the total duration of a track
 *  asynchronously.
 */
typedef void (*Callback_EspAudio_DurationChange)(int duration, void* codecSpecific, void* streamTag);

/*
 *  The codec-framework defines a callback for indicating if the current stream supports
 *  SetPosition.
 */
typedef void (*Callback_EspAudio_CanSetPosition)(int canSetPosition, void* codecSpecific, void* streamTag);



/*
 *  This counter tracks memory allocations used for
 *  the codec wrapper.
 */
int g_audio_malloc_counter  = 0;

/*
 *  Codec wrapper malloc.
 */
void* ESP_AUDIO_MALLOC(int size)
{
    g_audio_malloc_counter++;
#ifdef TRACK_MALLOC
    printf("g_audio_malloc_counter=%d\r\n", g_audio_malloc_counter);
#endif
    //ToDo: Memory Appears to be leaking here
    return malloc(size);
}

/*
 *  Codec wrapper free.
 */
void ESP_AUDIO_FREE(void *freeThis)
{
    g_audio_malloc_counter--;
#ifdef TRACK_MALLOC
    printf("g_audio_malloc_counter=%d\r\n", g_audio_malloc_counter);
#endif
    free(freeThis);
}

/*
 *  Codec wrapper string copy.
 */
char* ESP_AUDIO_SAFE_STRING_COPY(char* storeHere, const char* copyThis)
{
    if (storeHere == NULL) g_audio_malloc_counter++;
#ifdef TRACK_MALLOC
    printf("g_audio_malloc_counter=%d\r\n", g_audio_malloc_counter);
#endif
    //ToDo: Memory Appear to be leaking here
    return SafeStringCopy1(storeHere, copyThis);
}

/*
 *  Codec wrapper free that will reassign the
 *  provided pointer (after dereferencing) to
 *  a NULL value.
 */
void ESP_AUDIO_SAFE_FREE(void** freeThis)
{
    if (*freeThis != NULL) g_audio_malloc_counter--;
#ifdef TRACK_MALLOC
    printf("g_audio_malloc_counter=%d\r\n", g_audio_malloc_counter);
#endif
    SafeFree1(freeThis);
}

/*
 *  This structure tracks the state of a stream.
 */
struct StreamInstance {
    /*
     *  User-specified void*. This value represents the
     *  void* that the caller wants returned in
     *  codecwrapper callbacks.
     */
    void*   Input_StreamTag;

    /*
     *  If this value is nonzero, then the codec wrapper
     *  needs to start a new stream.
     */
    int     Input_ResetFlag;

    /*
     *  Method callback when the rendering framework changes state.
     */
    void *  Input_StateChangeCallback;


    /*
     *  Method callback when the rendering framework determines the
     *  duration of the track.
     */
//    Callback_EspAudio_DurationChange    Input_DurationChangeCallback;
    void *    Input_DurationChangeCallback;

    /*
     *  Method callback when the rendering framework determines the
     *  duration of the track.
     */
//   Callback_EspAudio_CanSetPosition    Input_CanSetPositionCallback;
    void*    Input_CanSetPositionCallback;

    /*
     *  This is the URI specified by the caller.
     */
    char*   Input_TrackUri;

    /*
     *  Semaphore to synchronize the input (executed on caller threads) and
     *  the codec wrapper thread, which performs the tasks for emulating
     *  a rendering operation.
     */
//    int   LockInput;

    /*
     *  Index into codewrapper_Streams.
     *  If the value is -1, it means the stream's thread
     *  should terminate.
     *  If the value is -2, then it means the stream has ended.
     */
    int Index;

    /*
     *  streamTag that the caller provided in EspAudio_SetupStream().
     *  Copied from the Input_StreamTag.
     */
    void* StreamTag;

    /*
     *  The current byte position in the stream decoding.
     */
    uint32_t BytePosition;

    /*
     *  The current time position in the stream decoding in seconds.
     */
    uint32_t TimePosition;

    /*
     *  The total number of bytes in the stream.
     */
    uint32_t TotalByteLength;

    /*
     *  The total time duration of the stream in seconds.
     */
    uint32_t TotalTimeLength;

    /*
     *  Current volume setting, between 0 and 100.
     */
    uint32_t Volume;

    /*
     *  Current URI for playback. Copied from Input_TrackUri.
     */
    char* Uri;

    /*
     *  Current play state - set only on the thread that executes
     *  in DecodeStreamLoop().
     */
    enum EspAudioState PlayState;

    /*
     *  Target play state - value is set on caller threads
     *  as well as codec/rendering thread
     *  (which executes in DecodeStreamLoop method).
     *  Thread safety achieved through LockInput.
     */
    enum EspAudioState TargetPlayState;

    /*
     *  This semaphore blocks when there is no media, state is stopped or paused.
     *  Effectively provides "idle" behavior for rendering.
     */
//    int BlockLock;

    /*
     *  The thread that is performing the work for rendering the stream.
     *  This thread executes in the DecodeStreamLoop method.
     */
    pthread_t Thread;

    /*
     *  Miscellaneous pointer for use in callbacks.
     *  Not used now, but could be used later when
     *  a rendering framework returns data in a struct
     *  from a callback.
     */
    void* CodecStuff;

    /*
     *  Execute this callback when the state changes.
     */
    Callback_EspAudio_StateChange StateChangeCallback;

    /*
     *  Execute this callback when the track's duration information
     *  is updated. (Sometimes, rendering frameworks need
     *  an asynchronous way to report the duration of the track.)
     */
    Callback_EspAudio_DurationChange DurationChangeCallback;

    /*
     *  Average bytes per second - used for estimating time.
     */
    uint32_t BytesPerSecond;

    /*
     *  Nonzero value indicates that the rendering should be muted.
     */
    int MuteFlag;

    uint32_t TrackRdPos;

    uint32_t TrackWrPos;

    int Input_UriSrcType;

    EspAudioMeidaType Input_MusicType;

    PlayerWorkMode Input_WorkMode;

    PlayerWorkMode workMode; // 1:decoder; 2:recorder;

    PlayerCtrlPrio  ctrlPrio;

};

/*
 *  Maximum number of simultaneous streams.
 *  Generally speaking, most rendering frameworks only
 *  allow one rendering stream. However this implementation
 *  it can support multiple simultaneous streams
 *  when calling EspAudio_Init() the first time.
 */
int MAX_MEDIA_STREAMS = 1;
#define STREAMINDEX     0
static xQueueHandle wrQue;

/*
 *  Dynamic allocation of streams that can be handled by the wrapper.
 *  Created in EspAudio_Init().
 */
struct StreamInstance *codecwrapperStreams;

/*
 *  This method changes the rendering framework's reported byte position.
 */
static void SetDecodeBytePosition(struct StreamInstance* si, int bytePos)
{
    /* position can be changed from multipel threads */
//  sem_wait(&(si->LockInput));
    si->BytePosition = bytePos;
    if (si->BytesPerSecond != 0) {
        si->TimePosition = si->BytePosition / si->BytesPerSecond;
    } else {
        si->TimePosition = 0;
    }

//  sem_post(&(si->LockInput));
}
//#include "CodecWrapper.h"
extern void MROnStopCB(void);
void MROnStopCB(void)
{
}

extern void OnCodecWrapper_StateChange (enum CodecWrapperState newState, void* codecSpecific, void* streamTag);

/*
 *  This method changes the rendering framework's reported playstate.
 */
static void SetAudioPlayState(struct StreamInstance* si, enum EspAudioState newState)
{
    /*  Apply new state. */
    si->PlayState = newState;
    // OnCodecWrapper_StateChange(newState, NULL, NULL);
    LOGD("si->PlayState=%d, newstate=%d", si->PlayState, newState);
}

// UriType:0 Not living;    1:living;   2:loacl file; 3:DLNA URI
/* see header file */
int EspAudio_SetupStream(const char* URI, InputSrcType UriType)
{
    struct StreamInstance *si = NULL;
    int retVal = -1;
    LOGD("Input_Uri:%s,Input source type=%d", URI, UriType);
    if ((NULL == URI)
        || (UriType > InputSrcType_Max)) {
        LOGE("URI=%s,InSrcType=%d", URI, UriType);
        return EspAudioErr_InvalidPara;
    }
    retVal = strlen(URI);
    if (retVal >= ESP_URI_LEN) {
        LOGE("URI out of range[%d]", retVal);
        return EspAudioErr_OutOfRange;
    }
    EspAudioMeidaType mediaType = checkMediaType(URI);
    if (EspAudioMeidaType_Unknown == mediaType) {
        LOGE("%s,media type does not support", URI);
        return EspAudioErr_NotSupport;
    }

    /*
     *  Find the appropriate stream.
     */
    si = &(codecwrapperStreams[STREAMINDEX]);
    if (si != NULL) {
//      sem_wait(&(si->LockInput));
        si->Input_ResetFlag = 1;
        si->Input_StreamTag = NULL;
        si->Input_TrackUri = ESP_AUDIO_SAFE_STRING_COPY(NULL, URI);
        si->Input_StateChangeCallback = NULL;
        si->Input_DurationChangeCallback = NULL;
        si->Input_CanSetPositionCallback = NULL;
        si->Input_UriSrcType = UriType;
        si->Input_MusicType = mediaType;

        si->ctrlPrio = PlayerCtrlPrio_Auto;
        retVal = 0;
        LOGD("Input source type=%d, music type=%d", UriType, mediaType);
    }

    return retVal;
}

/*
 *  This method can execute on caller-threads or on the codecwrapper
 *  thread that executes in DecodeStreamLoop.
 *  This method sets the desired state for the rendering framework
 *  to the specified state.
 */
int SetAudioDesiredState(enum EspAudioState state)
{
    int retVal = -1;
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    if (si != NULL) {
        si->TargetPlayState = state;
        if ((si->Uri == NULL) || (si->Uri[0] == '\0')) {
            si->TargetPlayState = AUDIOSTATE_UNINITIALIZED;
        }
        LOGD(">>>Wanted state[%d],TargetPlayState[%d] rdPos=%x", state, si->TargetPlayState, si->TrackRdPos);
        retVal = 0;
    }

    return retVal;
}

/*  Instructs rendering framework to play a specified stream. */
int EspAudio_Play(void)
{
    //TODO: enable trick modes with playSpeed

    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    si->ctrlPrio = PlayerCtrlPrio_User;

    return SetAudioDesiredState(AUDIOSTATE_PLAYING);
}

/*  Instructs rendering framework to stop a specified stream. */
int EspAudio_Stop(void)
{
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    si->ctrlPrio = PlayerCtrlPrio_User;
    return SetAudioDesiredState(AUDIOSTATE_STOPPED);
}

/*  Instructs rendering framework to pause a specified stream. */
int EspAudio_Pause(void)
{
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    si->ctrlPrio = PlayerCtrlPrio_User;
    return SetAudioDesiredState(AUDIOSTATE_PAUSED);
}

/*  Returns the current playstate for a given stream. */
enum EspAudioState EspAudio_GetPlayState(void)
{
    enum EspAudioState retVal = AUDIOSTATE_UNKNOWN;
    struct StreamInstance *si = NULL;

    si = &(codecwrapperStreams[STREAMINDEX]);

    if (si != NULL) {
        retVal = si->PlayState;
    }

    return retVal;
}

/*  Sets the desired playback position (seek operation) for a given stream. */
int EspAudio_SetPosition (int timePosition)
{
    struct StreamInstance *si = NULL;

    si = &(codecwrapperStreams[STREAMINDEX]);
    int byteSize = si->BytesPerSecond * timePosition;
    if (si != NULL) {
        SetDecodeBytePosition(si, byteSize);
        //downloadTaskDownloadSizeSet(&byteSize);
    }

    LOGD("Set time position=%d", timePosition);
    return 0;
}

/*  Returns the current playback time position for a given stream. */
int EspAudio_GetTimePosition(uint32_t *timePos)
{
    int retVal = -1;
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    if (si != NULL) {
        *timePos = (si->TimePosition);
    }
    LOGD("GetTimePosition=%d", *timePos);
    return retVal;
}

/*  Returns the total time duration for a given stream. */
int EspAudio_GetTimeTotalLength(uint32_t *timeTotalLen)
{
    int retVal = -1;
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    if (si != NULL) {
        *timeTotalLen = si->TotalTimeLength;
    }

    LOGD("TotalTimeLength=%d", *timeTotalLen);
    return retVal;
}

/*  Returns the current playback position, in terms of bytes, for a given stream. -1 means unknown. */
int EspAudio_GetBytePosition(uint32_t *bytePos)
{
    int retVal = -1;
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    if (si != NULL) {
        *bytePos = si->BytePosition;
    }

    LOGD("BytePosition=%d", *bytePos);
    return retVal;
}

/*  Returns the total byte length of a given for a given stream. -1 means unknown. */
int EspAudio_GetByteTotalLength(uint32_t *byteTotalLen)
{
    int retVal = -1;
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    if (si != NULL) {
        *byteTotalLen = si->TotalByteLength;
    }
    LOGD("TotalByteLength=%d", *byteTotalLen);
    return retVal;
}

/*  Gets the volume level of the specified stream. */
int  EspAudio_GetVolume(int *level)
{
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);

    if (si != NULL) {
        *level = si->Volume;
    }

    return EspAudioErr_NoErr;
}

/*  Sets the volume for a specified stream. */
int EspAudio_SetVolume(int level)
{
    struct StreamInstance *si = NULL;

    si = &(codecwrapperStreams[STREAMINDEX]);

    if (si != NULL) {
        mediaHalVolumeSet(level);
        if ((MediaErr_NoError == mediaHalVolumeSet(level))) {
            si->Volume = level;
            // Save data to flash.
        }
    }

    return 0;
}

int EspAudio_SetMode(PlayerWorkMode mode)
{
    if (mode > PlayerWorkMode_Max) {
        return -1;
    }
    struct StreamInstance *si = NULL;
    si = &(codecwrapperStreams[STREAMINDEX]);
    if (si != NULL) {
        si->Input_WorkMode = mode;
    }
    return 0;
}

void Show_Ap80Info(MusicInfo* info)
{
    printf("\n AP80 music info:\n");
    printf("stream_type  %d\n", info->stream_type);
    printf("num_channels %d\n", info->num_channels);
    printf("samplingrate %dHz\n", info->sampling_rate);
    printf("bitrate      %dbps\n", info->bitrate);
    printf("duration     %dms\n", info->duration);
    printf("file_size    %dbytes\n", info->file_size);
    printf("firmware_ver %d\n", info->firmware_ver);
    printf("firmware_num %d\n", info->firmware_num);
    printf("bufEmpty     %d\n", info->bufEmpty);
    printf("RFU          %d\n", info->RFU);
}


uint32_t CalTotalTime(uint32_t totalByte, uint32_t position, uint32_t time)
{
    return 1;
}

void decodeTaskCallback(uint32_t *state)
{
    struct StreamInstance *si;
    if (state) {
        si = &(codecwrapperStreams[0]);        
        si->TargetPlayState = AUDIOSTATE_STOPPED;
        audioBufRestAll();
        SetAudioPlayState(si, si->TargetPlayState);
    }
}


static int countForTime;
/*
 *  This is where all of the interesting work occurs.
 *  This method provides the execution loop for simulated
 *  rendering.
 */
void EspAudioLoop(void* streamInstance)
{
    struct StreamInstance *si;
    int uriChanged = 0;
    int newPos;
    enum EspAudioState oldState;
    int uriEmpty = 0;
    int finishedFlag = 0;
    int preTime = 0;
    int newTime = 0;
    uint32_t decodePreTime = 0;
    uint32_t intervalMS = 0; /* interval in milliseconds */
    si = (struct StreamInstance*) streamInstance;
    si->BytesPerSecond = 80000;
    LOGI("DecodeStreamLoop is running.");

    int musicInfoFlag = 0;
    while (si->Index >= 0) {
        if ((si->PlayState == AUDIOSTATE_UNINITIALIZED) ||
            (si->PlayState == AUDIOSTATE_STOPPED) ||
            (si->PlayState == AUDIOSTATE_PAUSED) ||
            (si->PlayState == AUDIOSTATE_ENDED)) {
            vTaskDelay(150);
            // LOGD("it's delay......heap:%d", system_get_free_heap_size());
        }
        uriChanged = 0;

//        LOGD("it's delay......heap:%d %d,state-%d %d", system_get_free_heap_size(), audioBufFillGet()
//            , si->PlayState, si->ctrlPrio);
        if (si->Input_ResetFlag != 0) {
            /* transfer the URI to the official state */
            ESP_AUDIO_SAFE_FREE((void**) & (si->Uri));
            si->Uri = si->Input_TrackUri;
            si->Input_TrackUri = NULL;
            si->StateChangeCallback = si->Input_StateChangeCallback;
            si->DurationChangeCallback = si->Input_DurationChangeCallback;
            si->StreamTag = si->Input_StreamTag;
            si->Input_ResetFlag = 0;
            countForTime = 0;
            uriChanged = 1;
            musicInfoFlag = 1;
            finishedFlag = 0;
            LOGD("Excute Input_ResetFlag");
        }
        TaskState httpTaskState = downloadTaskStateGet();
        TaskState mediaDecoderTaskState = mediaDecoderTaskStateGet();
        if ((TaskState_Finished == mediaDecoderTaskState)
            && (AUDIOSTATE_PLAYING == si->PlayState)) {
            si->TargetPlayState = AUDIOSTATE_ENDED;
            SetAudioPlayState(si, AUDIOSTATE_ENDED);

            finishedFlag = 1;
            LOGD("mediaDecoderTaskState has been stop");
        }

        if ((si->PlayState != AUDIOSTATE_PLAYING)
            && (audioBufFillGet() > (1024 * 10))
            && (si->ctrlPrio <= PlayerCtrlPrio_Auto)) {
            si->TargetPlayState = AUDIOSTATE_PLAYING;
            si->ctrlPrio = PlayerCtrlPrio_Auto;
            LOGI("Set playing, AUDIOSTATE_PLAYING");
        }
        if (uriChanged != 0) {
            UriSrcAttr uriAttr = { 0 };
            DecoderObj decoder = { 0 };

            TaskState httpTaskState = downloadTaskStateGet();
            TaskState mediaDecoderTaskState = mediaDecoderTaskStateGet();

            LOGD("InputType=%d,httpState=%d decoderState=%d",
                 si->Input_UriSrcType,  httpTaskState, mediaDecoderTaskState);
            downloadTaskStop(wrQue);
            LOGD("****downloadTaskStop****");
            mediaDecoderTaskStop();
            LOGD("****mediaDecoderTaskStop****");

            if ((TaskState_Unknown == httpTaskState)
                && (InputSrcType_Stream != si->Input_UriSrcType)) {
                downloadTaskCreate(si->Input_UriSrcType);
            } else  {
                LOGD("HttpObj task state:%d", httpTaskState);
            }
            if ((si->Uri == NULL) || (si->Uri[0] == '\0')) {
                si->TotalByteLength = 0;
                si->TotalTimeLength = 0;
                uriEmpty = 1;
                si->TargetPlayState = AUDIOSTATE_UNINITIALIZED;
                LOGE("URL is empty");
            } else {
                // Set the sound sources URL and tpye
                if (InputSrcType_Stream != si->Input_UriSrcType) {
                    uriAttr.str = si->Uri;
                    uriAttr.srcType = si->Input_UriSrcType;
                    downloadTaskInfoSet(&uriAttr);
                    downloadTaskStart();
                }

                mediaDecoderTaskInfoGet(&decoder);
                decoder.srcType = si->Input_UriSrcType;
                if (si->Input_MusicType != EspAudioMeidaType_M3u) {
                    decoder.musicType = si->Input_MusicType;
                } else {
                    decoder.musicType = EspAudioMeidaType_Unknown;
                }
                decoder.totalByteLength = 0;
                decoder.bytePosition = 0;
                mediaDecoderTaskInfoSet(&decoder);

                uriEmpty = 0;
                si->TargetPlayState = AUDIOSTATE_TRANSIT;

                si->TotalByteLength = 0;
                si->TotalTimeLength = 0;
            }

            si->CodecStuff = NULL;
            si->BytePosition = 0;
            si->TimePosition = 0;

            oldState = si->TargetPlayState;
            if (NULL != si->DurationChangeCallback) {
                si->DurationChangeCallback(si->TotalTimeLength, si, si->StreamTag);
            }
            /* set current position */
            SetDecodeBytePosition(si, 0);

            /* event that we're in transit or uninitialized */
            SetAudioPlayState(si, si->TargetPlayState);
            LOGD("***$$$***oldState=%x,%d", oldState, si->PlayState);

            uriChanged = 0;
        }

        if (si->PlayState != si->TargetPlayState) {
            LOGD("***it's here***oldstate=%d,new%d", si->PlayState, si->TargetPlayState);
            UriSrcAttr http = { 0 };
            TaskState httpTaskState;
            TaskState mediaDecoderTaskState;
            uint32_t contSize = 0;
            switch (si->TargetPlayState) {
            case AUDIOSTATE_ENDED:
                LOGI("AUDIOSTATE_ENDED");
                break;
            case AUDIOSTATE_PLAYING:
                // Ready to total bytes;

                LOGI("AUDIOSTATE_PLAYING, PlayState[%d], TargetPlayState[%d]", si->PlayState, si->TargetPlayState);
                downloadTaskContentSizeGet(&contSize);
                if (contSize != si->TotalByteLength) {
                    si->TotalByteLength = contSize;
                }
                DecoderObj decoder = { 0 };
                mediaDecoderTaskInfoGet(&decoder);
                if (contSize != decoder.totalByteLength) {
                    decoder.totalByteLength = contSize;
                    mediaDecoderTaskInfoSet(&decoder);
                }

                // Start
                httpTaskState = downloadTaskStateGet();
                mediaDecoderTaskState = mediaDecoderTaskStateGet();

                LOGI("mediaDecoderTaskResume[%d] httpTaskState[%d]...", mediaDecoderTaskState, httpTaskState);
                if (AUDIOSTATE_PAUSED == si->PlayState) {
                    if ((TaskState_Running != httpTaskState)) {
                        downloadTaskResume();
                        LOGI("invoke downloadTaskResume[%d]...", httpTaskState);
                    }
                    if ((TaskState_Running != mediaDecoderTaskState)) {
                        mediaDecoderTaskResume();
                        LOGI("invoke mediaDecoderTaskResume[%d]...", mediaDecoderTaskState);
                    }
                } else {
                    if ((TaskState_Running != httpTaskState)
                        && (InputSrcType_Stream != si->Input_UriSrcType)) {
                        downloadTaskStart();
                        LOGI("invoke downloadTaskStart[%d]...", httpTaskState);
                    }
                    if ((TaskState_Running != mediaDecoderTaskState)) {
                        mediaDecoderTaskStart();
                        LOGI("invoke mediaDecoderTaskStart[%d]...", mediaDecoderTaskState);
                    }
                }

                LOGI("Stream decoder is playing...,TotalByte=%d,fill=%d,totalTime=%d",
                     si->TotalByteLength, audioBufFillGet(), si->TotalTimeLength );
                break;
            case AUDIOSTATE_PAUSED:
                LOGI("AUDIOSTATE_PAUSED, PlayState:%d, TargetPlayState:%d", si->PlayState, si->TargetPlayState);
                if (si->ctrlPrio == PlayerCtrlPrio_Auto) {
                    if (1 == downloadTaskHTTPStateGet()) { // running
                        mediaDecoderTaskPause();
                    }
                } else {
                    mediaDecoderTaskPause();
                    downloadTaskPause();
                }
                break;
            case AUDIOSTATE_STOPPED:
                LOGI("AUDIOSTATE_STOPPED");
                mediaDecoderTaskStop();
                downloadTaskStop(wrQue);

                break;
            case AUDIOSTATE_TRANSIT:

                MROnStopCB();
                break;
            default:
                vTaskDelay(100 / portTICK_PERIOD_MS);
                break;
            }

            SetAudioPlayState(si, si->TargetPlayState);
        }

        if (((si->PlayState == AUDIOSTATE_PLAYING) || (finishedFlag))
            && (InputSrcType_Stream != si->Input_UriSrcType)
            && (InputSrcType_NetLiving != si->Input_UriSrcType)) {
            newTime = system_get_time();
//            LOGD("Ready to get decode time");
            if (((newTime - preTime) > 980000)) { // 1 second
                if (0/*MediaErr_NoError == mediaHalDecodeTimeGet(&intervalMS)*/) {
                    if (intervalMS > 0) {
                        preTime = system_get_time();
                        if (countForTime <= 3) {
                            si->TimePosition = intervalMS / 1000;
                        }
                        DecoderObj decoder = { 0 };
                        mediaDecoderTaskInfoGet(&decoder);
                        si->BytePosition = decoder.bytePosition;

                        int perSec = si->BytePosition / intervalMS;
                        int time = 0;
                        if (perSec > 0) {
                            time = si->TotalByteLength / perSec;
                        }
//                        LOGD("intervalMS%d,time%d,bytePos%d,perSec%d,totalTime%d",intervalMS,
//                            si->TimePosition, si->BytePosition, perSec, time);
                        si->TotalTimeLength = time / 1000;
                        if (decodePreTime == si->TimePosition) {
                            countForTime++;
                            if (countForTime > 3) {
                                si->TimePosition = si->TotalTimeLength;
                                LOGD("countForTime TimePosition %d,state=%d", si->TimePosition, si->PlayState);
                                finishedFlag = 0;
                                countForTime = 0;
                            }
                        } else {
                            countForTime = 0;
                        }
                        decodePreTime = si->TimePosition;

                    } else {
//                        LOGD("intervalMS=%d preTime=%d,%d,bytePostion%d",intervalMS, preTime,
//                            si->TimePosition, si->BytePosition);
                    }
                }
            }
        }


        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    LOGD("task is exit");
    si->Index = -2;
    vTaskDelete(NULL);
}


/*
 *  Initializes the codecwrapper library to handle the specified number of streams.
 */
void EspAudio_Init(void)
{
    int i;
    struct StreamInstance *si;
    codecwrapperStreams = (struct StreamInstance*) ESP_AUDIO_MALLOC(MAX_MEDIA_STREAMS * sizeof(struct StreamInstance));
    LOGI("| >>>>> EspAudio version is %s <<<<< |\r\n", ESP_AUDIO_RELEASE_VER);
    /*
     *  Initialize and reserve resources that could
     *  be used for playback of streams.
     */

    mediaHalInit();
    audioBufInit();
    audioBufRestAll(); // may be not need to do.if the URL is working
    MusicInfo info;
    mediaHalMusicInfoGet(&info);
    wrQue = xQueueCreate(2, 1);
    configASSERT(wrQue);
    LOGD("wrQue=%p", wrQue);
    audioBufWrQuitCfg(wrQue);
    if (MediaErr_NoError == mediaHalMusicInfoGet(&info)) {
        LOGI(" *** AP80 firmware_ver=%d,firmware_num=%d ***\n", info.firmware_ver, info.firmware_num);
    } else {
        LOGE(" *** AP80 firmware got failed ***\n");
    }

    for (i = 0; i < MAX_MEDIA_STREAMS; i++) {
        si = &(codecwrapperStreams[i]);

        si->Input_ResetFlag = 0;
        si->Input_StreamTag = NULL;
        si->Input_TrackUri = NULL;
        si->Volume = 0;

        si->Index = i;
        si->StreamTag = NULL;
        si->BytePosition = 0;
        si->TimePosition = 0;
        si->TotalByteLength = 0;
        si->TotalTimeLength = 0;
        si->PlayState = AUDIOSTATE_UNINITIALIZED;
        si->TargetPlayState = AUDIOSTATE_UNINITIALIZED;
        si->CodecStuff = NULL;
        si->StateChangeCallback = NULL;
        si->DurationChangeCallback = NULL;
        si->Input_CanSetPositionCallback = NULL;
        si->Uri = NULL;
        si->MuteFlag = 0;
        si->ctrlPrio = PlayerCtrlPrio_Unknown;

        audioBufPosGet(&si->TrackRdPos, &si->TrackWrPos);
        xTaskCreate(EspAudioLoop, "EspAudioLoop", 3*1024, si, AUDIOLOOP_TSK_PRIO, &(si->Thread));

        mediaDecoderTaskCreate();
        EspAudio_SetVolume(30);
        //    if (false == system_param_load(0x70,0,(void*)&si->Volume, sizeof(si->Volume))) {
//            LOGD("system_param_load volume failed,set default value");
        //    }

    }

}

/*
 *  Stops the codecwrapper thread and instructs appropriately
 *  deallocates stuff.
 */
void EspAudio_UnInit(void)
{
    int i;
    struct StreamInstance *si;
    int numThreadsRunning = MAX_MEDIA_STREAMS;

    /*
     *  Instruct all decode threads to stop
     *  by setting the index==-1, be
     *  sure to wake each thread in case it's asleep.
     */
    for (i = 0; i < MAX_MEDIA_STREAMS; i++) {
        si = &(codecwrapperStreams[i]);
        si->Index = -1;
        //PulseEspAudioThread(si);
    }

    downloadTaskDestroy();

    mediaDecoderTaskDestroy();
    /* spin here until all threads are done */
    while (numThreadsRunning > 0) {
        /* block for a short period of time */
        // SleepMsTime(100);
        numThreadsRunning = MAX_MEDIA_STREAMS;
        for (i = 0; i < MAX_MEDIA_STREAMS; i++) {
            si = &(codecwrapperStreams[i]);
            if (si->Index == -2) {
                numThreadsRunning--;
            }
        }
    }

    /* all threads are done, finish unitializing stream instances */

    for (i = 0; i < MAX_MEDIA_STREAMS; i++) {
        si = &(codecwrapperStreams[i]);
        ESP_AUDIO_SAFE_FREE((void**) & (si->Input_TrackUri));
        ESP_AUDIO_SAFE_FREE((void**) & (si->Uri));
        si->StreamTag = NULL;
        si->CodecStuff = NULL;
        si->StateChangeCallback = NULL;
        si->DurationChangeCallback = NULL;
//      sem_destroy(&(si->BlockLock));
//      sem_destroy(&(si->LockInput));
        si->Thread = 0;
    }
    vQueueDelete(wrQue);
    wrQue = NULL;
    ESP_AUDIO_FREE(codecwrapperStreams);
}

/*
 *  Returns the mute value for the specified stream.
 */
int EspAudio_GetMute(int *muteFlag)
{
    struct StreamInstance *si;
    si = &(codecwrapperStreams[STREAMINDEX]);
    *muteFlag = si->MuteFlag;
    return EspAudioErr_NoErr;
}

/*
 *  Sets tne mute value for the specified stream.
 */
int EspAudio_SetMute(int muteFlag)
{
    struct StreamInstance *si;
    si = &(codecwrapperStreams[STREAMINDEX]);
    si->MuteFlag = muteFlag;
    return 0;
}

static uint32_t streamSize = 0;
EspAudioErr EspAudioPlayerStreamCfg(uint32_t rate, uint32_t channel, uint32_t bitLen)
{
    StreamInfo info;
    info.rate = rate;
    info.ch = channel;
    info.bitLen = bitLen;
    info.size = 0xFFFFFFFF;
    streamSize = 0;
    mediaDecoderStreamInfoSet(&info);
    mediaDecoderTaskCb(decodeTaskCallback);
    return EspAudioErr_NoErr;
}

EspAudioErr EspAudioPlayerStreamWrite(uint8_t *inData, uint32_t inSize, uint32_t timeout)
{
    EspAudioErr ret = EspAudioErr_NoErr;
    if ((NULL == inData) || (0 == inSize)) {
        LOGE("Invalid para,inData=%x,inSize=%d", inData, inSize);
        ret = EspAudioErr_InvalidPara;
    } else {
        int wr = audioBufWrite((char*)inData, inSize, timeout);
        if (0 == wr) {
            ret = EspAudioErr_NoMem;
        } else {            
            streamSize += inSize;
        }
    }
    return ret;
}

EspAudioErr EspAudioPlayerStreamWriteEnd(void)
{
    mediaDecoderStreamSizeUpdate(&streamSize);
    streamSize = 0;
    return EspAudioErr_NoErr;
}

