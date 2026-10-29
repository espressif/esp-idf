/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "sdkconfig.h"
#include "esp_system.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/util_utf8.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/crypto.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/bluetooth/services/ots.h>
#include <zephyr/bluetooth/common/bt_str.h>
#include <zephyr/bluetooth/crypto/bt_crypto.h>

#include <zephyr/bluetooth/audio/aics.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap_lc3_preset.h>
#include <zephyr/bluetooth/audio/bap.h>
#include <zephyr/bluetooth/audio/cap.h>
#include <zephyr/bluetooth/audio/ccp.h>
#include <zephyr/bluetooth/audio/csip.h>
#include <zephyr/bluetooth/audio/mcc.h>
#include <zephyr/bluetooth/audio/media_proxy.h>
#include <zephyr/bluetooth/audio/micp.h>
#include <zephyr/bluetooth/audio/tbs.h>
#include <zephyr/bluetooth/audio/vcp.h>
#include <zephyr/bluetooth/audio/vocs.h>

#include <../host/conn_internal.h>
#include <../host/hci_core.h>

#include "utils/assert.h"
#include "utils/mem.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "bluedroid/init.h"
#else
#include "nimble/init.h"
#endif

#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
#include "ots/adapter/l2cap.h"
#endif

#include "../../../lib/include/lib_abi.h"
#include "../../../lib/include/lib_exports.h"

#include "esp_ble_audio_common_api.h"

#if CONFIG_BLE_ISO_COMPRESSED_LOG_ENABLE
#include "log_compression/utils.h"
#endif

LOG_MODULE_REGISTER(LEA_INIT, CONFIG_BT_ISO_LOG_LEVEL);

_Static_assert(sizeof(struct bt_le_audio_start_info) ==
               sizeof(esp_ble_audio_start_info_t),
               "Mismatch LE Audio start info structure");
#if CONFIG_BT_CSIP_SET_MEMBER
_Static_assert(offsetof(struct bt_le_audio_start_info, csis_insts) ==
               offsetof(esp_ble_audio_start_info_t, csis_insts),
               "Mismatch LE Audio start info structure");
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_OTS && !CONFIG_BT_OTS_SECONDARY_SVC
_Static_assert(0, "Currently only support using OTS as Secondary Service");
#endif /* CONFIG_BT_OTS && !CONFIG_BT_OTS_SECONDARY_SVC */

static const struct lib_ext_cfgs ext_cfgs = {
    /* BLE */
    .config_past_sender = CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER,
    .config_past_receiver = CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER,
    .config_past_check = false,
    .config_max_conn = CONFIG_BT_MAX_CONN,
    .config_max_paired = CONFIG_BT_MAX_PAIRED,
    .config_max_attr_len = 251,
    .config_l2cap_tx_mtu = 251,
    .config_l2cap_rx_mtu = 251,

    /* ISO */
#if CONFIG_BT_ISO_TEST_PARAMS
    .config_iso_test_params = CONFIG_BT_ISO_TEST_PARAMS,
#endif /* CONFIG_BT_ISO_TEST_PARAMS */
#if CONFIG_BT_ISO_MAX_CHAN
    .config_iso_max_chan = CONFIG_BT_ISO_MAX_CHAN,
#endif /* CONFIG_BT_ISO_MAX_CHAN */

    /* Audio (Audio Profiles Related) */
#if CONFIG_BT_AUDIO_TX
    .config_audio_tx = CONFIG_BT_AUDIO_TX,
#endif /* CONFIG_BT_AUDIO_TX */
#if CONFIG_BT_AUDIO_RX
    .config_audio_rx = CONFIG_BT_AUDIO_RX,
#endif /* CONFIG_BT_AUDIO_RX */
    .config_audio_notify_retry_delay = 20,
    .config_audio_notify_ignore_ccc = BT_LE_GATT_NOTIFY_IGNORE_CCC,

    /* AICS (Audio Input Control Service) */
#if CONFIG_BT_AICS
    .config_aics = CONFIG_BT_AICS,
    .config_aics_max_instance_count = CONFIG_BT_AICS_MAX_INSTANCE_COUNT,
    .config_aics_max_input_description_size = CONFIG_BT_AICS_MAX_INPUT_DESCRIPTION_SIZE,
#endif /* CONFIG_BT_AICS */
#if CONFIG_BT_AICS_CLIENT
    .config_aics_client = CONFIG_BT_AICS_CLIENT,
    .config_aics_client_max_instance_count = CONFIG_BT_AICS_CLIENT_MAX_INSTANCE_COUNT,
#endif /* CONFIG_BT_AICS_CLIENT */

    /* ASCS (Audio Stream Control Service) */
#if CONFIG_BT_ASCS
#if CONFIG_BT_ASCS_ASE_SNK
    .config_ascs_ase_snk = CONFIG_BT_ASCS_ASE_SNK,
    .config_ascs_max_ase_snk_count = CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT,
#endif /* CONFIG_BT_ASCS_ASE_SNK */
#if CONFIG_BT_ASCS_ASE_SRC
    .config_ascs_ase_src = CONFIG_BT_ASCS_ASE_SRC,
    .config_ascs_max_ase_src_count = CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT,
#endif /* CONFIG_BT_ASCS_ASE_SRC */
    .config_ascs_max_active_ases = CONFIG_BT_ASCS_MAX_ACTIVE_ASES,
    .config_ascs_iso_disconnect_delay = CONFIG_BT_ASCS_ISO_DISCONNECT_DELAY,
#endif /* CONFIG_BT_ASCS */

    /* BAP (Basic Audio Profile) */
#if CONFIG_BT_BAP_UNICAST_SERVER
    .config_bap_unicast_server = CONFIG_BT_BAP_UNICAST_SERVER,
#endif /* CONFIG_BT_BAP_UNICAST_SERVER */
#if CONFIG_BT_BAP_UNICAST_CLIENT
    .config_bap_unicast_client = CONFIG_BT_BAP_UNICAST_CLIENT,
    .config_bap_unicast_client_group_count = CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_COUNT,
    .config_bap_unicast_client_group_stream_count = CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT,
#if CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK
    .config_bap_unicast_client_ase_snk = CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK,
    .config_bap_unicast_client_ase_snk_count = CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT,
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK */
#if CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC
    .config_bap_unicast_client_ase_src = CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC,
    .config_bap_unicast_client_ase_src_count = CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC_COUNT,
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SRC */
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */
#if CONFIG_BT_BAP_BROADCAST_SINK
    .config_bap_broadcast_sink = CONFIG_BT_BAP_BROADCAST_SINK,
    .config_bap_broadcast_snk_count = CONFIG_BT_BAP_BROADCAST_SNK_COUNT,
    .config_bap_broadcast_snk_subgroup_count = CONFIG_BT_BAP_BROADCAST_SNK_SUBGROUP_COUNT,
    .config_bap_broadcast_snk_stream_count = CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT,
#endif /* CONFIG_BT_BAP_BROADCAST_SINK */
#if CONFIG_BT_BAP_BROADCAST_SOURCE
    .config_bap_broadcast_source = CONFIG_BT_BAP_BROADCAST_SOURCE,
    .config_bap_broadcast_src_count = CONFIG_BT_BAP_BROADCAST_SRC_COUNT,
    .config_bap_broadcast_src_subgroup_count = CONFIG_BT_BAP_BROADCAST_SRC_SUBGROUP_COUNT,
    .config_bap_broadcast_src_stream_count = CONFIG_BT_BAP_BROADCAST_SRC_STREAM_COUNT,
#endif /* CONFIG_BT_BAP_BROADCAST_SOURCE */
#if CONFIG_BT_BAP_SCAN_DELEGATOR
    .config_bap_scan_delegator = CONFIG_BT_BAP_SCAN_DELEGATOR,
    .config_bap_scan_delegator_recv_state_count = CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT,
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */
#if CONFIG_BT_BAP_BROADCAST_ASSISTANT
    .config_bap_broadcast_assistant = CONFIG_BT_BAP_BROADCAST_ASSISTANT,
    .config_bap_broadcast_assistant_recv_state_count = CONFIG_BT_BAP_BROADCAST_ASSISTANT_RECV_STATE_COUNT,
#endif /* CONFIG_BT_BAP_BROADCAST_ASSISTANT */
    .config_bap_bass_max_subgroups = CONFIG_BT_BAP_BASS_MAX_SUBGROUPS,
    .config_audio_codec_cfg_max_data_size = CONFIG_BT_AUDIO_CODEC_CFG_MAX_DATA_SIZE,
    .config_audio_codec_cfg_max_metadata_size = CONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE,
    .config_audio_codec_cap_max_data_size = CONFIG_BT_AUDIO_CODEC_CAP_MAX_DATA_SIZE,
    .config_audio_codec_cap_max_metadata_size = CONFIG_BT_AUDIO_CODEC_CAP_MAX_METADATA_SIZE,
#if CONFIG_BT_BAP_DEBUG_STREAM_DATA
    .config_bap_debug_stream_data = CONFIG_BT_BAP_DEBUG_STREAM_DATA,
#endif /* CONFIG_BT_BAP_DEBUG_STREAM_DATA */
#if CONFIG_BT_BAP_DEBUG_STREAM_SEQ_NUM
    .config_bap_debug_stream_seq_num = CONFIG_BT_BAP_DEBUG_STREAM_SEQ_NUM,
#endif /* CONFIG_BT_BAP_DEBUG_STREAM_SEQ_NUM */

    /* CAP (Common Audio Profile) */
#if CONFIG_BT_CAP_ACCEPTOR
    .config_cap_acceptor = CONFIG_BT_CAP_ACCEPTOR,
#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
    .config_cap_acceptor_set_member = CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER,
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */
#endif /* CONFIG_BT_CAP_ACCEPTOR */
#if CONFIG_BT_CAP_INITIATOR
    .config_cap_initiator = CONFIG_BT_CAP_INITIATOR,
#if CONFIG_BT_CAP_INITIATOR_UNICAST
    .config_cap_initiator_unicast = CONFIG_BT_CAP_INITIATOR_UNICAST,
#endif /* CONFIG_BT_CAP_INITIATOR_UNICAST */
#endif /* CONFIG_BT_CAP_INITIATOR */
#if CONFIG_BT_CAP_COMMANDER
    .config_cap_commander = CONFIG_BT_CAP_COMMANDER,
#endif /* CONFIG_BT_CAP_COMMANDER */
#if CONFIG_BT_CAP_HANDOVER
    .config_cap_handover = CONFIG_BT_CAP_HANDOVER,
#endif /* CONFIG_BT_CAP_HANDOVER */
#if CONFIG_BT_CAP_SUBPROC_SUPPORT
    .config_cap_subproc_support = CONFIG_BT_CAP_SUBPROC_SUPPORT,
#endif /* CONFIG_BT_CAP_SUBPROC_SUPPORT */

    /* CCP (Call Control Profile) */
#if CONFIG_BT_CCP_CALL_CONTROL_CLIENT
    .config_ccp_call_control_client = CONFIG_BT_CCP_CALL_CONTROL_CLIENT,
#if CONFIG_BT_CCP_CALL_CONTROL_CLIENT_BEARER_COUNT
    .config_ccp_call_control_client_bearer_count = CONFIG_BT_CCP_CALL_CONTROL_CLIENT_BEARER_COUNT,
#endif /* CONFIG_BT_CCP_CALL_CONTROL_CLIENT_BEARER_COUNT */
#endif /* CONFIG_BT_CCP_CALL_CONTROL_CLIENT */
#if CONFIG_BT_CCP_CALL_CONTROL_SERVER
    .config_ccp_call_control_server = CONFIG_BT_CCP_CALL_CONTROL_SERVER,
#if CONFIG_BT_CCP_CALL_CONTROL_SERVER_BEARER_COUNT
    .config_ccp_call_control_server_bearer_count = CONFIG_BT_CCP_CALL_CONTROL_SERVER_BEARER_COUNT,
#endif /* CONFIG_BT_CCP_CALL_CONTROL_SERVER_BEARER_COUNT */
#if CONFIG_BT_CCP_CALL_CONTROL_SERVER_PROVIDER_NAME_MAX_LEN
    .config_ccp_call_control_server_provider_name_max_len = CONFIG_BT_CCP_CALL_CONTROL_SERVER_PROVIDER_NAME_MAX_LEN,
#endif /* CONFIG_BT_CCP_CALL_CONTROL_SERVER_PROVIDER_NAME_MAX_LEN */
#if CONFIG_BT_CCP_CALL_CONTROL_SERVER_URI_SCHEMES_MAX_LEN
    .config_ccp_call_control_server_uri_schemes_max_len = CONFIG_BT_CCP_CALL_CONTROL_SERVER_URI_SCHEMES_MAX_LEN,
#endif /* CONFIG_BT_CCP_CALL_CONTROL_SERVER_URI_SCHEMES_MAX_LEN */
#endif /* CONFIG_BT_CCP_CALL_CONTROL_SERVER */

    /* CSIP (Coordinated Set Identification Profile) */
#if CONFIG_BT_CSIP_SET_MEMBER
    .config_csip_set_member = CONFIG_BT_CSIP_SET_MEMBER,
    .config_csip_set_member_max_instance_count = CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT,
    .config_csip_set_member_enc_sirk_support = CONFIG_BT_CSIP_SET_MEMBER_ENC_SIRK_SUPPORT,
#if CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE
    .config_csip_set_member_sirk_notifiable = CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE,
#endif /* CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE */
#if CONFIG_BT_CSIP_SET_MEMBER_SIZE_SUPPORT
    .config_csip_set_member_size_support = CONFIG_BT_CSIP_SET_MEMBER_SIZE_SUPPORT,
#endif /* CONFIG_BT_CSIP_SET_MEMBER_SIZE_SUPPORT */
#if CONFIG_BT_CSIP_SET_MEMBER_SIZE_NOTIFIABLE
    .config_csip_set_member_size_notifiable = CONFIG_BT_CSIP_SET_MEMBER_SIZE_NOTIFIABLE,
#endif /* CONFIG_BT_CSIP_SET_MEMBER_SIZE_NOTIFIABLE */
#if CONFIG_BT_CSIP_SET_MEMBER_LOCK_SUPPORT
    .config_csip_set_member_lock_support = CONFIG_BT_CSIP_SET_MEMBER_LOCK_SUPPORT,
#endif /* CONFIG_BT_CSIP_SET_MEMBER_LOCK_SUPPORT */
#if CONFIG_BT_CSIP_SET_MEMBER_RANK_SUPPORT
    .config_csip_set_member_rank_support = CONFIG_BT_CSIP_SET_MEMBER_RANK_SUPPORT,
#endif /* CONFIG_BT_CSIP_SET_MEMBER_RANK_SUPPORT */
#if CONFIG_BT_CSIP_SET_MEMBER_SET_NAME_NOTIFIABLE
    .config_csip_set_member_set_name_notifiable = CONFIG_BT_CSIP_SET_MEMBER_SET_NAME_NOTIFIABLE,
#endif /* CONFIG_BT_CSIP_SET_MEMBER_SET_NAME_NOTIFIABLE */
#if CONFIG_BT_CSIP_SET_MEMBER_TEST_SAMPLE_DATA
    .config_csip_set_member_test_sample_data = CONFIG_BT_CSIP_SET_MEMBER_TEST_SAMPLE_DATA,
#endif /* CONFIG_BT_CSIP_SET_MEMBER_TEST_SAMPLE_DATA */
#endif /* CONFIG_BT_CSIP_SET_MEMBER */
#if CONFIG_BT_CSIP_SET_COORDINATOR
    .config_csip_set_coordinator = CONFIG_BT_CSIP_SET_COORDINATOR,
    .config_csip_set_coordinator_max_csis_instances = CONFIG_BT_CSIP_SET_COORDINATOR_MAX_CSIS_INSTANCES,
    .config_csip_set_coordinator_enc_sirk_support = CONFIG_BT_CSIP_SET_COORDINATOR_ENC_SIRK_SUPPORT,
#if CONFIG_BT_CSIP_SET_COORDINATOR_TEST_SAMPLE_DATA
    .config_csip_set_coordinator_test_sample_data = CONFIG_BT_CSIP_SET_COORDINATOR_TEST_SAMPLE_DATA,
#endif /* CONFIG_BT_CSIP_SET_COORDINATOR_TEST_SAMPLE_DATA */
#endif /* CONFIG_BT_CSIP_SET_COORDINATOR */

    /* GMAP (Gaming Audio Profile) */
#if CONFIG_BT_GMAP
    .config_gmap = CONFIG_BT_GMAP,
#if CONFIG_BT_GMAP_UGG_SUPPORTED
    .config_gmap_ugg_supported = CONFIG_BT_GMAP_UGG_SUPPORTED,
#endif /* CONFIG_BT_GMAP_UGG_SUPPORTED */
#if CONFIG_BT_GMAP_UGT_SUPPORTED
    .config_gmap_ugt_supported = CONFIG_BT_GMAP_UGT_SUPPORTED,
#endif /* CONFIG_BT_GMAP_UGT_SUPPORTED */
#if CONFIG_BT_GMAP_BGS_SUPPORTED
    .config_gmap_bgs_supported = CONFIG_BT_GMAP_BGS_SUPPORTED,
#endif /* CONFIG_BT_GMAP_BGS_SUPPORTED */
#if CONFIG_BT_GMAP_BGR_SUPPORTED
    .config_gmap_bgr_supported = CONFIG_BT_GMAP_BGR_SUPPORTED,
#endif /* CONFIG_BT_GMAP_BGR_SUPPORTED */
#endif /* CONFIG_BT_GMAP */

    /* HAS (Hearing Access Service) */
#if CONFIG_BT_HAS
    .config_has = CONFIG_BT_HAS,
    .config_has_preset_count = CONFIG_BT_HAS_PRESET_COUNT,
#if CONFIG_BT_HAS_FEATURES_NOTIFIABLE
    .config_has_features_notifiable = CONFIG_BT_HAS_FEATURES_NOTIFIABLE,
#endif /* CONFIG_BT_HAS_FEATURES_NOTIFIABLE */
#if CONFIG_BT_HAS_PRESET_SUPPORT
    .config_has_preset_support = CONFIG_BT_HAS_PRESET_SUPPORT,
#if CONFIG_BT_HAS_PRESET_NAME_DYNAMIC
    .config_has_preset_name_dynamic = CONFIG_BT_HAS_PRESET_NAME_DYNAMIC,
#endif /* CONFIG_BT_HAS_PRESET_NAME_DYNAMIC */
#if CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE
    .config_has_preset_control_point_notifiable = CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE,
#endif /* CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE */
#if CONFIG_BT_HAS_ACTIVE_PRESET_INDEX
    .config_has_active_preset_index = CONFIG_BT_HAS_ACTIVE_PRESET_INDEX,
#endif /* CONFIG_BT_HAS_ACTIVE_PRESET_INDEX */
#endif /* CONFIG_BT_HAS_PRESET_SUPPORT */
#endif /* CONFIG_BT_HAS */
#if CONFIG_BT_HAS_CLIENT
    .config_has_client = CONFIG_BT_HAS_CLIENT,
#endif /* CONFIG_BT_HAS_CLIENT */

    /* MCS (Media Control Service) */
#if CONFIG_BT_MCS
    .config_mcs = CONFIG_BT_MCS,
    .config_mcs_instance_count = CONFIG_BT_MCS_INSTANCE_COUNT,
#endif /* CONFIG_BT_MCS */
#if CONFIG_BT_MCC
    .config_mcc = CONFIG_BT_MCC,
    .config_mcc_media_player_name_max = CONFIG_BT_MCC_MEDIA_PLAYER_NAME_MAX,
    .config_mcc_icon_url_max = CONFIG_BT_MCC_ICON_URL_MAX,
    .config_mcc_track_title_max = CONFIG_BT_MCC_TRACK_TITLE_MAX,
    .config_mcc_segment_name_max = CONFIG_BT_MCC_SEGMENT_NAME_MAX,
#if CONFIG_BT_MCC_OTS
    .config_mcc_ots = CONFIG_BT_MCC_OTS,
    .config_mcc_otc_obj_buf_size = CONFIG_BT_MCC_OTC_OBJ_BUF_SIZE,
    .config_mcc_total_obj_content_mem = CONFIG_BT_MCC_TOTAL_OBJ_CONTENT_MEM,
    .config_mcc_track_segs_max_cnt = CONFIG_BT_MCC_TRACK_SEGS_MAX_CNT,
    .config_mcc_group_records_max = CONFIG_BT_MCC_GROUP_RECORDS_MAX,
#endif /* CONFIG_BT_MCC_OTS */
#if CONFIG_BT_MCC_READ_MEDIA_PLAYER_ICON_URL
    .config_mcc_read_media_player_icon_url = CONFIG_BT_MCC_READ_MEDIA_PLAYER_ICON_URL,
#endif /* CONFIG_BT_MCC_READ_MEDIA_PLAYER_ICON_URL */
#if CONFIG_BT_MCC_READ_TRACK_TITLE
    .config_mcc_read_track_title = CONFIG_BT_MCC_READ_TRACK_TITLE,
#endif /* CONFIG_BT_MCC_READ_TRACK_TITLE */
#if CONFIG_BT_MCC_READ_TRACK_TITLE_ENABLE_SUBSCRIPTION
    .config_mcc_read_track_title_enable_subscription = CONFIG_BT_MCC_READ_TRACK_TITLE_ENABLE_SUBSCRIPTION,
#endif /* CONFIG_BT_MCC_READ_TRACK_TITLE_ENABLE_SUBSCRIPTION */
#if CONFIG_BT_MCC_READ_TRACK_DURATION
    .config_mcc_read_track_duration = CONFIG_BT_MCC_READ_TRACK_DURATION,
#endif /* CONFIG_BT_MCC_READ_TRACK_DURATION */
#if CONFIG_BT_MCC_READ_TRACK_POSITION
    .config_mcc_read_track_position = CONFIG_BT_MCC_READ_TRACK_POSITION,
#endif /* CONFIG_BT_MCC_READ_TRACK_POSITION */
#if CONFIG_BT_MCC_SET_TRACK_POSITION
    .config_mcc_set_track_position = CONFIG_BT_MCC_SET_TRACK_POSITION,
#endif /* CONFIG_BT_MCC_SET_TRACK_POSITION */
#if CONFIG_BT_MCC_READ_PLAYBACK_SPEED
    .config_mcc_read_playback_speed = CONFIG_BT_MCC_READ_PLAYBACK_SPEED,
#endif /* CONFIG_BT_MCC_READ_PLAYBACK_SPEED */
#if CONFIG_BT_MCC_SET_PLAYBACK_SPEED
    .config_mcc_set_playback_speed = CONFIG_BT_MCC_SET_PLAYBACK_SPEED,
#endif /* CONFIG_BT_MCC_SET_PLAYBACK_SPEED */
#if CONFIG_BT_MCC_READ_SEEKING_SPEED
    .config_mcc_read_seeking_speed = CONFIG_BT_MCC_READ_SEEKING_SPEED,
#endif /* CONFIG_BT_MCC_READ_SEEKING_SPEED */
#if CONFIG_BT_MCC_READ_PLAYING_ORDER
    .config_mcc_read_playing_order = CONFIG_BT_MCC_READ_PLAYING_ORDER,
#endif /* CONFIG_BT_MCC_READ_PLAYING_ORDER */
#if CONFIG_BT_MCC_SET_PLAYING_ORDER
    .config_mcc_set_playing_order = CONFIG_BT_MCC_SET_PLAYING_ORDER,
#endif /* CONFIG_BT_MCC_SET_PLAYING_ORDER */
#if CONFIG_BT_MCC_READ_PLAYING_ORDER_SUPPORTED
    .config_mcc_read_playing_order_supported = CONFIG_BT_MCC_READ_PLAYING_ORDER_SUPPORTED,
#endif /* CONFIG_BT_MCC_READ_PLAYING_ORDER_SUPPORTED */
#if CONFIG_BT_MCC_READ_MEDIA_STATE
    .config_mcc_read_media_state = CONFIG_BT_MCC_READ_MEDIA_STATE,
#endif /* CONFIG_BT_MCC_READ_MEDIA_STATE */
#if CONFIG_BT_MCC_SET_MEDIA_CONTROL_POINT
    .config_mcc_set_media_control_point = CONFIG_BT_MCC_SET_MEDIA_CONTROL_POINT,
#endif /* CONFIG_BT_MCC_SET_MEDIA_CONTROL_POINT */
#if CONFIG_BT_MCC_READ_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED
    .config_mcc_read_media_control_point_opcodes_supported = CONFIG_BT_MCC_READ_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED,
#endif /* CONFIG_BT_MCC_READ_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED */
#if CONFIG_BT_MCC_READ_CONTENT_CONTROL_ID
    .config_mcc_read_content_control_id = CONFIG_BT_MCC_READ_CONTENT_CONTROL_ID,
#endif /* CONFIG_BT_MCC_READ_CONTENT_CONTROL_ID */
#endif /* CONFIG_BT_MCC */

    /* MCTL (Media Control) */
#if CONFIG_BT_MCTL
    .config_mctl = CONFIG_BT_MCTL,
#if CONFIG_BT_MCTL_LOCAL_PLAYER_CONTROL
    .config_mctl_local_player_control = CONFIG_BT_MCTL_LOCAL_PLAYER_CONTROL,
#if CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL
    .config_mctl_local_player_local_control = CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL,
#endif /* CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL */
#if CONFIG_BT_MCTL_LOCAL_PLAYER_REMOTE_CONTROL
    .config_mctl_local_player_remote_control = CONFIG_BT_MCTL_LOCAL_PLAYER_REMOTE_CONTROL,
#endif /* CONFIG_BT_MCTL_LOCAL_PLAYER_REMOTE_CONTROL */
#endif /* CONFIG_BT_MCTL_LOCAL_PLAYER_CONTROL */
#if CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL
    .config_mctl_remote_player_control = CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL,
#if CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL_OBJECTS
    .config_mctl_remote_player_control_objects = CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL_OBJECTS,
#endif /* CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL_OBJECTS */
#endif /* CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL */
#endif /* CONFIG_BT_MCTL */

    /* MICP (Microphone Control Profile) */
#if CONFIG_BT_MICP_MIC_DEV
    .config_micp_mic_dev = CONFIG_BT_MICP_MIC_DEV,
#if CONFIG_BT_MICP_MIC_DEV_AICS
    .config_micp_mic_dev_aics = CONFIG_BT_MICP_MIC_DEV_AICS,
    .config_micp_mic_dev_aics_instance_count = CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT,
#endif /* CONFIG_BT_MICP_MIC_DEV_AICS */
#endif /* CONFIG_BT_MICP_MIC_DEV */
#if CONFIG_BT_MICP_MIC_CTLR
    .config_micp_mic_ctlr = CONFIG_BT_MICP_MIC_CTLR,
#if CONFIG_BT_MICP_MIC_CTLR_AICS
    .config_micp_mic_ctlr_aics = CONFIG_BT_MICP_MIC_CTLR_AICS,
    .config_micp_mic_ctlr_max_aics_inst = CONFIG_BT_MICP_MIC_CTLR_MAX_AICS_INST,
#endif /* CONFIG_BT_MICP_MIC_CTLR_AICS */
#endif /* CONFIG_BT_MICP_MIC_CTLR */

    /* MPL (Media player) */
#if CONFIG_BT_MPL
    .config_mpl = CONFIG_BT_MPL,
    .config_mpl_media_player_name = CONFIG_BT_MPL_MEDIA_PLAYER_NAME,
    .config_mpl_media_player_name_max = CONFIG_BT_MPL_MEDIA_PLAYER_NAME_MAX,
    .config_mpl_icon_url = CONFIG_BT_MPL_ICON_URL,
    .config_mpl_icon_url_max = CONFIG_BT_MPL_ICON_URL_MAX,
    .config_mpl_track_title_max = CONFIG_BT_MPL_TRACK_TITLE_MAX,
    .config_mpl_segment_name_max = CONFIG_BT_MPL_SEGMENT_NAME_MAX,
    .config_mpl_group_title_max = CONFIG_BT_MPL_GROUP_TITLE_MAX,
#if CONFIG_BT_MPL_OBJECTS
    .config_mpl_objects = CONFIG_BT_MPL_OBJECTS,
    .config_mpl_max_obj_size = CONFIG_BT_MPL_MAX_OBJ_SIZE,
    .config_mpl_icon_bitmap_size = CONFIG_BT_MPL_ICON_BITMAP_SIZE,
    .config_mpl_track_max_size = CONFIG_BT_MPL_TRACK_MAX_SIZE,
#endif /* CONFIG_BT_MPL_OBJECTS */
#endif /* CONFIG_BT_MPL */

    /* PACS (Published Audio Capabilities Service) */
#if CONFIG_BT_PAC_SNK
    .config_pac_snk = CONFIG_BT_PAC_SNK,
#if CONFIG_BT_PAC_SNK_NOTIFIABLE
    .config_pac_snk_notifiable = CONFIG_BT_PAC_SNK_NOTIFIABLE,
#endif /* CONFIG_BT_PAC_SNK_NOTIFIABLE */
#if CONFIG_BT_PAC_SNK_LOC
    .config_pac_snk_loc = CONFIG_BT_PAC_SNK_LOC,
#if CONFIG_BT_PAC_SNK_LOC_WRITEABLE
    .config_pac_snk_loc_writeable = CONFIG_BT_PAC_SNK_LOC_WRITEABLE,
#endif /* CONFIG_BT_PAC_SNK_LOC_WRITEABLE */
#if CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE
    .config_pac_snk_loc_notifiable = CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE,
#endif /* CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE */
#endif /* CONFIG_BT_PAC_SNK_LOC */
#endif /* CONFIG_BT_PAC_SNK */
#if CONFIG_BT_PAC_SRC
    .config_pac_src = CONFIG_BT_PAC_SRC,
#if CONFIG_BT_PAC_SRC_NOTIFIABLE
    .config_pac_src_notifiable = CONFIG_BT_PAC_SRC_NOTIFIABLE,
#endif /* CONFIG_BT_PAC_SRC_NOTIFIABLE */
#if CONFIG_BT_PAC_SRC_LOC
    .config_pac_src_loc = CONFIG_BT_PAC_SRC_LOC,
#if CONFIG_BT_PAC_SRC_LOC_WRITEABLE
    .config_pac_src_loc_writeable = CONFIG_BT_PAC_SRC_LOC_WRITEABLE,
#endif /* CONFIG_BT_PAC_SRC_LOC_WRITEABLE */
#if CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE
    .config_pac_src_loc_notifiable = CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE,
#endif /* CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE */
#endif /* CONFIG_BT_PAC_SRC_LOC */
#endif /* CONFIG_BT_PAC_SRC */
#if CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE
    .config_pacs_supported_context_notifiable = CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE,
#endif /* CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE */

    /* PBP (Public Broadcast Profile) */
#if CONFIG_BT_PBP
    .config_pbp = CONFIG_BT_PBP,
#endif /* CONFIG_BT_PBP */

    /* TBS (Telephone Bearer Service) */
#if CONFIG_BT_TBS
    .config_tbs = CONFIG_BT_TBS,
    .config_tbs_supported_features = CONFIG_BT_TBS_SUPPORTED_FEATURES,
    .config_tbs_max_calls = CONFIG_BT_TBS_MAX_CALLS,
    .config_tbs_bearer_count = CONFIG_BT_TBS_BEARER_COUNT,
    .config_tbs_max_scheme_list_length = CONFIG_BT_TBS_MAX_SCHEME_LIST_LENGTH,
#endif /* CONFIG_BT_TBS */
#if CONFIG_BT_TBS_CLIENT
    .config_tbs_client = CONFIG_BT_TBS_CLIENT,
#if CONFIG_BT_TBS_CLIENT_GTBS
    .config_tbs_client_gtbs = CONFIG_BT_TBS_CLIENT_GTBS,
#endif /* CONFIG_BT_TBS_CLIENT_GTBS */
#if CONFIG_BT_TBS_CLIENT_TBS
    .config_tbs_client_tbs = CONFIG_BT_TBS_CLIENT_TBS,
#endif /* CONFIG_BT_TBS_CLIENT_TBS */
    .config_tbs_client_max_calls = CONFIG_BT_TBS_CLIENT_MAX_CALLS,
#if CONFIG_BT_TBS_CLIENT_MAX_TBS_INSTANCES
    .config_tbs_client_max_tbs_instances = CONFIG_BT_TBS_CLIENT_MAX_TBS_INSTANCES,
#endif /* CONFIG_BT_TBS_CLIENT_MAX_TBS_INSTANCES */
#if CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME
    .config_tbs_client_bearer_provider_name = CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME,
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_PROVIDER_NAME */
#if CONFIG_BT_TBS_CLIENT_BEARER_UCI
    .config_tbs_client_bearer_uci = CONFIG_BT_TBS_CLIENT_BEARER_UCI,
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_UCI */
#if CONFIG_BT_TBS_CLIENT_BEARER_TECHNOLOGY
    .config_tbs_client_bearer_technology = CONFIG_BT_TBS_CLIENT_BEARER_TECHNOLOGY,
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_TECHNOLOGY */
#if CONFIG_BT_TBS_CLIENT_BEARER_URI_SCHEMES_SUPPORTED_LIST
    .config_tbs_client_bearer_uri_schemes_supported_list = CONFIG_BT_TBS_CLIENT_BEARER_URI_SCHEMES_SUPPORTED_LIST,
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_URI_SCHEMES_SUPPORTED_LIST */
#if CONFIG_BT_TBS_CLIENT_BEARER_SIGNAL_STRENGTH
    .config_tbs_client_bearer_signal_strength = CONFIG_BT_TBS_CLIENT_BEARER_SIGNAL_STRENGTH,
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_SIGNAL_STRENGTH */
#if CONFIG_BT_TBS_CLIENT_READ_BEARER_SIGNAL_INTERVAL
    .config_tbs_client_read_bearer_signal_interval = CONFIG_BT_TBS_CLIENT_READ_BEARER_SIGNAL_INTERVAL,
#endif /* CONFIG_BT_TBS_CLIENT_READ_BEARER_SIGNAL_INTERVAL */
#if CONFIG_BT_TBS_CLIENT_SET_BEARER_SIGNAL_INTERVAL
    .config_tbs_client_set_bearer_signal_interval = CONFIG_BT_TBS_CLIENT_SET_BEARER_SIGNAL_INTERVAL,
#endif /* CONFIG_BT_TBS_CLIENT_SET_BEARER_SIGNAL_INTERVAL */
#if CONFIG_BT_TBS_CLIENT_BEARER_LIST_CURRENT_CALLS
    .config_tbs_client_bearer_list_current_calls = CONFIG_BT_TBS_CLIENT_BEARER_LIST_CURRENT_CALLS,
#endif /* CONFIG_BT_TBS_CLIENT_BEARER_LIST_CURRENT_CALLS */
#if CONFIG_BT_TBS_CLIENT_CCID
    .config_tbs_client_ccid = CONFIG_BT_TBS_CLIENT_CCID,
#endif /* CONFIG_BT_TBS_CLIENT_CCID */
#if CONFIG_BT_TBS_CLIENT_INCOMING_URI
    .config_tbs_client_incoming_uri = CONFIG_BT_TBS_CLIENT_INCOMING_URI,
#endif /* CONFIG_BT_TBS_CLIENT_INCOMING_URI */
#if CONFIG_BT_TBS_CLIENT_STATUS_FLAGS
    .config_tbs_client_status_flags = CONFIG_BT_TBS_CLIENT_STATUS_FLAGS,
#endif /* CONFIG_BT_TBS_CLIENT_STATUS_FLAGS */
#if CONFIG_BT_TBS_CLIENT_CP_PROCEDURES
    .config_tbs_client_cp_procedures = CONFIG_BT_TBS_CLIENT_CP_PROCEDURES,
#endif /* CONFIG_BT_TBS_CLIENT_CP_PROCEDURES */
#if CONFIG_BT_TBS_CLIENT_ACCEPT_CALL
    .config_tbs_client_accept_call = CONFIG_BT_TBS_CLIENT_ACCEPT_CALL,
#endif /* CONFIG_BT_TBS_CLIENT_ACCEPT_CALL */
#if CONFIG_BT_TBS_CLIENT_TERMINATE_CALL
    .config_tbs_client_terminate_call = CONFIG_BT_TBS_CLIENT_TERMINATE_CALL,
#endif /* CONFIG_BT_TBS_CLIENT_TERMINATE_CALL */
#if CONFIG_BT_TBS_CLIENT_HOLD_CALL
    .config_tbs_client_hold_call = CONFIG_BT_TBS_CLIENT_HOLD_CALL,
#endif /* CONFIG_BT_TBS_CLIENT_HOLD_CALL */
#if CONFIG_BT_TBS_CLIENT_RETRIEVE_CALL
    .config_tbs_client_retrieve_call = CONFIG_BT_TBS_CLIENT_RETRIEVE_CALL,
#endif /* CONFIG_BT_TBS_CLIENT_RETRIEVE_CALL */
#if CONFIG_BT_TBS_CLIENT_ORIGINATE_CALL
    .config_tbs_client_originate_call = CONFIG_BT_TBS_CLIENT_ORIGINATE_CALL,
#endif /* CONFIG_BT_TBS_CLIENT_ORIGINATE_CALL */
#if CONFIG_BT_TBS_CLIENT_JOIN_CALLS
    .config_tbs_client_join_calls = CONFIG_BT_TBS_CLIENT_JOIN_CALLS,
#endif /* CONFIG_BT_TBS_CLIENT_JOIN_CALLS */
#if CONFIG_BT_TBS_CLIENT_OPTIONAL_OPCODES
    .config_tbs_client_optional_opcodes = CONFIG_BT_TBS_CLIENT_OPTIONAL_OPCODES,
#endif /* CONFIG_BT_TBS_CLIENT_OPTIONAL_OPCODES */
#if CONFIG_BT_TBS_CLIENT_INCOMING_CALL
    .config_tbs_client_incoming_call = CONFIG_BT_TBS_CLIENT_INCOMING_CALL,
#endif /* CONFIG_BT_TBS_CLIENT_INCOMING_CALL */
#if CONFIG_BT_TBS_CLIENT_CALL_FRIENDLY_NAME
    .config_tbs_client_call_friendly_name = CONFIG_BT_TBS_CLIENT_CALL_FRIENDLY_NAME,
#endif /* CONFIG_BT_TBS_CLIENT_CALL_FRIENDLY_NAME */
#endif /* CONFIG_BT_TBS_CLIENT */
#if CONFIG_BT_TBS || CONFIG_BT_TBS_CLIENT
    .config_tbs_max_uri_length = CONFIG_BT_TBS_MAX_URI_LENGTH,
    .config_tbs_max_provider_name_length = CONFIG_BT_TBS_MAX_PROVIDER_NAME_LENGTH,
    .config_tbs_max_friendly_name_length = CONFIG_BT_TBS_MAX_FRIENDLY_NAME_LENGTH,
#endif /* CONFIG_BT_TBS || CONFIG_BT_TBS_CLIENT */

    /* TMAP (Telephony and Media Audio Profile) */
#if CONFIG_BT_TMAP_CG_SUPPORTED
    .config_tmap_cg_supported = CONFIG_BT_TMAP_CG_SUPPORTED,
#endif /* CONFIG_BT_TMAP_CG_SUPPORTED */
#if CONFIG_BT_TMAP_CT_SUPPORTED
    .config_tmap_ct_supported = CONFIG_BT_TMAP_CT_SUPPORTED,
#endif /* CONFIG_BT_TMAP_CT_SUPPORTED */
#if CONFIG_BT_TMAP_UMS_SUPPORTED
    .config_tmap_ums_supported = CONFIG_BT_TMAP_UMS_SUPPORTED,
#endif /* CONFIG_BT_TMAP_UMS_SUPPORTED */
#if CONFIG_BT_TMAP_UMR_SUPPORTED
    .config_tmap_umr_supported = CONFIG_BT_TMAP_UMR_SUPPORTED,
#endif /* CONFIG_BT_TMAP_UMR_SUPPORTED */
#if CONFIG_BT_TMAP_BMS_SUPPORTED
    .config_tmap_bms_supported = CONFIG_BT_TMAP_BMS_SUPPORTED,
#endif /* CONFIG_BT_TMAP_BMS_SUPPORTED */
#if CONFIG_BT_TMAP_BMR_SUPPORTED
    .config_tmap_bmr_supported = CONFIG_BT_TMAP_BMR_SUPPORTED,
#endif /* CONFIG_BT_TMAP_BMR_SUPPORTED */
#if CONFIG_BT_TMAP
    .config_tmap = CONFIG_BT_TMAP,
#endif /* CONFIG_BT_TMAP */

    /* VCP (Volume Control Profile) */
#if CONFIG_BT_VCP_VOL_REND
    .config_vcp_vol_rend = CONFIG_BT_VCP_VOL_REND,
#if CONFIG_BT_VCP_VOL_REND_VOCS
    .config_vcp_vol_rend_vocs = CONFIG_BT_VCP_VOL_REND_VOCS,
    .config_vcp_vol_rend_vocs_instance_count = CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT,
#endif /* CONFIG_BT_VCP_VOL_REND_VOCS */
#if CONFIG_BT_VCP_VOL_REND_AICS
    .config_vcp_vol_rend_aics = CONFIG_BT_VCP_VOL_REND_AICS,
    .config_vcp_vol_rend_aics_instance_count = CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT,
#endif /* CONFIG_BT_VCP_VOL_REND_AICS */
#if CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE
    .config_vcp_vol_rend_vol_flags_notifiable = CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE,
#endif /* CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE */
#endif /* CONFIG_BT_VCP_VOL_REND */
#if CONFIG_BT_VCP_VOL_CTLR
    .config_vcp_vol_ctlr = CONFIG_BT_VCP_VOL_CTLR,
#if CONFIG_BT_VCP_VOL_CTLR_VOCS
    .config_vcp_vol_ctlr_vocs = CONFIG_BT_VCP_VOL_CTLR_VOCS,
    .config_vcp_vol_ctlr_max_vocs_inst = CONFIG_BT_VCP_VOL_CTLR_MAX_VOCS_INST,
#endif /* CONFIG_BT_VCP_VOL_CTLR_VOCS */
#if CONFIG_BT_VCP_VOL_CTLR_AICS
    .config_vcp_vol_ctlr_aics = CONFIG_BT_VCP_VOL_CTLR_AICS,
    .config_vcp_vol_ctlr_max_aics_inst = CONFIG_BT_VCP_VOL_CTLR_MAX_AICS_INST,
#endif /* CONFIG_BT_VCP_VOL_CTLR_AICS */
#endif /* CONFIG_BT_VCP_VOL_CTLR */

    /* VOCS (Volume Control Service) */
#if CONFIG_BT_VOCS
    .config_vocs = CONFIG_BT_VOCS,
    .config_vocs_max_instance_count = CONFIG_BT_VOCS_MAX_INSTANCE_COUNT,
    .config_vocs_max_output_description_size = CONFIG_BT_VOCS_MAX_OUTPUT_DESCRIPTION_SIZE,
#endif /* CONFIG_BT_VOCS */
#if CONFIG_BT_VOCS_CLIENT
    .config_vocs_client = CONFIG_BT_VOCS_CLIENT,
    .config_vocs_client_max_instance_count = CONFIG_BT_VOCS_CLIENT_MAX_INSTANCE_COUNT,
#endif /* CONFIG_BT_VOCS_CLIENT */

    /* OTS (Object Transfer Service) */
#if CONFIG_BT_OTS
    .config_ots = CONFIG_BT_OTS,
#endif /* CONFIG_BT_OTS */
#if CONFIG_BT_OTS_CLIENT
    .config_ots_client = CONFIG_BT_OTS_CLIENT,
#endif /* CONFIG_BT_OTS_CLIENT */
#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    .config_ots_obj_max_name_len = CONFIG_BT_OTS_OBJ_MAX_NAME_LEN,
#endif /* CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT */

    /* PTS test mode: gates PTS-only changes. Set true only for PTS builds. */
    .config_pts_test_enable = false,

    .config_version = LEA_VERSION,
};

#define LEA_TAG     "LEA_LIB"

static void log_debug(const char *format, ...)
{
#if (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_DEBUG)
    va_list args;

    va_start(args, format);

    /* Note: do not use `ESP_LOG_DEBUG` here! */
    esp_log_write(ESP_LOG_INFO, LEA_TAG, BT_ISO_LOG_COLOR_D "D (%lu) %s: ", esp_log_timestamp(), LEA_TAG);
    esp_log_writev(ESP_LOG_INFO, LEA_TAG, format, args);
    esp_log_write(ESP_LOG_INFO, LEA_TAG, BT_ISO_LOG_RESET_COLOR "\n");

    va_end(args);
#endif /* (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_DEBUG) */
}

static void log_info(const char *format, ...)
{
#if (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_INFO)
    va_list args;

    va_start(args, format);

    esp_log_write(ESP_LOG_INFO, LEA_TAG, BT_ISO_LOG_COLOR_I "I (%lu) %s: ", esp_log_timestamp(), LEA_TAG);
    esp_log_writev(ESP_LOG_INFO, LEA_TAG, format, args);
    esp_log_write(ESP_LOG_INFO, LEA_TAG, BT_ISO_LOG_RESET_COLOR "\n");

    va_end(args);
#endif /* (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_INFO) */
}

static void log_warn(const char *format, ...)
{
#if (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_WARN)
    va_list args;

    va_start(args, format);

    esp_log_write(ESP_LOG_WARN, LEA_TAG, BT_ISO_LOG_COLOR_W "W (%lu) %s: ", esp_log_timestamp(), LEA_TAG);
    esp_log_writev(ESP_LOG_WARN, LEA_TAG, format, args);
    esp_log_write(ESP_LOG_WARN, LEA_TAG, BT_ISO_LOG_RESET_COLOR "\n");

    va_end(args);
#endif /* (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_WARN) */
}

static void log_error(const char *format, ...)
{
#if (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_ERROR)
    va_list args;

    va_start(args, format);

    esp_log_write(ESP_LOG_ERROR, LEA_TAG, BT_ISO_LOG_COLOR_E "E (%lu) %s: ", esp_log_timestamp(), LEA_TAG);
    esp_log_writev(ESP_LOG_ERROR, LEA_TAG, format, args);
    esp_log_write(ESP_LOG_ERROR, LEA_TAG, BT_ISO_LOG_RESET_COLOR "\n");

    va_end(args);
#endif /* (CONFIG_BT_AUDIO_LOG_LEVEL >= BT_ISO_LOG_ERROR) */
}

static const struct lib_ext_funcs ext_funcs = {
    ._log_dbg = (void *)log_debug,
    ._log_inf = (void *)log_info,
    ._log_wrn = (void *)log_warn,
    ._log_err = (void *)log_error,

    ._assert = (void *)bt_le_assert,

#if CONFIG_BT_AUDIO_HEAP_EXTERNAL_MEMORY
    ._malloc = (void *)bt_le_ext_malloc,
    ._calloc = (void *)bt_le_ext_calloc,
#else
    ._malloc = (void *)malloc,
    ._calloc = (void *)calloc,
#endif
    ._free = (void *)free,

    ._rand = (void *)bt_rand,
    ._encrypt_le = (void *)bt_encrypt_le,
    ._aes_cmac = (void *)bt_crypto_aes_cmac,

    ._utf8_trunc = (void *)utf8_trunc,
    ._utf8_lcpy = (void *)utf8_lcpy,
    ._utf8_count_chars = (void *)utf8_count_chars,

    ._mutex_create = (void *)k_mutex_create,
    ._mutex_delete = (void *)k_mutex_delete,
    ._mutex_lock = (void *)k_mutex_lock,
    ._mutex_unlock = (void *)k_mutex_unlock,
    ._lock = (void *)bt_le_host_lock,
    ._unlock = (void *)bt_le_host_unlock,

    ._buf_simple_init = (void *)net_buf_simple_init,
    ._buf_simple_init_with_data = (void *)net_buf_simple_init_with_data,
    ._buf_simple_reset = (void *)net_buf_simple_reset,
    ._buf_simple_clone = (void *)net_buf_simple_clone,
    ._buf_simple_add = (void *)net_buf_simple_add,
    ._buf_simple_add_mem = (void *)net_buf_simple_add_mem,
    ._buf_simple_add_u8 = (void *)net_buf_simple_add_u8,
    ._buf_simple_add_le16 = (void *)net_buf_simple_add_le16,
    ._buf_simple_add_le24 = (void *)net_buf_simple_add_le24,
    ._buf_simple_add_le32 = (void *)net_buf_simple_add_le32,
    ._buf_simple_add_le48 = (void *)net_buf_simple_add_le48,
    ._buf_simple_add_le64 = (void *)net_buf_simple_add_le64,
    ._buf_simple_pull = (void *)net_buf_simple_pull,
    ._buf_simple_pull_mem = (void *)net_buf_simple_pull_mem,
    ._buf_simple_pull_u8 = (void *)net_buf_simple_pull_u8,
    ._buf_simple_pull_le16 = (void *)net_buf_simple_pull_le16,
    ._buf_simple_pull_le24 = (void *)net_buf_simple_pull_le24,
    ._buf_simple_pull_le32 = (void *)net_buf_simple_pull_le32,
    ._buf_simple_pull_le48 = (void *)net_buf_simple_pull_le48,
    ._buf_simple_pull_le64 = (void *)net_buf_simple_pull_le64,
    ._buf_simple_headroom = (void *)net_buf_simple_headroom,
    ._buf_simple_tailroom = (void *)net_buf_simple_tailroom,
    ._buf_simple_save = (void *)net_buf_simple_save,
    ._buf_simple_restore = (void *)net_buf_simple_restore,
    ._buf_user_data = (void *)net_buf_user_data,
    ._buf_frags_len = (void *)net_buf_frags_len,

    ._dwork_init = (void *)k_work_init_delayable,
    ._dwork_deinit = (void *)k_work_deinit_delayable,
    ._dwork_schedule = (void *)k_work_schedule,
    ._dwork_cancel = (void *)k_work_cancel_delayable,
    ._dwork_cancel_sync = (void *)k_work_cancel_delayable_sync,
    ._dwork_remaining_get = (void *)k_work_delayable_remaining_get,
    ._work_is_pending = (void *)k_work_is_pending,
    ._work_submit = (void *)k_work_submit,
    ._dwork_from_work = (void *)k_work_delayable_from_work,

    ._bt_hex = (void *)bt_hex,
    ._bt_addr_str = (void *)bt_addr_str,
    ._bt_addr_le_str = (void *)bt_addr_le_str,
    ._bt_addr_le_to_str = (void *)bt_addr_le_to_str,
    ._bt_addr_le_eq = (void *)bt_addr_le_eq,
    ._bt_addr_copy = (void *)bt_addr_copy,
    ._bt_addr_le_copy = (void *)bt_addr_le_copy,
    ._bt_uuid_str = (void *)bt_uuid_str,

    ._uuid_cmp = (void *)bt_uuid_cmp,
    ._uuid_create = (void *)bt_uuid_create,
    ._uuid_to_str = (void *)bt_uuid_to_str,

    ._bt_le_bond_exists = (void *)bt_le_bond_exists,
    ._bt_foreach_bond = (void *)bt_foreach_bond,

    ._scan_cb_register = (void *)bt_le_scan_cb_register,
    ._scan_cb_unregister = (void *)bt_le_scan_cb_unregister,
    ._pa_sync_cb_register = (void *)bt_le_per_adv_sync_cb_register,
    ._pa_sync_cb_unregister = (void *)bt_le_per_adv_sync_cb_unregister,
    ._pa_sync_get_info = (void *)bt_le_per_adv_sync_get_info,
    ._pa_sync_lookup_addr = (void *)bt_le_per_adv_sync_lookup_addr,

    ._conn_cb_register = (void *)bt_conn_cb_register,
    ._conn_cb_unregister = (void *)bt_conn_cb_unregister,
    ._conn_auth_info_cb_register = (void *)bt_conn_auth_info_cb_register,
    ._conn_auth_info_cb_unregister = (void *)bt_conn_auth_info_cb_unregister,
    ._conn_foreach = (void *)bt_conn_foreach,
    ._conn_get_info = (void *)bt_conn_get_info,
    ._conn_index = (void *)bt_conn_index,
    ._conn_lookup_index = (void *)bt_conn_lookup_index,
    ._conn_get_dst = (void *)bt_conn_get_dst,

    ._gatt_svc_register = (void *)bt_gatt_service_register,
    ._gatt_svc_unregister = (void *)bt_gatt_service_unregister,
    ._gatt_get_mtu = (void *)bt_gatt_get_mtu,
    ._gatt_eatt_count = (void *)bt_eatt_count,
    ._gatt_attr_value_handle = (void *)bt_gatt_attr_value_handle,
    ._gatt_foreach_attr_type = (void *)bt_gatt_foreach_attr_type,
    ._gatt_find_by_uuid = (void *)bt_gatt_find_by_uuid,
    ._gatts_attr_read = (void *)bt_gatt_attr_read,
    ._gatts_notify_cb = (void *)bt_gatt_notify_cb,
    ._gatts_notify = (void *)bt_gatt_notify,
    ._gatts_notify_uuid = (void *)bt_gatt_notify_uuid,
    ._gatts_indicate = (void *)bt_gatt_indicate,
    ._gatts_is_subscribed = (void *)bt_gatt_is_subscribed,
    ._gattc_discover = (void *)bt_gatt_discover,
    ._gattc_subscribe = (void *)bt_gatt_subscribe,
    ._gattc_unsubscribe = (void *)bt_gatt_unsubscribe,
    ._gattc_read = (void *)bt_gatt_read,
    ._gattc_write = (void *)bt_gatt_write,
    ._gattc_write_without_response_cb = (void *)bt_gatt_write_without_response_cb,
    ._gattc_write_without_response = (void *)bt_gatt_write_without_response,

#if CONFIG_BT_OTS
    ._ots_init = (void *)bt_ots_init,
    ._ots_obj_add = (void *)bt_ots_obj_add,
    ._ots_free_instance_get = (void *)bt_ots_free_instance_get,
#if CONFIG_BT_OTS_SECONDARY_SVC
    ._ots_svc_decl_get = (void *)bt_ots_svc_decl_get,
#endif /* CONFIG_BT_OTS_SECONDARY_SVC */
#endif /* CONFIG_BT_OTS */
#if CONFIG_BT_OTS_CLIENT
    ._ots_client_register = (void *)bt_ots_client_register,
    ._ots_client_indicate_handler = (void *)bt_ots_client_indicate_handler,
    ._ots_client_read_object_metadata = (void *)bt_ots_client_read_object_metadata,
    ._ots_client_read_object_data = (void *)bt_ots_client_read_object_data,
    ._ots_metadata_display = (void *)bt_ots_metadata_display,
#endif /* CONFIG_BT_OTS_CLIENT */
};

static const struct lib_int_funcs int_funcs = {
#if CONFIG_BT_AICS_CLIENT
    ._aics_client_state_get = lib_aics_client_state_get,
    ._aics_client_gain_setting_get = lib_aics_client_gain_setting_get,
    ._aics_client_type_get = lib_aics_client_type_get,
    ._aics_client_status_get = lib_aics_client_status_get,
    ._aics_client_unmute = lib_aics_client_unmute,
    ._aics_client_mute = lib_aics_client_mute,
    ._aics_client_manual_gain_set = lib_aics_client_manual_gain_set,
    ._aics_client_automatic_gain_set = lib_aics_client_automatic_gain_set,
    ._aics_client_gain_set = lib_aics_client_gain_set,
    ._aics_client_description_get = lib_aics_client_description_get,
    ._aics_client_description_set = lib_aics_client_description_set,
#endif /* CONFIG_BT_AICS_CLIENT */

#if CONFIG_BT_BAP_BROADCAST_SINK
    ._bap_broadcast_sink_has_ep = lib_bap_broadcast_sink_has_ep,
#endif /* CONFIG_BT_BAP_BROADCAST_SINK */

#if CONFIG_BT_BAP_BROADCAST_SOURCE
    ._bap_broadcast_source_has_ep = lib_bap_broadcast_source_has_ep,
#endif /* CONFIG_BT_BAP_BROADCAST_SOURCE */

#if CONFIG_BT_BAP_UNICAST_CLIENT
    ._bap_unicast_client_has_ep = lib_bap_unicast_client_has_ep,
    ._bap_unicast_client_ep_get_conn = lib_bap_unicast_client_ep_get_conn,
    ._bap_unicast_client_register_cb = lib_bap_unicast_client_register_cb,
    ._bap_unicast_client_config = lib_bap_unicast_client_config,
    ._bap_unicast_client_metadata = lib_bap_unicast_client_metadata,
    ._bap_unicast_client_connect = lib_bap_unicast_client_connect,
    ._bap_unicast_client_start = lib_bap_unicast_client_start,
    ._bap_unicast_client_disable = lib_bap_unicast_client_disable,
    ._bap_unicast_client_release = lib_bap_unicast_client_release,
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */

#if CONFIG_BT_BAP_UNICAST_SERVER
    ._bap_unicast_server_has_ep = lib_bap_unicast_server_has_ep,
    ._bap_unicast_server_ep_get_conn = lib_bap_unicast_server_ep_get_conn,
    ._bap_unicast_server_reconfig = lib_bap_unicast_server_reconfig,
    ._bap_unicast_server_start = lib_bap_unicast_server_start,
    ._bap_unicast_server_metadata = lib_bap_unicast_server_metadata,
    ._bap_unicast_server_disable = lib_bap_unicast_server_disable,
    ._bap_unicast_server_release = lib_bap_unicast_server_release,
#endif /* CONFIG_BT_BAP_UNICAST_SERVER */

#if CONFIG_BT_CAP_ACCEPTOR
    ._cap_acceptor_ccids_exist = lib_cap_acceptor_ccids_exist,
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if CONFIG_BT_CSIP_SET_COORDINATOR && (CONFIG_BT_CAP_INITIATOR_UNICAST || CONFIG_BT_CAP_COMMANDER)
    ._cap_common_discover_csis = lib_cap_common_discover_csis,
#endif /* CONFIG_BT_CSIP_SET_COORDINATOR && (CONFIG_BT_CAP_INITIATOR_UNICAST || CONFIG_BT_CAP_COMMANDER) */

#if CONFIG_BT_CAP_COMMANDER
    ._cap_commander_proc_complete = lib_cap_commander_proc_complete,
#endif /* CONFIG_BT_CAP_COMMANDER */

#if CONFIG_BT_CAP_SUBPROC_SUPPORT
    ._cap_common_set_subproc = lib_cap_common_set_subproc,
    ._cap_common_proc_is_type = lib_cap_common_proc_is_type,
    ._cap_common_subproc_is_type = lib_cap_common_subproc_is_type,
#endif /* CONFIG_BT_CAP_SUBPROC_SUPPORT */

#if CONFIG_BT_CAP_INITIATOR_UNICAST
    ._cap_initiator_cp_cb = lib_cap_initiator_cp_cb,
    ._cap_initiator_codec_configured = lib_cap_initiator_codec_configured,
    ._cap_initiator_qos_configured = lib_cap_initiator_qos_configured,
    ._cap_initiator_enabled = lib_cap_initiator_enabled,
    ._cap_initiator_connected = lib_cap_initiator_connected,
    ._cap_initiator_started = lib_cap_initiator_started,
    ._cap_initiator_metadata_updated = lib_cap_initiator_metadata_updated,
    ._cap_initiator_disabled = lib_cap_initiator_disabled,
    ._cap_initiator_stopped = lib_cap_initiator_stopped,
    ._cap_initiator_released = lib_cap_initiator_released,
    ._cap_initiator_unicast_audio_proc_complete = lib_cap_initiator_unicast_audio_proc_complete,
#endif /* CONFIG_BT_CAP_INITIATOR_UNICAST */

#if CONFIG_BT_CAP_HANDOVER
    ._cap_common_active_proc_is_handover = lib_cap_common_active_proc_is_handover,
    ._cap_handover_is_handover_broadcast_source = lib_cap_handover_is_handover_broadcast_source,
    ._cap_handover_complete = lib_cap_handover_complete,
    ._cap_handover_unicast_proc_complete = lib_cap_handover_unicast_proc_complete,
    ._cap_handover_broadcast_source_stopped = lib_cap_handover_broadcast_source_stopped,
    ._cap_handover_unicast_to_broadcast_reception_start = lib_cap_handover_unicast_to_broadcast_reception_start,
    ._cap_handover_broadcast_reception_stopped = lib_cap_handover_broadcast_reception_stopped,
    ._cap_handover_commander_proc_complete = lib_cap_handover_commander_proc_complete,
#endif /* CONFIG_BT_CAP_HANDOVER */

#if CONFIG_BT_CAP && CONFIG_BT_BAP_UNICAST
    ._cap_stream_codec_configured_cb = lib_cap_stream_codec_configured_cb,
    ._cap_stream_qos_configured_cb = lib_cap_stream_qos_configured_cb,
    ._cap_stream_enabled_cb = lib_cap_stream_enabled_cb,
    ._cap_stream_metadata_updated_cb = lib_cap_stream_metadata_updated_cb,
    ._cap_stream_disabled_cb = lib_cap_stream_disabled_cb,
    ._cap_stream_released_cb = lib_cap_stream_released_cb,
#endif /* CONFIG_BT_CAP && CONFIG_BT_BAP_UNICAST */

#if CONFIG_BT_CAP && CONFIG_BT_AUDIO_RX
    ._cap_stream_recv_cb = lib_cap_stream_recv_cb,
#endif /* CONFIG_BT_CAP && CONFIG_BT_AUDIO_RX */

#if CONFIG_BT_CAP && CONFIG_BT_AUDIO_TX
    ._cap_stream_sent_cb = lib_cap_stream_sent_cb,
#endif /* CONFIG_BT_CAP && CONFIG_BT_AUDIO_TX */

#if CONFIG_BT_MCC_OTS
    ._mcc_on_obj_selected = lib_mcc_on_obj_selected,
    ._mcc_on_icon_content = lib_mcc_on_icon_content,
    ._mcc_on_object_metadata = lib_mcc_on_object_metadata,
    ._mcc_on_obj_data_read_failed = lib_mcc_on_obj_data_read_failed,
#endif /* CONFIG_BT_MCC_OTS */

#if CONFIG_BT_MCS && CONFIG_BT_OTS
    ._mcs_get_ots = lib_mcs_get_ots,
    ._mcs_media_proxy_sctrl_current_track_id_cb = lib_mcs_media_proxy_sctrl_current_track_id_cb,
    ._mcs_media_proxy_sctrl_next_track_id_cb = lib_mcs_media_proxy_sctrl_next_track_id_cb,
    ._mcs_media_proxy_sctrl_parent_group_id_cb = lib_mcs_media_proxy_sctrl_parent_group_id_cb,
    ._mcs_media_proxy_sctrl_current_group_id_cb = lib_mcs_media_proxy_sctrl_current_group_id_cb,
    ._mcs_media_proxy_sctrl_search_cb = lib_mcs_media_proxy_sctrl_search_cb,
    ._mcs_media_proxy_sctrl_search_results_id_cb = lib_mcs_media_proxy_sctrl_search_results_id_cb,
    ._mcs_svc_inc_init = lib_mcs_svc_inc_init,
    ._mcs_chrc_icon_obj_id_init = lib_mcs_chrc_icon_obj_id_init,
    ._mcs_chrc_track_segments_obj_id_init = lib_mcs_chrc_track_segments_obj_id_init,
    ._mcs_chrc_current_track_obj_id_init = lib_mcs_chrc_current_track_obj_id_init,
    ._mcs_chrc_next_track_obj_id_init = lib_mcs_chrc_next_track_obj_id_init,
    ._mcs_chrc_parent_group_obj_id_init = lib_mcs_chrc_parent_group_obj_id_init,
    ._mcs_chrc_current_group_obj_id_init = lib_mcs_chrc_current_group_obj_id_init,
    ._mcs_chrc_search_control_point_init = lib_mcs_chrc_search_control_point_init,
    ._mcs_chrc_search_results_obj_id_init = lib_mcs_chrc_search_results_obj_id_init,
#endif /* CONFIG_BT_MCS && CONFIG_BT_OTS */

#if CONFIG_BT_MPL && CONFIG_BT_MCS
    ._mcs_svc_init = lib_mcs_svc_init,
#endif /* CONFIG_BT_MPL && CONFIG_BT_MCS */

/* MCS calls these for its object characteristics with any local player, not only MPL */
#if CONFIG_BT_MCS && CONFIG_BT_OTS
    ._media_proxy_sctrl_get_icon_id = lib_media_proxy_sctrl_get_icon_id,
    ._media_proxy_sctrl_get_track_segments_id = lib_media_proxy_sctrl_get_track_segments_id,
    ._media_proxy_sctrl_get_current_track_id = lib_media_proxy_sctrl_get_current_track_id,
    ._media_proxy_sctrl_set_current_track_id = lib_media_proxy_sctrl_set_current_track_id,
    ._media_proxy_sctrl_get_next_track_id = lib_media_proxy_sctrl_get_next_track_id,
    ._media_proxy_sctrl_set_next_track_id = lib_media_proxy_sctrl_set_next_track_id,
    ._media_proxy_sctrl_get_parent_group_id = lib_media_proxy_sctrl_get_parent_group_id,
    ._media_proxy_sctrl_get_current_group_id = lib_media_proxy_sctrl_get_current_group_id,
    ._media_proxy_sctrl_set_current_group_id = lib_media_proxy_sctrl_set_current_group_id,
    ._media_proxy_sctrl_send_search = lib_media_proxy_sctrl_send_search,
    ._media_proxy_sctrl_get_search_results_id = lib_media_proxy_sctrl_get_search_results_id,
    ._media_proxy_pl_current_track_id_cb = lib_media_proxy_pl_current_track_id_cb,
    ._media_proxy_pl_next_track_id_cb = lib_media_proxy_pl_next_track_id_cb,
    ._media_proxy_pl_parent_group_id_cb = lib_media_proxy_pl_parent_group_id_cb,
    ._media_proxy_pl_current_group_id_cb = lib_media_proxy_pl_current_group_id_cb,
    ._media_proxy_pl_search_cb = lib_media_proxy_pl_search_cb,
    ._media_proxy_pl_search_results_id_cb = lib_media_proxy_pl_search_results_id_cb,
#endif /* CONFIG_BT_MCS && CONFIG_BT_OTS */

#if CONFIG_BT_MICP_MIC_CTLR_AICS
    ._micp_discover_include_func = lib_micp_discover_include_func,
    ._micp_mic_ctrl_aics_init = lib_micp_mic_ctrl_aics_init,
#endif /* CONFIG_BT_MICP_MIC_CTLR_AICS */

#if CONFIG_BT_MICP_MIC_DEV_AICS
    ._micp_mic_dev_prepare_aics_inst = lib_micp_mic_dev_prepare_aics_inst,
#endif /* CONFIG_BT_MICP_MIC_DEV_AICS */

#if CONFIG_BT_MPL_OBJECTS
    ._mpl_get_icon_id = lib_mpl_get_icon_id,
    ._mpl_get_track_segments_id = lib_mpl_get_track_segments_id,
    ._mpl_get_current_track_id = lib_mpl_get_current_track_id,
    ._mpl_set_current_track_id = lib_mpl_set_current_track_id,
    ._mpl_get_next_track_id = lib_mpl_get_next_track_id,
    ._mpl_set_next_track_id = lib_mpl_set_next_track_id,
    ._mpl_get_parent_group_id = lib_mpl_get_parent_group_id,
    ._mpl_get_current_group_id = lib_mpl_get_current_group_id,
    ._mpl_set_current_group_id = lib_mpl_set_current_group_id,
    ._mpl_send_search = lib_mpl_send_search,
    ._mpl_get_search_results_id = lib_mpl_get_search_results_id,
#endif /* CONFIG_BT_MPL_OBJECTS */

#if CONFIG_BT_TBS_CLIENT_TBS
    ._tbs_client_primary_discover_tbs = lib_tbs_client_primary_discover_tbs,
#endif /* CONFIG_BT_TBS_CLIENT_TBS */

#if CONFIG_BT_TBS_CLIENT_GTBS
    ._tbs_client_primary_discover_gtbs = lib_tbs_client_primary_discover_gtbs,
#endif /* CONFIG_BT_TBS_CLIENT_GTBS */

#if CONFIG_BT_TBS_CLIENT_CCID
    ._tbs_client_get_by_ccid = lib_tbs_client_get_by_ccid,
#endif /* CONFIG_BT_TBS_CLIENT_CCID */

#if CONFIG_BT_TBS_CLIENT
    ._tbs_client_get_by_index = lib_tbs_client_get_by_index,
#endif /* CONFIG_BT_TBS_CLIENT */

#if CONFIG_BT_VCP_VOL_CTLR_AICS
    ._vcp_vol_ctlr_aics_init = lib_vcp_vol_ctlr_aics_init,
#endif /* CONFIG_BT_VCP_VOL_CTLR_AICS */

#if CONFIG_BT_VCP_VOL_CTLR_VOCS
    ._vcp_vol_ctlr_vocs_init = lib_vcp_vol_ctlr_vocs_init,
#endif /* CONFIG_BT_VCP_VOL_CTLR_VOCS */

#if CONFIG_BT_VCP_VOL_CTLR_AICS || CONFIG_BT_VCP_VOL_CTLR_VOCS
    ._vcp_vol_ctlr_vcs_discover_include_func = lib_vcp_vol_ctlr_vcs_discover_include_func,
    ._vcp_vol_ctlr_included_get = lib_vcp_vol_ctlr_included_get,
#endif /* CONFIG_BT_VCP_VOL_CTLR_AICS || CONFIG_BT_VCP_VOL_CTLR_VOCS */

#if (CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT > 0)
    ._vcp_vol_rend_prepare_aics_inst = lib_vcp_vol_rend_prepare_aics_inst,
#endif /* (CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT > 0) */

#if (CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT > 0)
    ._vcp_vol_rend_prepare_vocs_inst = lib_vcp_vol_rend_prepare_vocs_inst,
#endif /* (CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT > 0) */

#if CONFIG_BT_VOCS_CLIENT
    ._vocs_client_state_get = lib_vocs_client_state_get,
    ._vocs_client_state_set = lib_vocs_client_state_set,
    ._vocs_client_location_get = lib_vocs_client_location_get,
    ._vocs_client_location_set = lib_vocs_client_location_set,
    ._vocs_client_description_get = lib_vocs_client_description_get,
    ._vocs_client_description_set = lib_vocs_client_description_set,
#endif /* CONFIG_BT_VOCS_CLIENT */
};

static int lib_audio_resources_init(void)
{
    int err = 0;

#if CONFIG_BT_AICS || CONFIG_BT_AICS_CLIENT
    err = lib_aics_init();
    if (err) {
        LOG_ERR("LibAicsInitFail");
        return err;
    }
#endif /* CONFIG_BT_AICS || CONFIG_BT_AICS_CLIENT */

#if CONFIG_BT_AICS_CLIENT
    err = lib_aics_client_init();
    if (err) {
        LOG_ERR("LibAicsCliInitFail");
        return err;
    }
#endif /* CONFIG_BT_AICS_CLIENT */

#if CONFIG_BT_BAP_BASE
    err = lib_bap_base_init();
    if (err) {
        LOG_ERR("LibBapBaseInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_BASE */

#if CONFIG_BT_BAP_STREAM
    err = lib_bap_iso_init();
    if (err) {
        LOG_ERR("LibBapIsoInitFail");
        return err;
    }

    err = lib_bap_stream_init();
    if (err) {
        LOG_ERR("LibBapStreamInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_STREAM */

#if CONFIG_BT_ASCS
    err = lib_ascs_init();
    if (err) {
        LOG_ERR("LibAscsInitFail");
        return err;
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_PACS
    err = lib_pacs_init();
    if (err) {
        LOG_ERR("LibPacsInitFail");
        return err;
    }
#endif /* CONFIG_BT_PACS */

#if CONFIG_BT_BAP_BROADCAST_SINK
    err = lib_bap_broadcast_sink_init();
    if (err) {
        LOG_ERR("LibBapBsnkInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_BROADCAST_SINK */

#if CONFIG_BT_BAP_BROADCAST_SOURCE
    err = lib_bap_broadcast_source_init();
    if (err) {
        LOG_ERR("LibBapBsrcInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_BROADCAST_SOURCE */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    err = lib_bap_scan_delegator_init();
    if (err) {
        LOG_ERR("LibBapSdeInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_BAP_BROADCAST_ASSISTANT
    err = lib_bap_broadcast_assistant_init();
    if (err) {
        LOG_ERR("LibBapBaInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_BROADCAST_ASSISTANT */

#if CONFIG_BT_BAP_UNICAST_CLIENT
    err = lib_bap_unicast_client_init();
    if (err) {
        LOG_ERR("LibBapUclInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */

#if CONFIG_BT_BAP_UNICAST_SERVER
    err = lib_bap_unicast_server_init();
    if (err) {
        LOG_ERR("LibBapUsrInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_UNICAST_SERVER */

#if CONFIG_BT_CAP_INITIATOR_UNICAST || CONFIG_BT_CAP_COMMANDER
    err = lib_cap_common_init();
    if (err) {
        LOG_ERR("LibCapCommonInitFail");
        return err;
    }
#endif /* CONFIG_BT_CAP_INITIATOR_UNICAST || CONFIG_BT_CAP_COMMANDER */

#if CONFIG_BT_CAP
    err = lib_cap_stream_init();
    if (err) {
        LOG_ERR("LibCapStreamInitFail");
        return err;
    }
#endif /* CONFIG_BT_CAP */

#if CONFIG_BT_CAP_INITIATOR
    err = lib_cap_initiator_init();
    if (err) {
        LOG_ERR("LibCapIniInitFail");
        return err;
    }
#endif /* CONFIG_BT_CAP_INITIATOR */

#if CONFIG_BT_CAP_ACCEPTOR
    err = lib_cap_acceptor_init();
    if (err) {
        LOG_ERR("LibCapAccInitFail");
        return err;
    }
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if CONFIG_BT_CAP_COMMANDER
    err = lib_cap_commander_init();
    if (err) {
        LOG_ERR("LibCapComInitFail");
        return err;
    }
#endif /* CONFIG_BT_CAP_COMMANDER */

    err = lib_ccid_init();
    if (err) {
        LOG_ERR("LibCcidInitFail");
        return err;
    }

#if CONFIG_BT_CCP_CALL_CONTROL_CLIENT
    err = lib_ccp_call_control_client_init();
    if (err) {
        LOG_ERR("LibCcpCallControlClientInitFail");
        return err;
    }
#endif /* CONFIG_BT_CCP_CALL_CONTROL_CLIENT */

#if CONFIG_BT_CCP_CALL_CONTROL_SERVER
    err = lib_ccp_call_control_server_init();
    if (err) {
        LOG_ERR("LibCcpCallControlServerInitFail");
        return err;
    }
#endif /* CONFIG_BT_CCP_CALL_CONTROL_SERVER */

#if CONFIG_BT_BAP_STREAM
    err = lib_codec_init();
    if (err) {
        LOG_ERR("LibCodecInitFail");
        return err;
    }
#endif /* CONFIG_BT_BAP_STREAM */

#if CONFIG_BT_CSIP_SET_MEMBER
    err = lib_csip_set_member_init();
    if (err) {
        LOG_ERR("LibCsipSetMemberInitFail");
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_CSIP_SET_COORDINATOR
    err = lib_csip_set_coordinator_init();
    if (err) {
        LOG_ERR("LibCsipSetCoordinatorInitFail");
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_COORDINATOR */

#if CONFIG_BT_MCC
    err = lib_mcc_init();
    if (err) {
        LOG_ERR("LibMccInitFail");
        return err;
    }
#endif /* CONFIG_BT_MCC */

#if CONFIG_BT_MCS
    err = lib_mcs_init();
    if (err) {
        LOG_ERR("LibMcsInitFail");
        return err;
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_MCTL
    err = lib_media_proxy_init();
    if (err) {
        LOG_ERR("LibMprxInitFail");
        return err;
    }
#endif /* CONFIG_BT_MCTL */

#if CONFIG_BT_MICP_MIC_CTLR
    err = lib_micp_mic_ctlr_init();
    if (err) {
        LOG_ERR("LibMicpMicCtlrInitFail");
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_CTLR */

#if CONFIG_BT_MICP_MIC_DEV
    err = lib_micp_mic_dev_init();
    if (err) {
        LOG_ERR("LibMicpMicDevInitFail");
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */

#if CONFIG_BT_MPL
    err = lib_mpl_init();
    if (err) {
        LOG_ERR("LibMplInitFail");
        return err;
    }
#endif /* CONFIG_BT_MPL */

#if CONFIG_BT_PBP
    err = lib_pbp_init();
    if (err) {
        LOG_ERR("LibPbpInitFail");
        return err;
    }
#endif /* CONFIG_BT_PBP */

#if CONFIG_BT_TBS_CLIENT
    err = lib_tbs_client_init();
    if (err) {
        LOG_ERR("LibTbsCliInitFail");
        return err;
    }
#endif /* CONFIG_BT_TBS_CLIENT */

#if CONFIG_BT_TBS
    err = lib_tbs_init();
    if (err) {
        LOG_ERR("LibTbsInitFail");
        return err;
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_TMAP
    err = lib_tmap_init();
    if (err) {
        LOG_ERR("LibTmapInitFail");
        return err;
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_VCP_VOL_CTLR
    err = lib_vcp_vol_ctlr_init();
    if (err) {
        LOG_ERR("LibVcpVolCtlrInitFail");
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_CTLR */

#if CONFIG_BT_VCP_VOL_REND
    err = lib_vcp_vol_rend_init();
    if (err) {
        LOG_ERR("LibVcpVolRendInitFail");
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_VOCS || CONFIG_BT_VOCS_CLIENT
    err = lib_vocs_init();
    if (err) {
        LOG_ERR("LibVocsInitFail");
        return err;
    }
#endif /* CONFIG_BT_VOCS || CONFIG_BT_VOCS_CLIENT */

#if CONFIG_BT_VOCS_CLIENT
    err = lib_vocs_client_init();
    if (err) {
        LOG_ERR("LibVocsClientInitFail");
        return err;
    }
#endif /* CONFIG_BT_VOCS_CLIENT */

#if CONFIG_BT_HAS
    err = lib_has_init();
    if (err) {
        LOG_ERR("LibHasInitFail");
        return err;
    }
#endif /* CONFIG_BT_HAS */

#if CONFIG_BT_HAS_CLIENT
    err = lib_has_client_init();
    if (err) {
        LOG_ERR("LibHasCliInitFail");
        return err;
    }
#endif /* CONFIG_BT_HAS_CLIENT */

#if CONFIG_BT_GMAP
    err = lib_gmap_client_init();
    if (err) {
        LOG_ERR("LibGmapCliInitFail");
        return err;
    }
#endif /* CONFIG_BT_GMAP */

    return err;
}

/* Reverse order of lib_audio_resources_init(): a module may reference an earlier
 * one. Failures log and continue - stopping half way leaves more behind. Keep
 * every lib_*_init() listed here; a missing counterpart leaks silently. */
static void lib_audio_resources_deinit(void)
{
#if CONFIG_BT_GMAP
    /* No init counterpart above: bt_gmap_register() allocates GMAS on demand,
     * so only the deinit side is listed. */
    if (lib_gmap_server_deinit()) {
        LOG_ERR("LibGmapSrvDeinitFail");
    }

    if (lib_gmap_client_deinit()) {
        LOG_ERR("LibGmapCliDeinitFail");
    }
#endif /* CONFIG_BT_GMAP */

#if CONFIG_BT_HAS_CLIENT
    if (lib_has_client_deinit()) {
        LOG_ERR("LibHasCliDeinitFail");
    }
#endif /* CONFIG_BT_HAS_CLIENT */

#if CONFIG_BT_HAS
    if (lib_has_deinit()) {
        LOG_ERR("LibHasDeinitFail");
    }
#endif /* CONFIG_BT_HAS */

#if CONFIG_BT_VOCS_CLIENT
    if (lib_vocs_client_deinit()) {
        LOG_ERR("LibVocsClientDeinitFail");
    }
#endif /* CONFIG_BT_VOCS_CLIENT */

#if CONFIG_BT_VOCS || CONFIG_BT_VOCS_CLIENT
    if (lib_vocs_deinit()) {
        LOG_ERR("LibVocsDeinitFail");
    }
#endif /* CONFIG_BT_VOCS || CONFIG_BT_VOCS_CLIENT */

#if CONFIG_BT_VCP_VOL_REND
    if (lib_vcp_vol_rend_deinit()) {
        LOG_ERR("LibVcpVolRendDeinitFail");
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_VCP_VOL_CTLR
    if (lib_vcp_vol_ctlr_deinit()) {
        LOG_ERR("LibVcpVolCtlrDeinitFail");
    }
#endif /* CONFIG_BT_VCP_VOL_CTLR */

#if CONFIG_BT_TMAP
    if (lib_tmap_deinit()) {
        LOG_ERR("LibTmapDeinitFail");
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_TBS
    if (lib_tbs_deinit()) {
        LOG_ERR("LibTbsDeinitFail");
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_TBS_CLIENT
    if (lib_tbs_client_deinit()) {
        LOG_ERR("LibTbsCliDeinitFail");
    }
#endif /* CONFIG_BT_TBS_CLIENT */

#if CONFIG_BT_PBP
    if (lib_pbp_deinit()) {
        LOG_ERR("LibPbpDeinitFail");
    }
#endif /* CONFIG_BT_PBP */

#if CONFIG_BT_MPL
    if (lib_mpl_deinit()) {
        LOG_ERR("LibMplDeinitFail");
    }
#endif /* CONFIG_BT_MPL */

#if CONFIG_BT_MICP_MIC_DEV
    if (lib_micp_mic_dev_deinit()) {
        LOG_ERR("LibMicpMicDevDeinitFail");
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */

#if CONFIG_BT_MICP_MIC_CTLR
    if (lib_micp_mic_ctlr_deinit()) {
        LOG_ERR("LibMicpMicCtlrDeinitFail");
    }
#endif /* CONFIG_BT_MICP_MIC_CTLR */

#if CONFIG_BT_MCTL
    if (lib_media_proxy_deinit()) {
        LOG_ERR("LibMediaProxyDeinitFail");
    }
#endif /* CONFIG_BT_MCTL */

#if CONFIG_BT_MCS
    if (lib_mcs_deinit()) {
        LOG_ERR("LibMcsDeinitFail");
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_MCC
    if (lib_mcc_deinit()) {
        LOG_ERR("LibMccDeinitFail");
    }
#endif /* CONFIG_BT_MCC */

#if CONFIG_BT_CSIP_SET_COORDINATOR
    if (lib_csip_set_coordinator_deinit()) {
        LOG_ERR("LibCsipSetCoordinatorDeinitFail");
    }
#endif /* CONFIG_BT_CSIP_SET_COORDINATOR */

#if CONFIG_BT_CSIP_SET_MEMBER
    if (lib_csip_set_member_deinit()) {
        LOG_ERR("LibCsipSetMemberDeinitFail");
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_BAP_STREAM
    if (lib_codec_deinit()) {
        LOG_ERR("LibCodecDeinitFail");
    }
#endif /* CONFIG_BT_BAP_STREAM */

#if CONFIG_BT_CCP_CALL_CONTROL_SERVER
    if (lib_ccp_call_control_server_deinit()) {
        LOG_ERR("LibCcpCallControlServerDeinitFail");
    }
#endif /* CONFIG_BT_CCP_CALL_CONTROL_SERVER */

#if CONFIG_BT_CCP_CALL_CONTROL_CLIENT
    if (lib_ccp_call_control_client_deinit()) {
        LOG_ERR("LibCcpCallControlClientDeinitFail");
    }
#endif /* CONFIG_BT_CCP_CALL_CONTROL_CLIENT */

    if (lib_ccid_deinit()) {
        LOG_ERR("LibCcidDeinitFail");
    }

#if CONFIG_BT_CAP_COMMANDER
    if (lib_cap_commander_deinit()) {
        LOG_ERR("LibCapComDeinitFail");
    }
#endif /* CONFIG_BT_CAP_COMMANDER */

#if CONFIG_BT_CAP_ACCEPTOR
    if (lib_cap_acceptor_deinit()) {
        LOG_ERR("LibCapAccDeinitFail");
    }
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if CONFIG_BT_CAP_INITIATOR
    if (lib_cap_initiator_deinit()) {
        LOG_ERR("LibCapIniDeinitFail");
    }
#endif /* CONFIG_BT_CAP_INITIATOR */

#if CONFIG_BT_CAP
    if (lib_cap_stream_deinit()) {
        LOG_ERR("LibCapStreamDeinitFail");
    }
#endif /* CONFIG_BT_CAP */

#if CONFIG_BT_CAP_INITIATOR_UNICAST || CONFIG_BT_CAP_COMMANDER
    if (lib_cap_common_deinit()) {
        LOG_ERR("LibCapCommonDeinitFail");
    }
#endif /* CONFIG_BT_CAP_INITIATOR_UNICAST || CONFIG_BT_CAP_COMMANDER */

#if CONFIG_BT_BAP_UNICAST_SERVER
    if (lib_bap_unicast_server_deinit()) {
        LOG_ERR("LibBapUniSrvDeinitFail");
    }
#endif /* CONFIG_BT_BAP_UNICAST_SERVER */

#if CONFIG_BT_BAP_UNICAST_CLIENT
    if (lib_bap_unicast_client_deinit()) {
        LOG_ERR("LibBapUniCliDeinitFail");
    }
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */

#if CONFIG_BT_BAP_BROADCAST_ASSISTANT
    if (lib_bap_broadcast_assistant_deinit()) {
        LOG_ERR("LibBapBaDeinitFail");
    }
#endif /* CONFIG_BT_BAP_BROADCAST_ASSISTANT */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    if (lib_bap_scan_delegator_deinit()) {
        LOG_ERR("LibBapSdeDeinitFail");
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_BAP_BROADCAST_SOURCE
    if (lib_bap_broadcast_source_deinit()) {
        LOG_ERR("LibBapBsrcDeinitFail");
    }
#endif /* CONFIG_BT_BAP_BROADCAST_SOURCE */

#if CONFIG_BT_BAP_BROADCAST_SINK
    if (lib_bap_broadcast_sink_deinit()) {
        LOG_ERR("LibBapBsnkDeinitFail");
    }
#endif /* CONFIG_BT_BAP_BROADCAST_SINK */

#if CONFIG_BT_PACS
    if (lib_pacs_deinit()) {
        LOG_ERR("LibPacsDeinitFail");
    }
#endif /* CONFIG_BT_PACS */

#if CONFIG_BT_ASCS
    if (lib_ascs_deinit()) {
        LOG_ERR("LibAscsDeinitFail");
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_BAP_STREAM
    if (lib_bap_stream_deinit()) {
        LOG_ERR("LibBapStreamDeinitFail");
    }

    if (lib_bap_iso_deinit()) {
        LOG_ERR("LibBapIsoDeinitFail");
    }
#endif /* CONFIG_BT_BAP_STREAM */

#if CONFIG_BT_BAP_BASE
    if (lib_bap_base_deinit()) {
        LOG_ERR("LibBapBaseDeinitFail");
    }
#endif /* CONFIG_BT_BAP_BASE */

#if CONFIG_BT_AICS_CLIENT
    if (lib_aics_client_deinit()) {
        LOG_ERR("LibAicsClientDeinitFail");
    }
#endif /* CONFIG_BT_AICS_CLIENT */

#if CONFIG_BT_AICS || CONFIG_BT_AICS_CLIENT
    if (lib_aics_deinit()) {
        LOG_ERR("LibAicsDeinitFail");
    }
#endif /* CONFIG_BT_AICS || CONFIG_BT_AICS_CLIENT */
}

int bt_le_audio_init(void)
{
    int err;

    err = lib_audio_ext_cfgs_set(&ext_cfgs);
    if (err) {
        LOG_ERR("LibExtCfgsSetFail");
        return err;
    }

    err = lib_audio_ext_funcs_set(&ext_funcs);
    if (err) {
        LOG_ERR("LibExtFuncsSetFail");
        return err;
    }

    err = lib_audio_int_funcs_set(&int_funcs);
    if (err) {
        LOG_ERR("LibIntFuncsSetFail");
        return err;
    }

    esp_log_write(ESP_LOG_INFO, LEA_TAG,
                  BT_ISO_LOG_COLOR_I
                  "I (%lu) %s: BLE Audio lib commit: [%s]"
                  BT_ISO_LOG_RESET_COLOR "\n",
                  esp_log_timestamp(), LEA_TAG,
                  lib_audio_commit_get());

    err = lib_audio_resources_init();
    if (err) {
        goto deinit_lib;
    }

#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    err = bt_le_l2cap_ots_init();
    if (err) {
        goto deinit_lib;
    }
#endif

#if CONFIG_BT_BLUEDROID_ENABLED
    err = bt_le_bluedroid_audio_init();
#else
    err = bt_le_nimble_audio_init();
#endif
    if (err) {
        goto deinit_ots;
    }

    return 0;

deinit_ots:
#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    bt_le_l2cap_ots_deinit();
#endif
deinit_lib:
    lib_audio_resources_deinit();
    return err;
}

void bt_le_audio_deinit(void)
{
#if CONFIG_BT_BLUEDROID_ENABLED
    bt_le_bluedroid_audio_deinit();
#else
    bt_le_nimble_audio_deinit();
#endif

#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    bt_le_l2cap_ots_deinit();
#endif

    lib_audio_resources_deinit();
}

#if BLE_AUDIO_SVC_DEFERRED_ADD
#if CONFIG_BT_ASCS
int bt_le_ascs_init(void)
{
    LOG_DBG("AscsInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_ascs_init();
#else
    return bt_le_nimble_ascs_init();
#endif
}

int bt_le_ascs_deinit(void)
{
    LOG_DBG("AscsDeinit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_ascs_deinit();
#else
    return bt_le_nimble_ascs_deinit();
#endif
}
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
int bt_le_bass_init(void)
{
    LOG_DBG("BassInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_bass_init();
#else
    return bt_le_nimble_bass_init();
#endif
}

int bt_le_bass_deinit(void)
{
    LOG_DBG("BassDeinit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_bass_deinit();
#else
    return bt_le_nimble_bass_deinit();
#endif
}
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_TMAP
int bt_le_tmas_init(void)
{
    LOG_DBG("TmasInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_tmas_init();
#else
    return bt_le_nimble_tmas_init();
#endif
}
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_TBS
int bt_le_gtbs_init(void)
{
    LOG_DBG("GtbsInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_gtbs_init();
#else
    return bt_le_nimble_gtbs_init();
#endif
}

int bt_le_gtbs_deinit(void)
{
    LOG_DBG("GtbsDeinit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_gtbs_deinit();
#else
    return bt_le_nimble_gtbs_deinit();
#endif
}

int bt_le_tbs_init(void)
{
    LOG_DBG("TbsInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_tbs_init();
#else
    return bt_le_nimble_tbs_init();
#endif
}

int bt_le_tbs_deinit(uint8_t bearer_index)
{
    LOG_DBG("TbsDeinit[%u]", bearer_index);

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_tbs_deinit(bearer_index);
#else
    return bt_le_nimble_tbs_deinit(bearer_index);
#endif
}
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_HAS
int bt_le_has_init(void)
{
    LOG_DBG("HasInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_has_init();
#else
    return bt_le_nimble_has_init();
#endif
}
#endif /* CONFIG_BT_HAS */

#if CONFIG_BT_MCS
int bt_le_media_proxy_pl_init(void)
{
    LOG_DBG("MprxPlInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_media_proxy_pl_init();
#else
    return bt_le_nimble_media_proxy_pl_init();
#endif
}
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_VCP_VOL_REND
int bt_le_vcp_vol_rend_init(void)
{
    LOG_DBG("VcpVolRendInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_vcp_vol_rend_init();
#else
    return bt_le_nimble_vcp_vol_rend_init();
#endif
}
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
int bt_le_micp_mic_dev_init(void)
{
    LOG_DBG("MicpMicDevInit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_micp_mic_dev_init();
#else
    return bt_le_nimble_micp_mic_dev_init();
#endif
}
#endif /* CONFIG_BT_MICP_MIC_DEV */
#endif /* BLE_AUDIO_SVC_DEFERRED_ADD */

#if CONFIG_BT_PACS
int bt_le_pacs_deinit(void)
{
    LOG_DBG("PacsDeinit");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_pacs_deinit();
#else
    return bt_le_nimble_pacs_deinit();
#endif
}
#endif /* CONFIG_BT_PACS */

#if CONFIG_BT_CSIP_SET_MEMBER
int bt_le_csis_deinit(const void *svc_inst)
{
    struct bt_gatt_service *csis_svc;

    LOG_DBG("CsisDeinit");

    csis_svc = lib_csip_set_member_svc_get(svc_inst);
    if (!csis_svc) {
        LOG_ERR("CsisSvcGetFail");
        return -ENODEV;
    }

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_csis_deinit(csis_svc);
#else
    return bt_le_nimble_csis_deinit(csis_svc);
#endif
}
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

int bt_le_audio_start(void *info)
{
    LOG_DBG("AudioStart");

#if CONFIG_BT_BLUEDROID_ENABLED
    return bt_le_bluedroid_audio_start(info);
#else
    return bt_le_nimble_audio_start(info);
#endif
}

void bt_le_audio_lib_compressed_out(uint8_t log_level, uint32_t log_index, size_t arg_cnt, ...)
{
#if CONFIG_BLE_ISO_COMPRESSED_LOG_ENABLE
    if (CONFIG_BT_ISO_LOG_LEVEL >= log_level) {
        va_list args;
        va_start(args, arg_cnt);
        extern int ble_log_compressed_hex_printv(uint8_t source, uint32_t log_index,
                                                 size_t args_cnt, va_list args);
        ble_log_compressed_hex_printv(BLE_COMPRESSED_LOG_OUT_SOURCE_AUDIO_LIB,
                                      log_index, arg_cnt, args);
        va_end(args);
    }
#else
    ARG_UNUSED(log_level);
    ARG_UNUSED(log_index);
    ARG_UNUSED(arg_cnt);
#endif
}

void bt_le_audio_lib_compressed_buf_out(uint8_t log_level, uint32_t log_index, uint8_t buf_idx,
                                        const uint8_t *buf, size_t len)
{
#if CONFIG_BLE_ISO_COMPRESSED_LOG_ENABLE
    if (CONFIG_BT_ISO_LOG_LEVEL >= log_level) {
        extern int ble_log_compressed_hex_print_buf(uint8_t source, uint32_t log_index,
                                                    uint8_t buf_idx, const uint8_t *buf,
                                                    size_t len);
        ble_log_compressed_hex_print_buf(BLE_COMPRESSED_LOG_OUT_SOURCE_AUDIO_LIB,
                                         log_index, buf_idx, buf, len);
    }
#else
    ARG_UNUSED(log_level);
    ARG_UNUSED(log_index);
    ARG_UNUSED(buf_idx);
    ARG_UNUSED(buf);
    ARG_UNUSED(len);
#endif
}
