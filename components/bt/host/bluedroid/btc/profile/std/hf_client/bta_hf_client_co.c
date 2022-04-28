/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bta/bta_hf_client_co.h"
#include "hci/hci_audio.h"
#include "btc_hf_client.h"
#include "osi/allocator.h"
#include <string.h>

#if (BTA_HF_INCLUDED == TRUE)

#if (BTM_SCO_HCI_INCLUDED == TRUE)

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
#define HF_SBC_DEC_RAW_DATA_SIZE        240
#define HF_SBC_ENC_RAW_DATA_SIZE        240

/* BTA-HF-CO control block to map bdaddr to BTA handle */
typedef struct
{
    OI_CODEC_SBC_DECODER_CONTEXT    decoder_context;
    OI_UINT32                       decoder_context_data[HF_SBC_DEC_CONTEXT_DATA_LEN];
    OI_INT16                        decode_raw_data[HF_SBC_DEC_RAW_DATA_SIZE];

    SBC_ENC_PARAMS                  encoder;

    UINT8                           sequence_number;
    bool                            is_bad_frame;
    bool                            decode_first_pkt;
    OI_BYTE                         decode_msbc_data[BTM_MSBC_FRAME_SIZE];
    bool                            encode_first_pkt;
    OI_BYTE                         encode_msbc_data[BTM_MSBC_FRAME_SIZE];
} bta_hf_client_co_cb_t;

#if HFP_DYNAMIC_MEMORY == FALSE
static bta_hf_client_co_cb_t bta_hf_client_co_cb;
#else
static bta_hf_client_co_cb_t *bta_hf_client_co_cb_ptr;
#define bta_hf_client_co_cb (*bta_hf_client_co_cb_ptr)
#endif /* HFP_DYNAMIC_MEMORY == FALSE */

static UINT8 hf_air_mode = BTM_SCO_AIR_MODE_UNKNOWN;
static UINT8 hf_inout_pkt_size = 0;

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

    status = OI_CODEC_SBC_DecoderReset(&bta_hf_client_co_cb.decoder_context, bta_hf_client_co_cb.decoder_context_data,
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
    bta_hf_client_co_cb.sequence_number = 0;
    bta_hf_client_co_cb.decode_first_pkt = true;
    bta_hf_client_co_cb.encode_first_pkt = true;
    bta_hf_client_co_cb.is_bad_frame =  false;

    bta_hf_client_co_cb.encoder.sbc_mode = SBC_MODE_MSBC;
    bta_hf_client_co_cb.encoder.s16NumOfBlocks    = 15;
    bta_hf_client_co_cb.encoder.s16NumOfSubBands  = 8;
    bta_hf_client_co_cb.encoder.s16AllocationMethod = SBC_LOUDNESS;
    bta_hf_client_co_cb.encoder.s16BitPool   = 26;
    bta_hf_client_co_cb.encoder.s16ChannelMode = SBC_MONO;
    bta_hf_client_co_cb.encoder.s16NumOfChannels = 1;
    bta_hf_client_co_cb.encoder.s16SamplingFreq = SBC_sf16000;

    SBC_Encoder_Init(&(bta_hf_client_co_cb.encoder));
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
void bta_hf_client_sco_co_open(UINT16 handle, UINT8 air_mode, UINT8 inout_pkt_size, UINT16 event)
{
    APPL_TRACE_EVENT("%s hdl %x, pkt_sz %u, event %u", __FUNCTION__, handle,
                     inout_pkt_size, event);

    hf_air_mode = air_mode;
    hf_inout_pkt_size = inout_pkt_size;

    if (air_mode == BTM_SCO_AIR_MODE_TRANSPNT) {
#if (HFP_DYNAMIC_MEMORY == TRUE)
        bta_hf_client_co_cb_ptr = osi_calloc(sizeof(bta_hf_client_co_cb_t));
        if (!bta_hf_client_co_cb_ptr) {
            APPL_TRACE_ERROR("%s allocate failed", __FUNCTION__);
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
    } else {
        return;
        // Nothing to do
    }

#if (HFP_DYNAMIC_MEMORY == TRUE)
error_exit:;
    hf_air_mode = BTM_SCO_AIR_MODE_UNKNOWN;
    hf_inout_pkt_size = 0;

    if (bta_hf_client_co_cb_ptr) {
        osi_free(bta_hf_client_co_cb_ptr);
        bta_hf_client_co_cb_ptr = NULL;
    }

#if (PLC_INCLUDED == TRUE)
    if (bta_hf_ct_plc_ptr) {
        osi_free(bta_hf_ct_plc_ptr);
        bta_hf_ct_plc_ptr = NULL;
    }
#endif  ///(PLC_INCLUDED == TRUE)
#endif  /// (HFP_DYNAMIC_MEMORY == TRUE)
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
        bta_hf_ct_plc_ptr = NULL;
#endif  /// (HFP_DYNAMIC_MEMORY == TRUE)

#endif  ///(PLC_INCLUDED == TRUE)

#if (HFP_DYNAMIC_MEMORY == TRUE)
        osi_free(bta_hf_client_co_cb_ptr);
        bta_hf_client_co_cb_ptr = NULL;
#endif /* HFP_DYNAMIC_MEMORY == TRUE */
    } else {
        // Nothing to do
    }

    hf_air_mode = BTM_SCO_AIR_MODE_UNKNOWN;
    hf_inout_pkt_size = 0;
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
    UINT8 h2_header_sn0 = bta_hf_client_co_cb.sequence_number & BTA_HF_H2_HEADER_BIT0_MASK;
    UINT8 h2_header_sn1 = bta_hf_client_co_cb.sequence_number & BTA_HF_H2_HEADER_BIT1_MASK;
    h2_header |= (h2_header_sn0 << BTA_HF_H2_HEADER_SN0_BIT_OFFSET1
                | h2_header_sn0 << BTA_HF_H2_HEADER_SN0_BIT_OFFSET2
                | h2_header_sn1 << (BTA_HF_H2_HEADER_SN1_BIT_OFFSET1 - 1)
                | h2_header_sn1 << (BTA_HF_H2_HEADER_SN1_BIT_OFFSET2 - 1)
                );

    bta_hf_client_co_cb.sequence_number++;
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
        uint32_t hf_raw_pkt_size = hf_inout_pkt_size;
        return btc_hf_client_outgoing_data_cb_to_app(p_buf, hf_raw_pkt_size);
    } else if (hf_air_mode == BTM_SCO_AIR_MODE_TRANSPNT) {
        // mSBC
#if (HFP_DYNAMIC_MEMORY == TRUE)
        if(bta_hf_client_co_cb_ptr == NULL) {
            return 0;
        }
#endif /* HFP_DYNAMIC_MEMORY == TRUE */
        if (hf_inout_pkt_size == BTM_MSBC_FRAME_SIZE / 2) {
            if (bta_hf_client_co_cb.encode_first_pkt){
                UINT32 size = btc_hf_client_outgoing_data_cb_to_app((UINT8 *)bta_hf_client_co_cb.encoder.as16PcmBuffer, HF_SBC_ENC_RAW_DATA_SIZE);
                if (size != HF_SBC_ENC_RAW_DATA_SIZE){
                    return 0;
                }

                bta_hf_client_h2_header((UINT16 *)bta_hf_client_co_cb.encode_msbc_data);
                bta_hf_client_co_cb.encoder.pu8Packet = bta_hf_client_co_cb.encode_msbc_data + 2;

                SBC_Encoder(&bta_hf_client_co_cb.encoder);
                memcpy(p_buf, bta_hf_client_co_cb.encode_msbc_data, hf_inout_pkt_size);
                bta_hf_client_co_cb.encode_first_pkt = !bta_hf_client_co_cb.encode_first_pkt;
                return hf_inout_pkt_size;
            } else {
                memcpy(p_buf, bta_hf_client_co_cb.encode_msbc_data + hf_inout_pkt_size, hf_inout_pkt_size);
                bta_hf_client_co_cb.encode_first_pkt = !bta_hf_client_co_cb.encode_first_pkt;
                return hf_inout_pkt_size;
            }

        } else if (hf_inout_pkt_size == BTM_MSBC_FRAME_SIZE) {
            UINT32 size = btc_hf_client_outgoing_data_cb_to_app((UINT8 *)bta_hf_client_co_cb.encoder.as16PcmBuffer, HF_SBC_ENC_RAW_DATA_SIZE);
            if (size != HF_SBC_ENC_RAW_DATA_SIZE){
                return 0;
            }

            bta_hf_client_h2_header((UINT16 *)p_buf);
            bta_hf_client_co_cb.encoder.pu8Packet = p_buf + 2;

            SBC_Encoder(&bta_hf_client_co_cb.encoder);
            return hf_inout_pkt_size;
        } else {
            //Never run to here.
        }



    } else {
        APPL_TRACE_ERROR("%s invaild air mode: %d", __FUNCTION__, hf_air_mode);
    }
    return 0;
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
    UINT8 zero_signal_frame_len = BTM_MSBC_FRAME_DATA_SIZE;
    UINT32 sbc_raw_data_size = HF_SBC_DEC_RAW_DATA_SIZE;

    if (is_bad_frame){
        status = OI_CODEC_SBC_CHECKSUM_MISMATCH;
    } else {
        status = OI_CODEC_SBC_DecodeFrame(&bta_hf_client_co_cb.decoder_context, (const OI_BYTE **)data,
                                          (OI_UINT32 *)length,
                                          (OI_INT16 *)bta_hf_client_co_cb.decode_raw_data,
                                          (OI_UINT32 *)&sbc_raw_data_size);
    }

// PLC_INCLUDED will be set to TRUE when enabling Wide Band Speech
#if (PLC_INCLUDED == TRUE)
    switch(status){
        case OI_OK:
            bta_hf_ct_plc.first_good_frame_found = TRUE;
            sbc_plc_good_frame(&(bta_hf_ct_plc.plc_state), (int16_t *)bta_hf_client_co_cb.decode_raw_data, bta_hf_ct_plc.sbc_plc_out);
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
            sbc_raw_data_size = HF_SBC_DEC_RAW_DATA_SIZE;
            status = OI_CODEC_SBC_DecodeFrame(&bta_hf_client_co_cb.decoder_context, &zero_signal_frame_data,
                                                (OI_UINT32 *)&zero_signal_frame_len,
                                                (OI_INT16 *)bta_hf_client_co_cb.decode_raw_data,
                                                (OI_UINT32 *)&sbc_raw_data_size);
            sbc_plc_bad_frame(&(bta_hf_ct_plc.plc_state), bta_hf_client_co_cb.decode_raw_data, bta_hf_ct_plc.sbc_plc_out);
            APPL_TRACE_DEBUG("bad frame, using PLC to fix it.");
            break;
        case OI_STATUS_INVALID_PARAMETERS:
            // This caused by corrupt frames.
            // The codec apparently does not recover from this.
            // Re-initialize the codec.
            APPL_TRACE_ERROR("Frame decode error: OI_STATUS_INVALID_PARAMETERS");

            if (!OI_SUCCESS(OI_CODEC_SBC_DecoderReset(&bta_hf_client_co_cb.decoder_context, bta_hf_client_co_cb.decoder_context_data,
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
        btc_hf_client_incoming_data_cb_to_app((const uint8_t *)(bta_hf_ct_plc.sbc_plc_out), sbc_raw_data_size);
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

    STREAM_SKIP_UINT16(p);
    STREAM_TO_UINT8 (pkt_size, p);

    if (hf_air_mode == BTM_SCO_AIR_MODE_CVSD) {
        // CVSD
        if(status != BTM_SCO_DATA_CORRECT){
            APPL_TRACE_DEBUG("%s: not a correct frame(%d).", __func__, status);
        }
        btc_hf_client_incoming_data_cb_to_app(p, pkt_size);
    } else if (hf_air_mode == BTM_SCO_AIR_MODE_TRANSPNT) {
        // mSBC
        UINT8 *data = NULL;

        if (pkt_size != hf_inout_pkt_size) {
            bta_hf_client_co_cb.is_bad_frame = true;
        }
        if (status != BTM_SCO_DATA_CORRECT) {
            bta_hf_client_co_cb.is_bad_frame = true;
        }
        if (hf_inout_pkt_size == BTM_MSBC_FRAME_SIZE / 2) {
            if (bta_hf_client_co_cb.decode_first_pkt){
                if (!bta_hf_client_co_cb.is_bad_frame){
                    memcpy(bta_hf_client_co_cb.decode_msbc_data, p, pkt_size);
                }
            } else {
                if (!bta_hf_client_co_cb.is_bad_frame){
                    memcpy(bta_hf_client_co_cb.decode_msbc_data + BTM_MSBC_FRAME_SIZE / 2, p, pkt_size);
                }

                data = bta_hf_client_co_cb.decode_msbc_data;
                bta_hf_client_decode_msbc_frame(&data, &pkt_size, bta_hf_client_co_cb.is_bad_frame);
                bta_hf_client_co_cb.is_bad_frame = false;
            }
            bta_hf_client_co_cb.decode_first_pkt = !bta_hf_client_co_cb.decode_first_pkt;

        } else if (hf_inout_pkt_size == BTM_MSBC_FRAME_SIZE) {
            data = p;
            bta_hf_client_decode_msbc_frame(&data, &pkt_size, bta_hf_client_co_cb.is_bad_frame);
            bta_hf_client_co_cb.is_bad_frame = false;
        } else {
            //Never run to here.
        }
    } else {
        APPL_TRACE_ERROR("%s invaild air mode: %d", __FUNCTION__, hf_air_mode);
    }
}

#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */

#endif /* #if (BTA_HF_INCLUDED == TRUE) */
