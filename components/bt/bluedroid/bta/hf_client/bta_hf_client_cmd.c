/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2003-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include "bta_hf_client_int.h"
#include "stdio.h"
#include "common/bt_target.h"

#if (BTA_HF_INCLUDED == TRUE)

void bta_hf_client_send_at_cmd(tBTA_HF_CLIENT_DATA *p_data)
{
    tBTA_HF_CLIENT_DATA_VAL *p_val = (tBTA_HF_CLIENT_DATA_VAL *)p_data;

    switch (p_val->uint8_val) {
    case BTA_HF_CLIENT_AT_CMD_VTS:
        bta_hf_client_send_at_vts((char)p_val->uint32_val1);
        break;
    case BTA_HF_CLIENT_AT_CMD_BTRH:
        bta_hf_client_send_at_btrh(FALSE, p_val->uint32_val1);
        break;
    case BTA_HF_CLIENT_AT_CMD_CHUP:
        bta_hf_client_send_at_chup();
        break;
    case BTA_HF_CLIENT_AT_CMD_CHLD:
        /* expects ascii code for command */
        bta_hf_client_send_at_chld('0' + p_val->uint32_val1, p_val->uint32_val2);
        break;
    case BTA_HF_CLIENT_AT_CMD_BCC:
        bta_hf_client_send_at_bcc();
        break;
    case BTA_HF_CLIENT_AT_CMD_CNUM:
        bta_hf_client_send_at_cnum();
        break;
    case BTA_HF_CLIENT_AT_CMD_ATA:
        bta_hf_client_send_at_ata();
        break;
    case BTA_HF_CLIENT_AT_CMD_COPS:
        bta_hf_client_send_at_cops(TRUE);
        break;
    case BTA_HF_CLIENT_AT_CMD_ATD:
        bta_hf_client_send_at_atd(p_val->str, p_val->uint32_val1);
        break;
    case BTA_HF_CLIENT_AT_CMD_VGM:
        bta_hf_client_send_at_vgm(p_val->uint32_val1);
        break;
    case BTA_HF_CLIENT_AT_CMD_VGS:
        bta_hf_client_send_at_vgs(p_val->uint32_val1);
        break;
    case BTA_HF_CLIENT_AT_CMD_BVRA:
        bta_hf_client_send_at_bvra(p_val->uint32_val1 == 0 ? FALSE : TRUE);
        break;
    case BTA_HF_CLIENT_AT_CMD_CLCC:
        bta_hf_client_send_at_clcc();
        break;
    case BTA_HF_CLIENT_AT_CMD_BINP:
        bta_hf_client_send_at_binp(p_val->uint32_val1);
        break;
    case BTA_HF_CLIENT_AT_CMD_BLDN:
        bta_hf_client_send_at_bldn();
        break;
    case BTA_HF_CLIENT_AT_CMD_NREC:
        bta_hf_client_send_at_nrec();
        break;
    default:
        APPL_TRACE_ERROR("Default case, %s", __FUNCTION__);
        break;
    }
}
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
