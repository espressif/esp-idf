/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#ifndef __AP80XX_H__
#define __AP80XX_H__
#include <stdint.h>
#pragma pack (1)

#define CHIPTYPE 32

#if CHIPTYPE == 32
#include "soc/gpio_sig_map.h"

#define SPIMasterSendData           spi_master_send_data

#define HSPICLK_OUT_MUX_IDX         HSPICLK_OUT_IDX
#define SpiNum_HSPI                 SPI_NUM_SPI2

#define AP80XXENABLEDMA             1
#define AP80XXENABLEINT             1
#define CALLPLAYOBJ_EN              1

#define SPIM_STREAM_CONTROL_GET_STATUS() (gpio_get_level(GPIO_NUM_22))

#elif CHIPTYPE == 31
#define SPIM_STREAM_CONTROL_GET_STATUS() (GPIO_INPUT_GET(22))
#endif

#define EAU_SYNC_BYTE               'S'

#define EAU_COMMAND_LENGTH          (1)
#define EAU_SYNC_WORD_LENGTH        (1)
#define EAU_CRC_VALUE_LENGTH        (2)
#define EAU_RESEND_IF_CRC_ERROR     (1)

#define EAU_SEND_DATA_LENGTH        (32 * 48)
#define EAU_SEND_FARMA_LENGTH       (32)
#define EAU_RECV_FARMA_LENGTH       (32)

#define OK_RESPONSE                 (0x00)
//“Ï≥£∂®“Â
#define ERROR_TIME_OUT              (0x01)
#define ERROR_STREAM_FULL           (0x02)
#define ERROR_CRC                   (0x03)
#define ERROR_RESPONSE              (0x04)
#define ERROR_CMD                   (0x05)
#define ERROR_PARAMETER             (0x06)
#define ERROR_INIT                  (0x07)
#define ERROR_OTHER                 (0x08)
#define ERROR_PROMODE               (0x09)
#define ERROR_DEVICE_TIME_OUT       (0x0A)
#define ERROR_START                 (0x0B)
#define ERROR_DMABUSY               (0x0C)

#define SPIDATABUFMAXLEN            (64)

typedef uint32_t DecodeTime;

typedef enum _SSPP_CMD
{
    EAU_CMD_UNKOWN = 0,
    EAU_CMD_START,
    EAU_CMD_PAUSE,
    EAU_CMD_RESUME,
    EAU_CMD_STOP,
    EAU_CMD_MUTE,
    EAU_CMD_UNMUTE,
    EAU_CMD_DATA,
    EAU_CMD_WRITE_DATA = EAU_CMD_DATA,
    EAU_CMD_VOL_SET,
    EAU_CMD_VOL_ADD,
    EAU_CMD_VOL_SUB,
    EAU_CMD_INFO_GET,
    EAU_CMD_INFO_SET,
    EAU_CMD_TIME_REST,
    EAU_CMD_TIME_GET,
    EAU_CMD_VERSION_GET,
    EAU_CMD_ENCODE_MODE,
    EAU_CMD_DECODE_MODE,
    EAU_CMD_READ_DATA,
    EAU_CMD_LIN_SWITCH,
    EAU_CMD_MIC_SWITCH,
    EAU_CMD_ENCODE_START,
}SSPP_CMD;

typedef struct _MASTERCmdContext
{
    uint8_t  SyncWord;
    uint8_t  Command;
    uint16_t Content;
    uint16_t CrcValue;
}MASTERCmdContext,EAUCmdContext;

typedef struct _EAUCmdResponseContext
{
    uint8_t  SyncWord;
    uint8_t  Command;
    uint16_t Response;
    uint16_t CrcValue;
}EAUCmdResponseContext;


typedef struct _DataContext
{
    uint8_t  SyncWord;
    uint8_t  data[EAU_SEND_DATA_LENGTH+EAU_CRC_VALUE_LENGTH];
}EAUDataContext;

typedef enum __EAU_CMD_RESPONSE
{
    EAU_CMD_RESP_UNKOWN = 0,
    EAU_CMD_RESP_OKCMD,
    EAU_CMD_RESP_ERCMD,
    EAU_CMD_RESP_OKSEND,
    EAU_CMD_RESP_RESEND,
    EAU_CMD_RESP_NEXTSEND,
    EAU_CMD_RESP_INITERROR,
    EAU_CMD_RESP_STATEERROR,
}EAU_CMD_RESPONSE;

/**
 * @ly Audio encoder type Ex
 */
typedef enum _EncoderTypeEx
{
    OPUS_ENCODER_EX = 1,        /**<   OPUS encoder */
    PCM_ENCODER_EX,             /**<   PCM  encoder */
} EncoderTypeEx;

/**
 * @ly Audio Decoder Type Set
 */
typedef enum _DecoderType
{
    UNKOWN_DECODER               = 0,       /**< Unknown decoder                     */
    PURE_SOFTWARE_DECODERS       = 1,       /**< Pure software decoders as follows   */
    WAV_DECODER,                            /**< WAV  decoder */
    FLAC_DECODER,                           /**< FLAC decoder */
    AAC_DECODER,                            /**< AAC  decoder */
    AIF_DECODER,                            /**< AIFF and AIFC decoder */
    ALAC_DECODER,                           /**< ALAC and AIFC decoder */
    OPUS_DECODER,                           /**< OPUS decoder */
    PCM_DECODER,                            /**< PCM  decoder */
    WITH_HARDWARE_DECODERS       = 128,     /**< Decoders with hardware as follows   */
    MP3_DECODER,                            /**< MP3  decoder */
    WMA_DECODER,                            /**< WAM  decoder */
    SBC_DECODER,                            /**< SBC  decoder */
} DecoderType;

/**
 * @ly Song Information
 */
typedef struct _SongInformation
{
    int32_t     stream_type;            /**< Stream type, must be in @code StreamTypeSet    */
    uint32_t    num_channels;           /**< Number of channels                             */
    uint32_t    sampling_rate;          /**< Sampling rate, unit: Hz                        */
    uint32_t    bitrate;                /**< Bit rate, unit:bps                             */
    uint32_t    duration;               /**< Total play time, unit:ms                       */
    uint32_t    file_size;              /**< Song file size in bytes .                      */
    uint16_t    firmware_ver;           /**< AP80xx firmware version .                      */
    uint16_t    firmware_num;           /**< AP80xx firmware number  .                      */
    uint8_t     Stream_buf_is_Empty;    /**< Stream_buf_is_Empty                     .      */
    uint8_t     RFU;                    /**< RFU                     .                      */
} SongInformation;

/**
 * @ly code Information
 */
typedef struct _setdeccodeInfo
{
    int32_t     streamDirection;        /**< Stream direction                               */
    uint32_t    numChannels;            /**< Number of channels                             */    
    uint32_t    sampleRate;             /**< Bit rate, unit:bps                             */
    uint32_t    decoderType;            /**< Decoder Type:OPUS,PCM                          */
    uint32_t    frameSize;              /**< opus and pcm                                   */
    uint32_t    RFU2;                   /**< RFU                                            */
    uint32_t    RFU3;                   /**< RFU                                            */
    uint32_t    RFU4;                   /**< RFU                                            */
    uint32_t    RFU5;                   /**< RFU                                            */
    uint32_t    RFU6;                   /**< RFU                                            */
    uint32_t    RFU7;                   /**< RFU                                            */
} setDecodeInfo;

/**
 * @ly encode Information
 */
typedef struct _set_enccode_info
{
    int32_t         streamDirection;        /**< Stream direction                               */
    uint32_t        numChannels;            /**< Number of channels                             */
    uint32_t        sampleRate;             /**< Sample rate, unit:bps                          */
    uint32_t        encoderType;            /**< Encoder Type:OPUS,PCM                          */
    uint32_t        frameSize;              /**< RFU                                            */
    uint32_t        RFU2;                   /**< RFU                                            */
    uint32_t        RFU3;                   /**< RFU                                            */
    uint32_t        RFU4;                   /**< RFU                                            */
    uint32_t        RFU5;                   /**< RFU                                            */
    uint32_t        RFU6;                   /**< RFU                                            */
    uint32_t        RFU7;                   /**< RFU                                            */
}setEnccodeInfo;

//public function
void apInit();
uint8_t apPlayStop(void);
uint8_t apPlayPause(void);
uint8_t apLinSwitch(void);
uint8_t apMicSwitch(void);
uint8_t apPlayResume(void);
uint8_t apEncodeStart(void);
uint8_t apVolSet(uint16_t vol);
uint8_t apSetEncodeMode(void);
uint8_t apSetDecodeMode(void);
uint8_t apVolMuteEnable(void);
uint8_t apVolMuteDisable(void);
uint8_t apRestDecodeTime(void);
uint8_t apSetWaitTime(uint32_t waitTime);
uint8_t apGetDecodeTime(DecodeTime *time);
uint8_t apGetInfo(SongInformation *songInfo);
uint8_t apSetDecodeInfo(setDecodeInfo *decodeInfo);
uint8_t apSetEncodeInfo(setEnccodeInfo *encodeInfo);
uint8_t apSendData(uint8_t *sendData, uint16_t  sendLen);
uint8_t apMicReadData(uint8_t *readData, uint16_t  *readLen);
uint8_t apPlayStart(const char *songFileName, uint8_t len, DecoderType decType);

//private function
void audio_test_main();
void mic_test();
void audio_test_main();
void apSetResponse(bool responseEnable);
void printfByteS(uint8_t *indata, uint16_t len);

#endif //  __AP80XX_H__
