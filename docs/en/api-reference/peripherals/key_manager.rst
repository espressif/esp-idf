.. _key-manager:

Key Manager
===========

:link_to_translation:`zh_CN:[中文]`

The {IDF_TARGET_NAME}'s Key Manager peripheral provides hardware-assisted **key deployment and recovery** for cryptographic keys. It allows cryptographic keys to be provisioned and used without storing plaintext key material in flash, RAM, or eFuses.

The Key Manager is intended for applications that require secure handling of long-term cryptographic keys.

.. only:: esp32p4

  .. note::

        The Key Manager peripheral is only supported on ESP32-P4 chip revision >= v3.0.

.. only:: esp32c5

    .. note::

        The Key Manager peripheral is only supported on ESP32-C5 chip revision >= v1.2.

Key Manager provides the following properties:

- **Device uniqueness**

  Keys are cryptographically bound to a Hardware Unique Key (HUK) that is unique to each chip.

- **No plaintext key storage**

  Key material is never exposed to software accessible memory.

- **Flexible key lifecycle**

  Keys can be deployed, recovered, or replaced by a newer key without reprogramming the eFuses for each key.

- **Resistance to physical extraction**

  Reading flash or eFuses contents would not reveal usable key material.

Hardware Unique Key (HUK)
-------------------------

The Hardware Unique Key (HUK) is a device-specific unique key generated entirely in hardware HUK peripheral. It is generated using SRAM Physical Unclonable Function (PUF) and is reconstructed using the HUK recovery info stored in the key recovery info of a Key Manager deployed key. See **{IDF_TARGET_NAME} Technical Reference Manual** > **Chapter Key Manager** [`PDF <{IDF_TARGET_TRM_EN_URL}>`__] > **HUK Generator** for more details about the HUK peripheral.

The HUK acts as the root of trust for all keys deployed through the Key Manager.

Key Deployment and Key Recovery
-------------------------------

The Key Manager operates in two distinct phases:

- **Key deployment**

  A cryptographic key is generated or securely introduced into the chip, and it gets bound to the HUK. This step is usually performed during manufacturing, first boot up or when generating transient or persistent keys during the application runtime.

- **Key recovery**

  On subsequent boots, a Key Manager-deployed persistent key is restored using the previously generated key recovery information, without exposing the key value.

During deployment, the Key Manager generates a data structure referred to as :cpp:type:`esp_key_mgr_key_recovery_info_t`. In case of persistent keys, the applications must store this data in non-volatile storage (for example, flash) in order to recover the key on later boots.

Supported Key Types
-------------------

The Key Manager can manage keys for the following key types:

.. list::

    :SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY: - ECDSA
    :SOC_KEY_MANAGER_FE_KEY_DEPLOY: - Flash Encryption (XTS-AES)
    :SOC_KEY_MANAGER_HMAC_KEY_DEPLOY: - HMAC
    :SOC_KEY_MANAGER_DS_KEY_DEPLOY: - Digital Signature peripherals
    :SOC_KEY_MANAGER_FE_KEY_DEPLOY: - PSRAM Encryption

Each key is associated with a :cpp:type:`esp_key_mgr_key_purpose_t`, which defines how the key can be used by hardware peripherals.

Key Deployment Modes
--------------------

The Key Manager provides multiple key deployment modes to support different provisioning and security requirements.

Random Deploy Mode
^^^^^^^^^^^^^^^^^^

In this mode, the Key Manager generates a random private key internally.

- The key value is never known to the application software.
- No external key material is required.
- Intended for use cases where the key does not need to be backed up or exported.

AES Deploy Mode
^^^^^^^^^^^^^^^

In this mode, a user-specified private key is securely deployed.

- The key is encrypted before being transmitted to the chip.
- Auxiliary key material is used to protect the deployment process.
- Intended for factory provisioning scenarios where the key value must be predefined.

ECDH0 Deploy Mode
^^^^^^^^^^^^^^^^^

In this mode, a private key is negotiated using Elliptic Curve Diffie-Hellman (ECDH).

- The final private key is never transmitted.
- The deployment process can occur over an untrusted channel.
- Intended for high-security provisioning environments.

For detailed information various deployment modes, see **{IDF_TARGET_NAME} Technical Reference Manual** > **Chapter Key Manager** [`PDF <{IDF_TARGET_TRM_EN_URL}>`__] > **Section Key Manager**.

.. ECDH1 Deploy Mode
.. ~~~~~~~~~~~~~~~~~
..
.. This mode is similar to ECDH0 Deploy Mode, with additional flexibility for manufacturing workflows.
..
.. - Supports negotiated key deployment using auxiliary recovery data
.. - Allows updating deployed keys by replacing auxiliary information
.. - Intended for large-scale manufacturing with controlled trust assumptions

Typical Workflows
-----------------

First Boot or Manufacturing
^^^^^^^^^^^^^^^^^^^^^^^^^^^

A typical provisioning flow includes:

1. Generating the Hardware Unique Key (HUK)
2. Deploying required cryptographic keys using an appropriate deployment mode
3. Storing the generated ``key_recovery_info`` in non-volatile storage
4. Locking relevant security configuration eFuses, if required

This process is usually performed once per device.

Normal Boot
^^^^^^^^^^^

During a normal boot:

1. The application provides the previously generated and stored ``key_recovery_info`` of a Key Manager-deployed key.
2. The HUK is reconstructed automatically by hardware.
3. The Key Manager recovers the deployed key internally.
4. Cryptographic peripherals can use the recovered key.

Security Considerations
-----------------------

Applications using the Key Manager should consider the following:

- Protect the ``key_recovery_info`` of a Key Manager-deployed key against unauthorized modification or loss.
- Lock Key Manager's security-related eFuses after successful key deployment to prevent re-deployment of a key of the same type.
- Avoid deploying new XTS-AES keys when Flash Encryption is already enabled unless explicitly intended.

API Reference
-------------

.. include-build-file:: inc/esp_key_mgr.inc
.. include-build-file:: inc/key_mgr_types.inc

Examples
--------

See :example:`security/key_manager` for an example demonstrating key deployment using the Key Manager and using the deployed key to perform signing operations.

This example shows how to:

- Initialize the Key Manager
- Deploy keys using the AES deployment mode
- Use the PSA interface to perform signing operations using the Key Manager deployed key
