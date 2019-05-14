// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bta/bta_hf_client_co.h"
#include "hci/hci_audio.h"
#include "btc_hf_client.h"
#include "osi/allocator.h"
#include <string.h>

#if (BTA_HF_INCLUDED == TRUE)

#if (BTM_SCO_HCI_INCLUDED == TRUE)

#include "btm_int.h"
#include "stack/btm_api.h"
#include "oi_codec_sbc.h"
#include "oi_status.h"
#include "sbc_encoder.h"

#if (PLC_INCLUDED == TRUE)
#include "sbc_plc.h"

typedef struct {
    bool first_good_frame_found;
    sbc_plc_state_t plc_state;
    int16_t sbc_plc_out[SBC_FS];
} bta_hf_ct_plc_t;

#if HFP_DYNAMIC_MEMORY == FALSE
static bta_hf_ct_plc_t bta_hf_ct_plc;
#else
static bta_hf_ct_plc_t *bta_hf_ct_plc_ptr;
#define bta_hf_ct_plc (*bta_hf_ct_plc_ptr)
#endif

#endif  ///(PLC_INCLUDED == TRUE)


#define HF_SBC_DEC_CONTEXT_DATA_LEN     (CODEC_DATA_WORDS(1, SBC_CODEC_FAST_FILTER_BUFFERS))
#define HF_SBC_DEC_PCM_DATA_LEN         240
#define HF_SBC_ENC_PCM_DATA_LEN         240

#if HFP_DYNAMIC_MEMORY == FALSE
static OI_CODEC_SBC_DECODER_CONTEXT hf_sbc_decoder_context;
static OI_UINT32 hf_sbc_decoder_context_data[HF_SBC_DEC_CONTEXT_DATA_LEN];
static OI_INT16 hf_sbc_decode_pcm_data[HF_SBC_DEC_PCM_DATA_LEN];

static SBC_ENC_PARAMS hf_sbc_encoder;
#else
static OI_CODEC_SBC_DECODER_CONTEXT *hf_sbc_decoder_context_ptr;
static OI_UINT32 *hf_sbc_decoder_context_data;
static OI_INT16 *hf_sbc_decode_pcm_data;
#define hf_sbc_decoder_context (*hf_sbc_decoder_context_ptr)

static SBC_ENC_PARAMS *hf_sbc_encoder_ptr;
#define hf_sbc_encoder (*hf_sbc_encoder_ptr)
#endif /* HFP_DYNAMIC_MEMORY == FALSE */

static UINT8 hf_sequence_number = 0;
static UINT8 hf_air_mode;
static UINT8 hf_pkt_size;

/*******************************************************************************
**
** Function         bta_hf_client_co_audio_state
**
** Description      This function is called by the HF CLIENT before the audio connection
**                  is brought up, after it comes up, and after it goes down.
**
** Parameters       handle - handle of the AG instance
**                  state - Audio state
**                  codec - if WBS support is compiled in, codec to going to be used is provided
**                      and when in SCO_STATE_SETUP, BTM_I2SPCMConfig() must be called with
**                      the correct platform parameters.
**                      in the other states codec type should not be ignored
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_co_audio_state(UINT16 handle, UINT8 state, tBTA_HFP_PEER_CODEC codec)
{
    switch (state)
    {
    case SCO_STATE_ON:
    case SCO_STATE_OFF:
    case SCO_STATE_OFF_TRANSFER:
    case SCO_STATE_SETUP:
    default:
        break;
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_sco_co_init
**
** Description      This function can be used by the phone to initialize audio
**                  codec or for other initialization purposes before SCO connection
**                  is opened.
**
**
** Returns          Void.
**
*******************************************************************************/
tBTA_HFP_SCO_ROUTE_TYPE bta_hf_client_sco_co_init(UINT32 rx_bw, UINT32 tx_bw,
        tBTA_HFP_CODEC_INFO *p_codec_info, UINT8 app_id)
{
    APPL_TRACE_EVENT("%s rx_bw %d, tx_bw %d, codec %d", __FUNCTION__, rx_bw, tx_bw,
                     p_codec_info->codec_type);
    return BTA_HFP_SCO_ROUTE_HCI;
}

/*******************************************************************************
 **
 ** Function       bta_hf_dec_init
 **
 ** Description    Initialize decoding task
 **
 ** Returns        void
 **
 *******************************************************************************/
static void bta_hf_dec_init(void) {
#if (PLC_INCLUDED == TRUE)
    sbc_plc_init(&(bta_hf_ct_plc.plc_state));
#endif  ///(PLC_INCLUDED == TRUE)

    OI_STATUS status;

    status = OI_CODEC_SBC_DecoderReset(&hf_sbc_decoder_context, hf_sbc_decoder_context_data,
                                       HF_SBC_DEC_CONTEXT_DATA_LEN * sizeof(OI_UINT32), 1, 1, FALSE, TRUE);
    if (!OI_SUCCESS(status)) {
        APPL_TRACE_ERROR("OI_CODEC_SBC_DecoderReset failed with error code %d\n", status);
    }
}

/*******************************************************************************
 **
 ** Function       bta_hf_enc_init
 **
 ** Description    Initialize encoding task
 **
 ** Returns        void
 **
 *******************************************************************************/
static void bta_hf_enc_init(void) {
    hf_sequence_number = 0;

    hf_sbc_encoder.sbc_mode = SBC_MODE_MSBC;
    hf_sbc_encoder.s16NumOfBlocks    = 15;
    hf_sbc_encoder.s16NumOfSubBands  = 8;
    hf_sbc_encoder.s16AllocationMethod = SBC_LOUDNESS;
    hf_sbc_encoder.s16BitPool   = 26;
    hf_sbc_encoder.s16ChannelMode = SBC_MONO;
    hf_sbc_encoder.s16NumOfChannels = 1;
    hf_sbc_encoder.s16SamplingFreq = SBC_sf16000;

    SBC_Encoder_Init(&(hf_sbc_encoder));
}

/*******************************************************************************
**
** Function         bta_hf_client_sco_co_open
**
** Description      This function is executed when a SCO connection is open.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_sco_co_open(UINT16 handle, UINT8 air_mode, UINT8 pkt_size, UINT16 event)
{
    APPL_TRACE_EVENT("%s hdl %x, pkt_sz %u, event %u", __FUNCTION__, handle,
                     pkt_size, event);

    hf_air_mode = air_mode;
    hf_pkt_size = pkt_size;

    if (air_mode == BTM_SCO_AIR_MODE_TRANSPNT) {
#if (HFP_DYNAMIC_MEMORY == TRUE)
        hf_sbc_decoder_context_ptr = osi_calloc(sizeof(OI_CODEC_SBC_DECODER_CONTEXT));
        hf_sbc_decoder_context_data = osi_calloc(HF_SBC_DEC_CONTEXT_DATA_LEN * sizeof(OI_UINT32));
        hf_sbc_decode_pcm_data = osi_calloc(HF_SBC_DEC_PCM_DATA_LEN * sizeof(OI_INT16));
        if (!hf_sbc_decoder_context_ptr || !hf_sbc_decoder_context_data || !hf_sbc_decode_pcm_data) {
            APPL_TRACE_ERROR("%s failed to allocate SBC decoder", __FUNCTION__);
            goto error_exit;
        }

        hf_sbc_encoder_ptr = osi_calloc(sizeof(SBC_ENC_PARAMS));

        if (!hf_sbc_encoder_ptr) {
            APPL_TRACE_ERROR("%s failed to allocate SBC encoder", __FUNCTION__);
            goto error_exit;
        }

#if (PLC_INCLUDED == TRUE)
        bta_hf_ct_plc_ptr = (bta_hf_ct_plc_t *)osi_calloc(sizeof(bta_hf_ct_plc_t));
            if (!bta_hf_ct_plc_ptr) {
            APPL_TRACE_ERROR("%s malloc fail.", __FUNCTION__);
            goto error_exit;
        }
#endif  ///(PLC_INCLUDED == TRUE)

#endif  /// (HFP_DYNAMIC_MEMORY == TRUE)

    bta_hf_dec_init();
    bta_hf_enc_init();

    return;

error_exit:;
#if (HFP_DYNAMIC_MEMORY == TRUE)
        if (hf_sbc_decoder_context_ptr) {
            osi_free(hf_sbc_decoder_context_ptr);
            hf_sbc_decoder_context_ptr = NULL;
        }
        if (hf_sbc_decoder_context_data) {
            osi_free(hf_sbc_decoder_context_data);
            hf_sbc_decoder_context_data = NULL;
        }
        if (hf_sbc_decode_pcm_data) {
            osi_free(hf_sbc_decode_pcm_data);
            hf_sbc_decode_pcm_data = NULL;
        }
        if (hf_sbc_encoder_ptr) {
            osi_free(hf_sbc_encoder_ptr);
            hf_sbc_encoder_ptr = NULL;
        }

#if (PLC_INCLUDED == TRUE)
        if (bta_hf_ct_plc_ptr) {
            osi_free(bta_hf_ct_plc_ptr);
            bta_hf_ct_plc_ptr = NULL;
        }
#endif  ///(PLC_INCLUDED == TRUE)

#endif  /// (HFP_DYNAMIC_MEMORY == TRUE)
    } else {
        // Nothing to do
    }

    return;
}

/*******************************************************************************
**
** Function         bta_hf_client_sco_co_close
**
** Description      This function is called when a SCO connection is closed
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_sco_co_close(void)
{
    APPL_TRACE_EVENT("%s", __FUNCTION__);

    if (hf_air_mode == BTM_SCO_AIR_MODE_TRANSPNT) {
#if (PLC_INCLUDED == TRUE)
        sbc_plc_deinit(&(bta_hf_ct_plc.plc_state));
        bta_hf_ct_plc.first_good_frame_found = FALSE;

#if (HFP_DYNAMIC_MEMORY == TRUE)
        osi_free(bta_hf_ct_plc_ptr);
#endif  /// (HFP_DYNAMIC_MEMORY == TRUE)

#endif  ///(PLC_INCLUDED == TRUE)

#if (HFP_DYNAMIC_MEMORY == TRUE)
        osi_free(hf_sbc_decoder_context_ptr);
        hf_sbc_decoder_context_ptr = NULL;

        osi_free(hf_sbc_decoder_context_data);
        hf_sbc_decoder_context_data = NULL;

        osi_free(hf_sbc_decode_pcm_data);
        hf_sbc_decode_pcm_data = NULL;

        osi_free(hf_sbc_encoder_ptr);
        hf_sbc_encoder_ptr = NULL;
#endif /* HFP_DYNAMIC_MEMORY == TRUE */
    } else {
        // Nothing to do
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_h2_header
**
** Description      This function is called to fill in H2 header
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_h2_header(UINT16 *p_buf)
{
    // H2: Header with synchronization word and sequence number
#define BTA_HF_H2_HEADER                0x0801
#define BTA_HF_H2_HEADER_BIT0_MASK   (1 << 0)
#define BTA_HF_H2_HEADER_BIT1_MASK   (1 << 1)
#define BTA_HF_H2_HEADER_SN0_BIT_OFFSET1 12
#define BTA_HF_H2_HEADER_SN0_BIT_OFFSET2 13
#define BTA_HF_H2_HEADER_SN1_BIT_OFFSET1 14
#define BTA_HF_H2_HEADER_SN1_BIT_OFFSET2 15

    UINT16 h2_header = BTA_HF_H2_HEADER;
    UINT8 h2_header_sn0 = hf_sequence_number & BTA_HF_H2_HEADER_BIT0_MASK;
    UINT8 h2_header_sn1 = hf_sequence_number & BTA_HF_H2_HEADER_BIT1_MASK;
    h2_header |= (h2_header_sn0 << BTA_HF_H2_HEADER_SN0_BIT_OFFSET1
                | h2_header_sn0 << BTA_HF_H2_HEADER_SN0_BIT_OFFSET2
                | h2_header_sn1 << (BTA_HF_H2_HEADER_SN1_BIT_OFFSET1 - 1)
                | h2_header_sn1 << (BTA_HF_H2_HEADER_SN1_BIT_OFFSET2 - 1)
                );

    hf_sequence_number++;
    *p_buf = h2_header;
}

/*******************************************************************************
**
** Function         bta_hf_client_sco_co_out_data
**
** Description      This function is called to send SCO data over HCI.
**
** Returns          number of bytes got from application
**
*******************************************************************************/
uint32_t bta_hf_client_sco_co_out_data(UINT8 *p_buf)
{
    if (hf_air_mode == BTM_SCO_AIR_MODE_CVSD) {
        // CVSD
        return btc_hf_client_outgoing_data_cb_to_app(p_buf, hf_pkt_size);
    } else if (hf_air_mode == BTM_SCO_AIR_MODE_TRANSPNT) {
        // mSBC
        UINT32 size = btc_hf_client_outgoing_data_cb_to_app((UINT8 *)hf_sbc_encoder.as16PcmBuffer, HF_SBC_ENC_PCM_DATA_LEN);
        if (size != HF_SBC_ENC_PCM_DATA_LEN){
            return 0;
        }

        bta_hf_client_h2_header((UINT16 *)p_buf);
        hf_sbc_encoder.pu8Packet = p_buf + 2;

        SBC_Encoder(&hf_sbc_encoder);
        if (hf_sbc_encoder.u16PacketLength == BTM_ESCO_DATA_SIZE) {
            return BTM_ESCO_DATA_SIZE_MAX;
        } else {
            return 0;
        }
    } else {
        APPL_TRACE_ERROR("%s invaild air mode: %d", __FUNCTION__, hf_air_mode);
        return 0;
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_decode_msbc_frame
**
** Description      This function is called decode a mSBC frame
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_decode_msbc_frame(UINT8 **data, UINT8 *length, BOOLEAN is_bad_frame){
    OI_STATUS status;
    const OI_BYTE *zero_signal_frame_data;
    UINT8 zero_signal_frame_len = BTM_ESCO_DATA_SIZE;
    UINT32 sbc_frame_len = HF_SBC_DEC_PCM_DATA_LEN;

    if (is_bad_frame){
        status = OI_CODEC_SBC_CHECKSUM_MISMATCH;
    } else {
        status = OI_CODEC_SBC_DecodeFrame(&hf_sbc_decoder_context, (const OI_BYTE **)data,
                                          (OI_UINT32 *)length,
                                          (OI_INT16 *)hf_sbc_decode_pcm_data,
                                          (OI_UINT32 *)&sbc_frame_len);
    }

// PLC_INCLUDED will be set to TRUE when enabling Wide Band Speech
#if (PLC_INCLUDED == TRUE)
    switch(status){
        case OI_OK:
            bta_hf_ct_plc.first_good_frame_found = TRUE;
            sbc_plc_good_frame(&(bta_hf_ct_plc.plc_state), (int16_t *)hf_sbc_decode_pcm_data, bta_hf_ct_plc.sbc_plc_out);
        case OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA:
        case OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA:
        case OI_CODEC_SBC_NOT_ENOUGH_AUDIO_DATA:
            break;
        case OI_CODEC_SBC_NO_SYNCWORD:
        case OI_CODEC_SBC_CHECKSUM_MISMATCH:
            if (!bta_hf_ct_plc.first_good_frame_found) {
                break;
            }
            zero_signal_frame_data = sbc_plc_zero_signal_frame();
            sbc_frame_len = HF_SBC_DEC_PCM_DATA_LEN;
            status = OI_CODEC_SBC_DecodeFrame(&hf_sbc_decoder_context, &zero_signal_frame_data,
                                                (OI_UINT32 *)&zero_signal_frame_len,
                                                (OI_INT16 *)hf_sbc_decode_pcm_data,
                                                (OI_UINT32 *)&sbc_frame_len);

            sbc_plc_bad_frame(&(bta_hf_ct_plc.plc_state), hf_sbc_decode_pcm_data, bta_hf_ct_plc.sbc_plc_out);
            break;
        case OI_STATUS_INVALID_PARAMETERS:
            // This caused by corrupt frames.
            // The codec apparently does not recover from this.
            // Re-initialize the codec.
            APPL_TRACE_ERROR("Frame decode error: OI_STATUS_INVALID_PARAMETERS");

            if (!OI_SUCCESS(OI_CODEC_SBC_DecoderReset(&hf_sbc_decoder_context, hf_sbc_decoder_context_data,
                                       HF_SBC_DEC_CONTEXT_DATA_LEN * sizeof(OI_UINT32), 1, 1, FALSE, TRUE))) {
                APPL_TRACE_ERROR("OI_CODEC_SBC_DecoderReset failed with error code %d\n", status);
            }
            break;
        default:
            APPL_TRACE_ERROR("Frame decode error: %d", status);
            break;
    }
#endif  ///(PLC_INCLUDED == TRUE)

    if (OI_SUCCESS(status)){
        btc_hf_client_incoming_data_cb_to_app((const uint8_t *)(bta_hf_ct_plc.sbc_plc_out), sbc_frame_len);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_sco_co_in_data
**
** Description      This function is called to send incoming SCO data to application.
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_sco_co_in_data(BT_HDR  *p_buf, tBTM_SCO_DATA_FLAG status)
{
    UINT8 *p = (UINT8 *)(p_buf + 1) + p_buf->offset;
    UINT8 pkt_size = 0;
    UINT16 handle, sco_index;

    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT8 (pkt_size, p);

    handle = BTMD_GET_HANDLE (handle);
    sco_index = btm_find_scb_by_handle(handle);
    tSCO_CONN *sco_p = &btm_cb.sco_cb.sco_db[sco_index];

    if (sco_p->esco.data.air_mode == BTM_SCO_AIR_MODE_CVSD) {
        // CVSD
        if(status != BTM_SCO_DATA_CORRECT){
            APPL_TRACE_DEBUG("%s: not a correct frame(%d).", __func__, status);
        }
        btc_hf_client_incoming_data_cb_to_app(p, pkt_size);
    } else if (sco_p->esco.data.air_mode == BTM_SCO_AIR_MODE_TRANSPNT) {
        // mSBC
        BOOLEAN is_bad_frame = false;
        UINT8 msbc_frame_size = BTM_ESCO_DATA_SIZE;
        if (pkt_size < msbc_frame_size) {
            is_bad_frame = true;
        }
        if (status != BTM_SCO_DATA_CORRECT) {
            is_bad_frame = true;
        }
        UINT8 *data = p;
        bta_hf_client_decode_msbc_frame(&data, &pkt_size, is_bad_frame);
    } else {
        APPL_TRACE_ERROR("%s invaild air mode: %d", __FUNCTION__, sco_p->esco.data.air_mode);
    }
}

#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */

#endif /* #if (BTA_HF_INCLUDED == TRUE) */