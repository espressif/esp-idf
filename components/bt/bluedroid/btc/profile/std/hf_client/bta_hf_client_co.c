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
#if (BTA_HF_INCLUDED == TRUE)

#if (BTM_SCO_HCI_INCLUDED == TRUE)
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
** Function         bta_hf_client_sco_co_open
**
** Description      This function is executed when a SCO connection is open.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_sco_co_open(UINT16 handle, UINT8 pkt_size, UINT16 event)
{
    APPL_TRACE_EVENT("%s hdl %x, pkt_sz %u, event %u", __FUNCTION__, handle,
                     pkt_size, event);
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
uint32_t bta_hf_client_sco_co_out_data(uint8_t *p_buf, uint32_t sz)
{
    return btc_hf_client_outgoing_data_cb_to_app(p_buf, sz);
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
    UINT8       *p = (UINT8 *)(p_buf + 1) + p_buf->offset;
    UINT8       pkt_size = 0;

    STREAM_SKIP_UINT16(p);
    STREAM_TO_UINT8 (pkt_size, p);
    btc_hf_client_incoming_data_cb_to_app(p, pkt_size);
}

#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */

#endif /* #if (BTA_HF_INCLUDED == TRUE) */
