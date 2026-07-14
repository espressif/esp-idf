Bluedroid Host Support for Dual Local Identities
================================================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

When a single peer phone connects to an ESP32 peripheral through two different **local identities** (for example, a Public address and a fixed Static Random address from two extended advertising sets), the default Bluedroid Host treats both links as the same peer. Bonds, LTK, and NVS sections can overwrite each other.

Enable :ref:`BT_BLE_PERIPH_PSEUDO_ADDR_BOND <CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND>` to derive a Host-internal **pseudo address** ``f(local_identity, peer)`` per link. The application sees two different ``remote_bda`` values for the same phone, while SMP and the controller still use the real peer identity on air.

Example
-------

See :example:`ble50_dual_identity_server <bluetooth/bluedroid/ble_50/ble50_dual_identity_server>` for a Bluetooth LE 5.0 peripheral that advertises two identities concurrently, pairs with both, and keeps **isolated bonds per (local, peer) pair**.

Application Notes
-----------------

- Use **conn_id** as the link key in GATTS calls; do not use ``remote_bda`` to tell links apart.
- ``remote_bda`` in GAP/GATTS events is the **pseudo address** when this feature is enabled.
- Call ``esp_ble_gap_get_conn_identity()`` while connected to recover the real peer and local identity.
- Use ``esp_ble_gap_remove_bond_for_identity()`` to delete one identity's bond without affecting the other.
- For controller operations (whitelist, directed advertising), use the **real peer** address, never the pseudo.
