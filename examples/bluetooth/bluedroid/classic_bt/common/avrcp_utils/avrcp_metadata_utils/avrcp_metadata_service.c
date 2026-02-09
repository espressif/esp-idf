/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include "esp_log.h"
#include "esp_avrc_api.h"
#include "avrcp_metadata_service.h"
#include "avrcp_common_utils.h"

/* tags*/
#define RC_MD_SRV_TAG  "RC_MD_SRV"

typedef struct {
    /* AVRCP metadata response data from ESP_AVRC_CT_METADATA_RSP_EVT */
    avrc_metadata_srv_param_t attr_title;
    avrc_metadata_srv_param_t attr_artist;
    avrc_metadata_srv_param_t attr_album;
    avrc_metadata_srv_param_t attr_genre;
} avrc_metadata_srv_cb_t;

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

/* AVRCP metadata service control block */
static avrc_metadata_srv_cb_t s_avrc_metadata_srv_cb;

/********************************
 * STATIC FUNCTION DEFINITIONS
 *******************************/

static void avrc_metadata_srv_copy_metadata(avrc_metadata_srv_param_t *p_dest, avrc_metadata_srv_param_t *p_src)
{
    p_dest->attr_id = p_src->attr_id;
    p_dest->attr_length = p_src->attr_length;

    if (p_dest->attr_text) {
        free(p_dest->attr_text);
        p_dest->attr_text = NULL;
    }

    p_dest->attr_text = (uint8_t *)malloc(p_dest->attr_length + 1);
    if (p_dest->attr_text == NULL) {
        ESP_LOGE(RC_MD_SRV_TAG, "Memory allocation failed.");
        return;
    }

    memcpy(p_dest->attr_text, p_src->attr_text, p_dest->attr_length);
    p_dest->attr_text[p_dest->attr_length] = 0;
}

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void avrc_metadata_srv_open(void)
{
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_TITLE);
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_ARTIST);
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_ALBUM);
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_GENRE);

    memset(&s_avrc_metadata_srv_cb, 0, sizeof(avrc_metadata_srv_cb_t));
}

void avrc_metadata_srv_close(void)
{
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_TITLE);
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_ARTIST);
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_ALBUM);
    avrc_metadata_srv_md_free(ESP_AVRC_MD_ATTR_GENRE);

    memset(&s_avrc_metadata_srv_cb, 0, sizeof(avrc_metadata_srv_cb_t));
}

void avrc_metadata_srv_md_req(void)
{
    /* request metadata */
    uint8_t attr_mask = ESP_AVRC_MD_ATTR_TITLE |
                        ESP_AVRC_MD_ATTR_ARTIST |
                        ESP_AVRC_MD_ATTR_ALBUM |
                        ESP_AVRC_MD_ATTR_GENRE;

    /* send metadata request */
    esp_avrc_ct_send_metadata_cmd(bt_avrc_common_alloc_tl(), attr_mask);
}

void avrc_metadata_srv_md_save(avrc_metadata_srv_param_t *param)
{
    avrc_metadata_srv_param_t *rc = param;

    switch (rc->attr_id) {
    case ESP_AVRC_MD_ATTR_TITLE: {
        avrc_metadata_srv_copy_metadata(&s_avrc_metadata_srv_cb.attr_title, rc);
        break;
    }
    case ESP_AVRC_MD_ATTR_ARTIST: {
        avrc_metadata_srv_copy_metadata(&s_avrc_metadata_srv_cb.attr_artist, rc);
        break;
    }
    case ESP_AVRC_MD_ATTR_ALBUM: {
        avrc_metadata_srv_copy_metadata(&s_avrc_metadata_srv_cb.attr_album, rc);
        break;
    }
    case ESP_AVRC_MD_ATTR_GENRE: {
        avrc_metadata_srv_copy_metadata(&s_avrc_metadata_srv_cb.attr_genre, rc);
        break;
    }

    default:
        ESP_LOGW(RC_MD_SRV_TAG, "%s unhandled attr_id: %d", __func__, rc->attr_id);
        break;
    }
}

void avrc_metadata_srv_md_free(uint8_t attr_id)
{
    switch (attr_id) {
    case ESP_AVRC_MD_ATTR_TITLE: {
        if (s_avrc_metadata_srv_cb.attr_title.attr_text) {
            free(s_avrc_metadata_srv_cb.attr_title.attr_text);
            s_avrc_metadata_srv_cb.attr_title.attr_text = NULL;
        }
        break;
    }
    case ESP_AVRC_MD_ATTR_ARTIST: {
        if (s_avrc_metadata_srv_cb.attr_artist.attr_text) {
            free(s_avrc_metadata_srv_cb.attr_artist.attr_text);
            s_avrc_metadata_srv_cb.attr_artist.attr_text = NULL;
        }
        break;
    }
    case ESP_AVRC_MD_ATTR_ALBUM: {
        if (s_avrc_metadata_srv_cb.attr_album.attr_text) {
            free(s_avrc_metadata_srv_cb.attr_album.attr_text);
            s_avrc_metadata_srv_cb.attr_album.attr_text = NULL;
        }
        break;
    }
    case ESP_AVRC_MD_ATTR_GENRE: {
        if (s_avrc_metadata_srv_cb.attr_genre.attr_text) {
            free(s_avrc_metadata_srv_cb.attr_genre.attr_text);
            s_avrc_metadata_srv_cb.attr_genre.attr_text = NULL;
        }
        break;
    }

    default:
        ESP_LOGW(RC_MD_SRV_TAG, "%s unhandled attr_id: %d", __func__, attr_id);
        break;
    }
}
