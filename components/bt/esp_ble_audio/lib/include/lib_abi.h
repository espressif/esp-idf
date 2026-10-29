/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Interface between the prebuilt BLE Audio lib and IDF; both sides build against this file. */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEA_VERSION     (0x20261102)

enum bt_bap_ascs_reason;
enum bt_bap_ascs_rsp_code;
enum bt_cap_common_proc_type;
enum bt_cap_common_subproc_type;
enum bt_conn_type;
struct bt_aics;
struct bt_audio_codec_cfg;
struct bt_bap_ep;
struct bt_bap_qos_cfg_pref;
struct bt_bap_stream;
struct bt_bap_unicast_client_cb;
struct bt_cap_broadcast_source;
struct bt_cap_common_client;
struct bt_cap_common_proc;
struct bt_cap_stream;
struct bt_conn;
struct bt_conn_auth_info_cb;
struct bt_conn_cb;
struct bt_conn_info;
struct bt_gatt_attr;
struct bt_gatt_discover_params;
struct bt_gatt_subscribe_params;
struct bt_iso_recv_info;
struct bt_le_per_adv_sync;
struct bt_le_per_adv_sync_cb;
struct bt_le_per_adv_sync_info;
struct bt_le_scan_cb;
struct bt_micp_mic_dev_register_param;
struct bt_ots;
struct bt_ots_cb;
struct bt_ots_client;
struct bt_tbs_instance;
struct bt_uuid;
struct bt_vcp_included;
struct bt_vcp_vol_ctlr;
struct bt_vcp_vol_rend_register_param;
struct bt_vocs_client;
struct mpl_search;

struct lib_ext_cfgs {
    /* BLE */
    bool     config_past_sender;
    bool     config_past_receiver;
    bool     config_past_check;
    uint8_t  config_max_conn;
    uint8_t  config_max_paired;
    uint16_t config_max_attr_len;
    uint16_t config_l2cap_tx_mtu;
    uint16_t config_l2cap_rx_mtu;

    /* ISO */
    bool     config_iso_test_params;
    uint8_t  config_iso_max_chan;

    /* Audio (Audio Profiles Related) */
    bool     config_audio_tx;
    bool     config_audio_rx;
    uint16_t config_audio_notify_retry_delay;
    bool     config_audio_notify_ignore_ccc;

    /* AICS (Audio Input Control Service) */
    bool     config_aics;
    uint8_t  config_aics_max_instance_count;
    uint16_t config_aics_max_input_description_size;
    bool     config_aics_client;
    uint8_t  config_aics_client_max_instance_count;

    /* ASCS (Audio Stream Control Service) */
    bool     config_ascs_ase_snk;
    uint8_t  config_ascs_max_ase_snk_count;
    bool     config_ascs_ase_src;
    uint8_t  config_ascs_max_ase_src_count;
    uint8_t  config_ascs_max_active_ases;
    uint16_t config_ascs_iso_disconnect_delay;

    /* BAP (Basic Audio Profile) */
    bool     config_bap_unicast_server;
    bool     config_bap_unicast_client;
    uint8_t  config_bap_unicast_client_group_count;
    uint8_t  config_bap_unicast_client_group_stream_count;
    bool     config_bap_unicast_client_ase_snk;
    uint8_t  config_bap_unicast_client_ase_snk_count;
    bool     config_bap_unicast_client_ase_src;
    uint8_t  config_bap_unicast_client_ase_src_count;
    bool     config_bap_broadcast_sink;
    uint8_t  config_bap_broadcast_snk_count;
    uint8_t  config_bap_broadcast_snk_subgroup_count;
    uint8_t  config_bap_broadcast_snk_stream_count;
    bool     config_bap_broadcast_source;
    uint8_t  config_bap_broadcast_src_count;
    uint8_t  config_bap_broadcast_src_subgroup_count;
    uint8_t  config_bap_broadcast_src_stream_count;
    bool     config_bap_scan_delegator;
    uint8_t  config_bap_scan_delegator_recv_state_count;
    bool     config_bap_broadcast_assistant;
    uint8_t  config_bap_broadcast_assistant_recv_state_count;
    uint8_t  config_bap_bass_max_subgroups;
    uint8_t  config_audio_codec_cfg_max_data_size;
    uint8_t  config_audio_codec_cfg_max_metadata_size;
    uint8_t  config_audio_codec_cap_max_data_size;
    uint8_t  config_audio_codec_cap_max_metadata_size;
    bool     config_bap_debug_stream_data;
    bool     config_bap_debug_stream_seq_num;

    /* CAP (Common Audio Profile) */
    bool     config_cap_acceptor;
    bool     config_cap_acceptor_set_member;
    bool     config_cap_initiator;
    bool     config_cap_initiator_unicast;
    bool     config_cap_commander;
    bool     config_cap_handover;
    bool     config_cap_subproc_support;

    /* CCP (Call Control Profile) */
    bool     config_ccp_call_control_client;
    uint8_t  config_ccp_call_control_client_bearer_count;
    bool     config_ccp_call_control_server;
    uint8_t  config_ccp_call_control_server_bearer_count;
    uint16_t config_ccp_call_control_server_provider_name_max_len;
    uint16_t config_ccp_call_control_server_uri_schemes_max_len;

    /* CSIP (Coordinated Set Identification Profile) */
    bool     config_csip_set_member;
    uint8_t  config_csip_set_member_max_instance_count;
    bool     config_csip_set_member_enc_sirk_support;
    bool     config_csip_set_member_sirk_notifiable;
    bool     config_csip_set_member_size_support;
    bool     config_csip_set_member_size_notifiable;
    bool     config_csip_set_member_lock_support;
    bool     config_csip_set_member_rank_support;
    bool     config_csip_set_member_set_name_notifiable;
    bool     config_csip_set_member_test_sample_data;
    bool     config_csip_set_coordinator;
    uint8_t  config_csip_set_coordinator_max_csis_instances;
    bool     config_csip_set_coordinator_enc_sirk_support;
    bool     config_csip_set_coordinator_test_sample_data;

    /* GMAP (Gaming Audio Profile) */
    bool     config_gmap;
    bool     config_gmap_ugg_supported;
    bool     config_gmap_ugt_supported;
    bool     config_gmap_bgs_supported;
    bool     config_gmap_bgr_supported;

    /* HAS (Hearing Access Service) */
    bool     config_has;
    uint8_t  config_has_preset_count;
    bool     config_has_features_notifiable;
    bool     config_has_preset_support;
    bool     config_has_preset_name_dynamic;
    bool     config_has_preset_control_point_notifiable;
    bool     config_has_active_preset_index;
    bool     config_has_client;

    /* MCS (Media Control Service) */
    bool     config_mcs;
    uint8_t  config_mcs_instance_count;
    bool     config_mcc;
    uint8_t  config_mcc_media_player_name_max;
    uint8_t  config_mcc_icon_url_max;
    uint8_t  config_mcc_track_title_max;
    uint8_t  config_mcc_segment_name_max;
    bool     config_mcc_ots;
    uint16_t config_mcc_otc_obj_buf_size;
    uint16_t config_mcc_total_obj_content_mem;
    uint16_t config_mcc_track_segs_max_cnt;
    uint16_t config_mcc_group_records_max;
    bool     config_mcc_read_media_player_icon_url;
    bool     config_mcc_read_track_title;
    bool     config_mcc_read_track_title_enable_subscription;
    bool     config_mcc_read_track_duration;
    bool     config_mcc_read_track_position;
    bool     config_mcc_set_track_position;
    bool     config_mcc_read_playback_speed;
    bool     config_mcc_set_playback_speed;
    bool     config_mcc_read_seeking_speed;
    bool     config_mcc_read_playing_order;
    bool     config_mcc_set_playing_order;
    bool     config_mcc_read_playing_order_supported;
    bool     config_mcc_read_media_state;
    bool     config_mcc_set_media_control_point;
    bool     config_mcc_read_media_control_point_opcodes_supported;
    bool     config_mcc_read_content_control_id;

    /* MCTL (Media Control) */
    bool     config_mctl;
    bool     config_mctl_local_player_control;
    bool     config_mctl_local_player_local_control;
    bool     config_mctl_local_player_remote_control;
    bool     config_mctl_remote_player_control;
    bool     config_mctl_remote_player_control_objects;

    /* MICP (Microphone Control Profile) */
    bool     config_micp_mic_dev;
    bool     config_micp_mic_dev_aics;
    uint8_t  config_micp_mic_dev_aics_instance_count;
    bool     config_micp_mic_ctlr;
    bool     config_micp_mic_ctlr_aics;
    uint8_t  config_micp_mic_ctlr_max_aics_inst;

    /* MPL (Media player) */
    bool     config_mpl;
    char    *config_mpl_media_player_name;
    uint8_t  config_mpl_media_player_name_max;
    char    *config_mpl_icon_url;
    uint8_t  config_mpl_icon_url_max;
    uint8_t  config_mpl_track_title_max;
    uint8_t  config_mpl_segment_name_max;
    uint8_t  config_mpl_group_title_max;
    bool     config_mpl_objects;
    uint16_t config_mpl_max_obj_size;
    uint16_t config_mpl_icon_bitmap_size;
    uint16_t config_mpl_track_max_size;

    /* PACS (Published Audio Capabilities Service) */
    bool     config_pac_snk;
    bool     config_pac_snk_notifiable;
    bool     config_pac_snk_loc;
    bool     config_pac_snk_loc_writeable;
    bool     config_pac_snk_loc_notifiable;
    bool     config_pac_src;
    bool     config_pac_src_notifiable;
    bool     config_pac_src_loc;
    bool     config_pac_src_loc_writeable;
    bool     config_pac_src_loc_notifiable;
    bool     config_pacs_supported_context_notifiable;

    /* PBP (Public Broadcast Profile) */
    bool     config_pbp;

    /* TBS (Telephone Bearer Service) */
    bool     config_tbs;
    uint8_t  config_tbs_supported_features;
    uint8_t  config_tbs_max_calls;
    uint8_t  config_tbs_bearer_count;
    uint16_t config_tbs_max_scheme_list_length;
    bool     config_tbs_client;
    bool     config_tbs_client_gtbs;
    bool     config_tbs_client_tbs;
    uint8_t  config_tbs_client_max_calls;
    uint8_t  config_tbs_client_max_tbs_instances;
    bool     config_tbs_client_bearer_provider_name;
    bool     config_tbs_client_bearer_uci;
    bool     config_tbs_client_bearer_technology;
    bool     config_tbs_client_bearer_uri_schemes_supported_list;
    bool     config_tbs_client_bearer_signal_strength;
    bool     config_tbs_client_read_bearer_signal_interval;
    bool     config_tbs_client_set_bearer_signal_interval;
    bool     config_tbs_client_bearer_list_current_calls;
    bool     config_tbs_client_ccid;
    bool     config_tbs_client_incoming_uri;
    bool     config_tbs_client_status_flags;
    bool     config_tbs_client_cp_procedures;
    bool     config_tbs_client_accept_call;
    bool     config_tbs_client_terminate_call;
    bool     config_tbs_client_hold_call;
    bool     config_tbs_client_retrieve_call;
    bool     config_tbs_client_originate_call;
    bool     config_tbs_client_join_calls;
    bool     config_tbs_client_optional_opcodes;
    bool     config_tbs_client_incoming_call;
    bool     config_tbs_client_call_friendly_name;
    uint8_t  config_tbs_max_uri_length;
    uint16_t config_tbs_max_provider_name_length;
    uint16_t config_tbs_max_friendly_name_length;

    /* TMAP (Telephony and Media Audio Profile) */
    bool     config_tmap_cg_supported;
    bool     config_tmap_ct_supported;
    bool     config_tmap_ums_supported;
    bool     config_tmap_umr_supported;
    bool     config_tmap_bms_supported;
    bool     config_tmap_bmr_supported;
    bool     config_tmap;

    /* VCP (Volume Control Profile) */
    bool     config_vcp_vol_rend;
    bool     config_vcp_vol_rend_vocs;
    uint8_t  config_vcp_vol_rend_vocs_instance_count;
    bool     config_vcp_vol_rend_aics;
    uint8_t  config_vcp_vol_rend_aics_instance_count;
    bool     config_vcp_vol_rend_vol_flags_notifiable;
    bool     config_vcp_vol_ctlr;
    bool     config_vcp_vol_ctlr_vocs;
    uint8_t  config_vcp_vol_ctlr_max_vocs_inst;
    bool     config_vcp_vol_ctlr_aics;
    uint8_t  config_vcp_vol_ctlr_max_aics_inst;

    /* VOCS (Volume Control Service) */
    bool     config_vocs;
    uint8_t  config_vocs_max_instance_count;
    uint16_t config_vocs_max_output_description_size;
    bool     config_vocs_client;
    uint8_t  config_vocs_client_max_instance_count;

    /* OTS (Object Transfer Service) */
    bool     config_ots;
    bool     config_ots_client;
    uint8_t  config_ots_obj_max_name_len;

    /* PTS */
    bool     config_pts_test_enable;

    /* Version Check */
    uint32_t config_version;
};

struct lib_ext_funcs {
    /* LOG */
    void (*_log_dbg)(const char *format, ...);
    void (*_log_inf)(const char *format, ...);
    void (*_log_wrn)(const char *format, ...);
    void (*_log_err)(const char *format, ...);

    /* Fatal assert: log [tag][info=...][file:line][func] then abort.
     * Used via LIB_ASSERT_ALLOC / LIB_ASSERT macros in utils.h.
     * `tag` is a short classifier (e.g. "Alloc", "State"); `info` is
     * a related numeric (alloc size, state value, etc.).
     */
    void (*_assert)(const char *tag, size_t info,
                    const char *file, int line, const char *func);

    /* Memory */
    void *(*_malloc)(size_t size);
    void *(*_calloc)(size_t n, size_t size);
    void (*_free)(void *ptr);

    /* Crypto */
    int (*_rand)(void *buf, size_t len);
    int (*_encrypt_le)(const uint8_t key[16],
                       const uint8_t plaintext[16],
                       uint8_t enc_data[16]);
    int (*_aes_cmac)(const uint8_t *key, const uint8_t *in,
                     size_t len, uint8_t *out);

    /* UTF8 */
    char *(*_utf8_trunc)(char *utf8_str);
    char *(*_utf8_lcpy)(char *dst, const char *src, size_t n);
    int (*_utf8_count_chars)(const char *s);

    /* Mutex */
    void (*_mutex_create)(void *mutex);
    void (*_mutex_delete)(void *mutex);
    int (*_mutex_lock)(void *mutex, uint32_t timeout);
    int (*_mutex_unlock)(void *mutex);
    void (*_lock)(void);
    void (*_unlock)(void);

    /* Net buf */
    void (*_buf_simple_init)(void *buf, size_t reserve_head);
    void (*_buf_simple_init_with_data)(void *buf, void *data, size_t size);
    void (*_buf_simple_reset)(void *buf);
    void (*_buf_simple_clone)(const void *original, void *clone);
    void *(*_buf_simple_add)(void *buf, size_t len);
    void *(*_buf_simple_add_mem)(void *buf, const void *mem, size_t len);
    uint8_t *(*_buf_simple_add_u8)(void *buf, uint8_t val);
    void (*_buf_simple_add_le16)(void *buf, uint16_t val);
    void (*_buf_simple_add_le24)(void *buf, uint32_t val);
    void (*_buf_simple_add_le32)(void *buf, uint32_t val);
    void (*_buf_simple_add_le48)(void *buf, uint64_t val);
    void (*_buf_simple_add_le64)(void *buf, uint64_t val);
    void *(*_buf_simple_pull)(void *buf, size_t len);
    void *(*_buf_simple_pull_mem)(void *buf, size_t len);
    uint8_t (*_buf_simple_pull_u8)(void *buf);
    uint16_t (*_buf_simple_pull_le16)(void *buf);
    uint32_t (*_buf_simple_pull_le24)(void *buf);
    uint32_t (*_buf_simple_pull_le32)(void *buf);
    uint64_t (*_buf_simple_pull_le48)(void *buf);
    uint64_t (*_buf_simple_pull_le64)(void *buf);
    size_t (*_buf_simple_headroom)(const void *buf);
    size_t (*_buf_simple_tailroom)(void *buf);
    void (*_buf_simple_save)(void *buf, void *state);
    void (*_buf_simple_restore)(void *buf, void *state);
    void *(*_buf_user_data)(void *buf);
    size_t (*_buf_frags_len)(void *buf);

    /* Timer */
    int (*_dwork_init)(void *dwork, void *handler);
    int (*_dwork_deinit)(void *dwork);
    int (*_dwork_schedule)(void *dwork, uint32_t delay);
    int (*_dwork_cancel)(void *dwork);
    bool (*_dwork_cancel_sync)(void *dwork, void *sync);
    uint32_t (*_dwork_remaining_get)(void *dwork);
    bool (*_work_is_pending)(void *work);
    int (*_work_submit)(void *work);
    void *(*_dwork_from_work)(void *work);

    /* BT_STR */
    const char *(*_bt_hex)(const void *buf, size_t len);
    const char *(*_bt_addr_str)(const void *addr);
    const char *(*_bt_addr_le_str)(const void *addr);
    int (*_bt_addr_le_to_str)(const void *addr, char *str, size_t len);
    bool (*_bt_addr_le_eq)(const void *a, const void *b);
    void (*_bt_addr_copy)(void *dst, const void *src);
    void (*_bt_addr_le_copy)(void *dst, const void *src);
    const char *(*_bt_uuid_str)(const struct bt_uuid *uuid);

    /* UUID */
    int (*_uuid_cmp)(const struct bt_uuid *u1, const struct bt_uuid *u2);
    bool (*_uuid_create)(struct bt_uuid *uuid, const uint8_t *data, uint8_t data_len);
    void (*_uuid_to_str)(const struct bt_uuid *uuid, char *str, size_t len);

    /* SMP */
    bool (*_bt_le_bond_exists)(uint8_t id, const void *addr);
    void (*_bt_foreach_bond)(uint8_t id, void *func, void *user_data);

    /* Scan */
    int (*_scan_cb_register)(struct bt_le_scan_cb *cb);
    void (*_scan_cb_unregister)(struct bt_le_scan_cb *cb);
    int (*_pa_sync_cb_register)(struct bt_le_per_adv_sync_cb *cb);
    int (*_pa_sync_cb_unregister)(struct bt_le_per_adv_sync_cb *cb);
    int (*_pa_sync_get_info)(struct bt_le_per_adv_sync *per_adv_sync,
                             struct bt_le_per_adv_sync_info *info);
    struct bt_le_per_adv_sync *(*_pa_sync_lookup_addr)(const void *adv_addr, uint8_t sid);

    /* Connection */
    int (*_conn_cb_register)(struct bt_conn_cb *cb);
    int (*_conn_cb_unregister)(struct bt_conn_cb *cb);
    int (*_conn_auth_info_cb_register)(struct bt_conn_auth_info_cb *cb);
    int (*_conn_auth_info_cb_unregister)(struct bt_conn_auth_info_cb *cb);
    void (*_conn_foreach)(enum bt_conn_type type,
                          void (*func)(struct bt_conn *conn, void *data),
                          void *data);
    int (*_conn_get_info)(const struct bt_conn *conn, struct bt_conn_info *info);
    uint8_t (*_conn_index)(const struct bt_conn *conn);
    struct bt_conn *(*_conn_lookup_index)(uint8_t index);
    const void *(*_conn_get_dst)(const struct bt_conn *conn);
    struct bt_conn *(*_conn_ref)(struct bt_conn *conn);
    void (*_conn_unref)(struct bt_conn *conn);

    /* GATT */
    int (*_gatt_svc_register)(void *svc);
    int (*_gatt_svc_unregister)(void *svc);
    uint16_t (*_gatt_get_mtu)(void *conn);
    size_t (*_gatt_eatt_count)(void *conn);
    uint16_t (*_gatt_attr_value_handle)(const void *attr);
    void (*_gatt_foreach_attr_type)(uint16_t start_handle, uint16_t end_handle,
                                    const void *uuid, const void *attr_data,
                                    uint16_t num_matches, void *func, void *user_data);
    void *(*_gatt_find_by_uuid)(const void *attr, uint16_t attr_count, const void *uuid);
    ssize_t (*_gatts_attr_read)(void *conn, const void *attr, void *buf, uint16_t buf_len,
                                uint16_t offset, const void *value, uint16_t value_len);
    int (*_gatts_notify_cb)(void *conn, void *params);
    int (*_gatts_notify)(void *conn, const void *attr, const void *data, uint16_t len);
    int (*_gatts_notify_uuid)(void *conn, const void *uuid, const void *attr,
                              const void *data, uint16_t len);
    int (*_gatts_indicate)(void *conn, void *params);
    bool (*_gatts_is_subscribed)(void *conn, const void *attr, uint16_t ccc_type);
    int (*_gattc_discover)(void *conn, void *params);
    int (*_gattc_subscribe)(void *conn, void *params);
    int (*_gattc_unsubscribe)(void *conn, void *params);
    int (*_gattc_read)(void *conn, void *params);
    int (*_gattc_write)(void *conn, void *params);
    int (*_gattc_write_without_response_cb)(void *conn, uint16_t handle, const void *data,
                                            uint16_t length, bool sign, void *func,
                                            void *user_data);
    int (*_gattc_write_without_response)(void *conn, uint16_t handle, const void *data,
                                         uint16_t length, bool sign);

    /* OTS */
    int (*_ots_init)(void *ots, void *ots_init);
    int (*_ots_obj_add)(void *ots, const void *param);
    void *(*_ots_free_instance_get)(void);
    void *(*_ots_svc_decl_get)(void *ots);
    int (*_ots_client_register)(void *ots_inst);
    uint8_t (*_ots_client_indicate_handler)(struct bt_conn *conn,
                                            struct bt_gatt_subscribe_params *params,
                                            const void *data, uint16_t length);
    int (*_ots_client_read_object_metadata)(void *otc_inst, void *conn, uint8_t metadata);
    int (*_ots_client_read_object_data)(void *otc_inst, void *conn);
    void (*_ots_metadata_display)(void *metadata, uint16_t count);
};

struct lib_int_funcs {
    /* AICS Client */
    int (*_aics_client_state_get)(struct bt_aics *inst);
    int (*_aics_client_gain_setting_get)(struct bt_aics *inst);
    int (*_aics_client_type_get)(struct bt_aics *inst);
    int (*_aics_client_status_get)(struct bt_aics *inst);
    int (*_aics_client_unmute)(struct bt_aics *inst);
    int (*_aics_client_mute)(struct bt_aics *inst);
    int (*_aics_client_manual_gain_set)(struct bt_aics *inst);
    int (*_aics_client_automatic_gain_set)(struct bt_aics *inst);
    int (*_aics_client_gain_set)(struct bt_aics *inst, int8_t gain);
    int (*_aics_client_description_get)(struct bt_aics *inst);
    int (*_aics_client_description_set)(struct bt_aics *inst, const char *description);

    /* BAP Broadcast Sink */
    bool (*_bap_broadcast_sink_has_ep)(const struct bt_bap_ep *ep);

    /* BAP Broadcast Source */
    bool (*_bap_broadcast_source_has_ep)(const struct bt_bap_ep *ep);

    /* BAP Unicast Client */
    bool (*_bap_unicast_client_has_ep)(const struct bt_bap_ep *ep);
    struct bt_conn *(*_bap_unicast_client_ep_get_conn)(const struct bt_bap_ep *ep);
    int (*_bap_unicast_client_register_cb)(struct bt_bap_unicast_client_cb *cb);
    int (*_bap_unicast_client_config)(struct bt_bap_stream *stream,
                                      const struct bt_audio_codec_cfg *codec_cfg);
    int (*_bap_unicast_client_metadata)(struct bt_bap_stream *stream,
                                        const uint8_t meta[], size_t meta_len);
    int (*_bap_unicast_client_connect)(struct bt_bap_stream *stream);
    int (*_bap_unicast_client_start)(struct bt_bap_stream *stream);
    int (*_bap_unicast_client_disable)(struct bt_bap_stream *stream);
    int (*_bap_unicast_client_release)(struct bt_bap_stream *stream);

    /* BAP Unicast Server */
    bool (*_bap_unicast_server_has_ep)(const struct bt_bap_ep *ep);
    struct bt_conn *(*_bap_unicast_server_ep_get_conn)(const struct bt_bap_ep *ep);
    int (*_bap_unicast_server_reconfig)(struct bt_bap_stream *stream,
                                        const struct bt_audio_codec_cfg *codec_cfg);
    int (*_bap_unicast_server_start)(struct bt_bap_stream *stream);
    int (*_bap_unicast_server_metadata)(struct bt_bap_stream *stream,
                                        const uint8_t meta[], size_t meta_len);
    int (*_bap_unicast_server_disable)(struct bt_bap_stream *stream);
    int (*_bap_unicast_server_release)(struct bt_bap_stream *stream);

    /* CAP Acceptor */
    bool (*_cap_acceptor_ccids_exist)(const struct bt_conn *conn, const uint8_t ccids[],
                                      uint8_t ccid_cnt);

    /* CAP Common */
    void (*_cap_common_discover_csis)(struct bt_conn *conn, struct bt_cap_common_client *client);

    /* CAP Commander */
    void (*_cap_commander_proc_complete)(struct bt_cap_common_proc *active_proc);

    /* CAP Initiator */
    void (*_cap_common_set_subproc)(enum bt_cap_common_subproc_type subproc_type);
    bool (*_cap_common_proc_is_type)(enum bt_cap_common_proc_type proc_type);
    bool (*_cap_common_subproc_is_type)(enum bt_cap_common_subproc_type subproc_type);
    void (*_cap_initiator_cp_cb)(struct bt_cap_stream *cap_stream,
                                 enum bt_bap_ascs_rsp_code rsp_code,
                                 enum bt_bap_ascs_reason reason);
    void (*_cap_initiator_codec_configured)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_qos_configured)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_enabled)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_connected)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_started)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_metadata_updated)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_disabled)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_stopped)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_released)(struct bt_cap_stream *cap_stream);
    void (*_cap_initiator_unicast_audio_proc_complete)(struct bt_cap_common_proc *active_proc);

    /* CAP Handover */
    bool (*_cap_common_active_proc_is_handover)(void);
    bool (*_cap_handover_is_handover_broadcast_source)(const struct bt_cap_broadcast_source *cap_broadcast_source);
    void (*_cap_handover_complete)(struct bt_cap_common_proc *active_proc);
    void (*_cap_handover_unicast_proc_complete)(struct bt_cap_common_proc *active_proc);
    void (*_cap_handover_broadcast_source_stopped)(uint8_t reason);
    void (*_cap_handover_unicast_to_broadcast_reception_start)(void);
    int (*_cap_handover_broadcast_reception_stopped)(struct bt_cap_common_proc *active_proc);
    void (*_cap_handover_commander_proc_complete)(struct bt_cap_common_proc *active_proc);

    /* CAP Stream */
    void (*_cap_stream_codec_configured_cb)(struct bt_bap_stream *bap_stream,
                                            const struct bt_bap_qos_cfg_pref *pref);
    void (*_cap_stream_qos_configured_cb)(struct bt_bap_stream *bap_stream);
    void (*_cap_stream_enabled_cb)(struct bt_bap_stream *bap_stream);
    void (*_cap_stream_metadata_updated_cb)(struct bt_bap_stream *bap_stream);
    void (*_cap_stream_disabled_cb)(struct bt_bap_stream *bap_stream);
    void (*_cap_stream_released_cb)(struct bt_bap_stream *bap_stream);
    void (*_cap_stream_recv_cb)(struct bt_bap_stream *bap_stream,
                                const struct bt_iso_recv_info *info,
                                const uint8_t *data, uint16_t len);
    void (*_cap_stream_sent_cb)(struct bt_bap_stream *bap_stream, void *user_data);

    /* MCC */
    void (*_mcc_on_obj_selected)(struct bt_ots_client *otc_inst,
                                 struct bt_conn *conn, int result);
    int (*_mcc_on_icon_content)(struct bt_ots_client *otc_inst,
                                struct bt_conn *conn,
                                uint32_t offset, uint32_t len,
                                uint8_t *data_p, bool is_complete);
    void (*_mcc_on_object_metadata)(struct bt_ots_client *otc_inst,
                                    struct bt_conn *conn, int err,
                                    uint8_t metadata_read);
    void (*_mcc_on_obj_data_read_failed)(struct bt_ots_client *otc_inst,
                                         struct bt_conn *conn, int err);

    /* MCS */
    struct bt_ots *(*_mcs_get_ots)(void);
    void (*_mcs_media_proxy_sctrl_current_track_id_cb)(uint64_t id);
    void (*_mcs_media_proxy_sctrl_next_track_id_cb)(uint64_t id);
    void (*_mcs_media_proxy_sctrl_parent_group_id_cb)(uint64_t id);
    void (*_mcs_media_proxy_sctrl_current_group_id_cb)(uint64_t id);
    void (*_mcs_media_proxy_sctrl_search_cb)(uint8_t result_code);
    void (*_mcs_media_proxy_sctrl_search_results_id_cb)(uint64_t id);
    void (*_mcs_svc_inc_init)(void);
    void (*_mcs_chrc_icon_obj_id_init)(void);
    void (*_mcs_chrc_track_segments_obj_id_init)(void);
    void (*_mcs_chrc_current_track_obj_id_init)(void);
    void (*_mcs_chrc_next_track_obj_id_init)(void);
    void (*_mcs_chrc_parent_group_obj_id_init)(void);
    void (*_mcs_chrc_current_group_obj_id_init)(void);
    void (*_mcs_chrc_search_control_point_init)(void);
    void (*_mcs_chrc_search_results_obj_id_init)(void);
    int (*_mcs_svc_init)(struct bt_ots_cb *ots_cbs);

    /* Media Proxy */
    uint64_t (*_media_proxy_sctrl_get_icon_id)(void);
    uint64_t (*_media_proxy_sctrl_get_track_segments_id)(void);
    uint64_t (*_media_proxy_sctrl_get_current_track_id)(void);
    void (*_media_proxy_sctrl_set_current_track_id)(uint64_t id);
    uint64_t (*_media_proxy_sctrl_get_next_track_id)(void);
    void (*_media_proxy_sctrl_set_next_track_id)(uint64_t id);
    uint64_t (*_media_proxy_sctrl_get_parent_group_id)(void);
    uint64_t (*_media_proxy_sctrl_get_current_group_id)(void);
    void (*_media_proxy_sctrl_set_current_group_id)(uint64_t id);
    void (*_media_proxy_sctrl_send_search)(const struct mpl_search *search);
    uint64_t (*_media_proxy_sctrl_get_search_results_id)(void);
    void (*_media_proxy_pl_current_track_id_cb)(uint64_t id);
    void (*_media_proxy_pl_next_track_id_cb)(uint64_t id);
    void (*_media_proxy_pl_parent_group_id_cb)(uint64_t id);
    void (*_media_proxy_pl_current_group_id_cb)(uint64_t id);
    void (*_media_proxy_pl_search_cb)(uint8_t result_code);
    void (*_media_proxy_pl_search_results_id_cb)(uint64_t id);

    /* MICP Microphone Controller */
    uint8_t (*_micp_discover_include_func)(struct bt_conn *conn,
                                           const struct bt_gatt_attr *attr,
                                           struct bt_gatt_discover_params *params);
    int (*_micp_mic_ctrl_aics_init)(void);

    /* MICP Microphone Device */
    int (*_micp_mic_dev_prepare_aics_inst)(struct bt_micp_mic_dev_register_param *param);

    /* MPL */
    uint64_t (*_mpl_get_icon_id)(void);
    uint64_t (*_mpl_get_track_segments_id)(void);
    uint64_t (*_mpl_get_current_track_id)(void);
    void (*_mpl_set_current_track_id)(uint64_t id);
    uint64_t (*_mpl_get_next_track_id)(void);
    void (*_mpl_set_next_track_id)(uint64_t id);
    uint64_t (*_mpl_get_parent_group_id)(void);
    uint64_t (*_mpl_get_current_group_id)(void);
    void (*_mpl_set_current_group_id)(uint64_t id);
    void (*_mpl_send_search)(const struct mpl_search *search);
    uint64_t (*_mpl_get_search_results_id)(void);

    /* TBS Client */
    int (*_tbs_client_primary_discover_tbs)(struct bt_conn *conn);
    int (*_tbs_client_primary_discover_gtbs)(struct bt_conn *conn);
    struct bt_tbs_instance *(*_tbs_client_get_by_ccid)(const struct bt_conn *conn,
                                                       uint8_t ccid);
    struct bt_tbs_instance *(*_tbs_client_get_by_index)(const struct bt_conn *conn,
                                                        uint8_t index);

    /* VCP Volume Controller */
    int (*_vcp_vol_ctlr_aics_init)(void);
    int (*_vcp_vol_ctlr_vocs_init)(void);
    uint8_t (*_vcp_vol_ctlr_vcs_discover_include_func)(struct bt_conn *conn,
                                                       const struct bt_gatt_attr *attr,
                                                       struct bt_gatt_discover_params *params);
    int (*_vcp_vol_ctlr_included_get)(struct bt_vcp_vol_ctlr *vol_ctlr,
                                      struct bt_vcp_included *included);

    /* VCP Volume Renderer */
    int (*_vcp_vol_rend_prepare_aics_inst)(struct bt_vcp_vol_rend_register_param *param);
    int (*_vcp_vol_rend_prepare_vocs_inst)(struct bt_vcp_vol_rend_register_param *param);

    /* VOCS Client */
    int (*_vocs_client_state_get)(struct bt_vocs_client *inst);
    int (*_vocs_client_state_set)(struct bt_vocs_client *inst, int16_t offset);
    int (*_vocs_client_location_get)(struct bt_vocs_client *inst);
    int (*_vocs_client_location_set)(struct bt_vocs_client *inst, uint32_t location);
    int (*_vocs_client_description_get)(struct bt_vocs_client *inst);
    int (*_vocs_client_description_set)(struct bt_vocs_client *inst, const char *description);
};

#ifdef __cplusplus
}
#endif
