Migrate Bluetooth to ESP-IDF 5.0
================================

Jump to the API Reference

* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/index.html


Espressif BT API Changes
================================

We changes the HFP API names and parameters that may make the function confused. The parameter change and API name change is listed separately.

Main Change
------------

    - The prefix of `esp_bt_hf_xxx` is not so clear to implicit `HFP AG functional mode`, so change it into `esp_hf_ag_xxx`.

    - Separate audio connection and service level connection by changing the API name. The setting up of service level connection (SLC) means that the ACL channel of Bluetooth you can send or get AT command.

API Name & Parameter Change
----------------------------

    - ``esp_bt_hf_init(esp_bd_addr_t remote_addr)`` changes into ``esp_hf_ag_init(void)``.

    - ``esp_bt_hf_deinit(esp_bd_addr_t remote_addr)`` chanegs into ``esp_hf_ag_deinit(void)``

Along with this change, the `bt_bdaddr_t init` and  `bt_bdaddr_t deinit` has been removed from `union btc_arg_t`.


Parameter Change Only
-----------------------

    - Reserved.

API Name Change Only
-----------------------

HFP AG APIs has all been renamed.

    - ``esp_bt_hf_register_callback`` is now changed into ``esp_hf_ag_register_callback``.

    - ``esp_bt_hf_connect`` changes into ``esp_hf_ag_slc_connect``
    
    - ``esp_bt_hf_disconnect`` changes into ``esp_hf_ag_slc_disconnect``

    - ``esp_bt_hf_connect_audio`` changes into ``esp_hf_ag_audio_connect``

    - ``esp_bt_hf_disconnect_audio`` changes into ``esp_hf_ag_audio_disconnect``

    - ``esp_bt_hf_vra`` changes into ``esp_hf_ag_vra_control``

    - ``esp_bt_hf_volume_control`` changes into ``esp_hf_ag_volume_control``
    
    - ``esp_hf_unat_response`` changes into ``esp_hf_ag_unknown_at_send``

    - ``esp_bt_hf_cmee_response`` changes into ``esp_hf_ag_cmee_send``

    - ``esp_bt_hf_indchange_notification`` changes into ``esp_hf_ag_devices_status_indchange``

    - ``esp_bt_hf_cind_response`` changes into ``esp_hf_ag_cind_response``

    - ``esp_bt_hf_cops_response`` changes into ``esp_hf_ag_cops_response``

    - ``esp_bt_hf_clcc_response`` changes into ``esp_hf_ag_clcc_response``

    - ``esp_bt_hf_cnum_response`` changes into ``esp_hf_ag_cnum_response``

    - ``esp_bt_hf_bsir`` changes into ``esp_hf_ag_bsir``

    - ``esp_bt_hf_answer_call`` changes into ``esp_hf_ag_answer_call``

    - ``esp_bt_hf_reject_call`` changes into ``esp_hf_ag_reject_call``

    - ``esp_bt_hf_out_call`` changes into ``esp_hf_ag_out_call``

    - ``esp_bt_hf_end_call`` changes into ``esp_hf_ag_end_call``

    - ``esp_bt_hf_register_data_callback`` changes into ``esp_hf_ag_register_data_callback``

    - ``esp_hf_outgoing_data_ready`` changes into ``esp_hf_ag_outgoing_data_ready``