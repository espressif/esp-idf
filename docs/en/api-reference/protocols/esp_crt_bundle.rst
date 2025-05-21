ESP x509 Certificate Bundle
===========================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The ESP x509 Certificate Bundle API provides an easy way to include a bundle of custom x509 root certificates for TLS server verification.

.. note::

    The bundle is currently not available when using WolfSSL.

The bundle comes with the complete list of root certificates from Mozilla's NSS root certificate store. Using the gen_crt_bundle.py python utility, the certificates' subject name and public key are stored in a file and embedded in the {IDF_TARGET_NAME} binary.

When generating the bundle you may choose between:

 * The full root certificate bundle from Mozilla, containing more than 130 certificates. The current bundle was updated Tue May 20 03:12:02 2025 GMT.
 * A pre-selected filter list of the name of the most commonly used root certificates, reducing the amount of certificates to around 42 while still having around 93% absolute usage coverage and 99% market share coverage according to SSL certificate authorities statistics.

In addition, it is possible to specify a path to a certificate file or a directory containing certificates which then will be added to the generated bundle.

.. note::

    Trusting all root certificates means the list will have to be updated if any of the certificates are retracted. This includes removing them from ``cacrt_all.pem``.

Configuration
-------------

Most configuration is done through menuconfig. CMake generates the bundle according to the configuration and embed it.

 * :ref:`CONFIG_MBEDTLS_CERTIFICATE_BUNDLE`: automatically build and attach the bundle.
 * :ref:`CONFIG_MBEDTLS_DEFAULT_CERTIFICATE_BUNDLE`: decide which certificates to include from the complete root certificate list.
 * :ref:`CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE_PATH`: specify the path of any additional certificates to embed in the bundle.

To enable the bundle when using ESP-TLS simply pass the function pointer to the bundle attach function:

.. code-block:: c

    esp_tls_cfg_t cfg = {
         .crt_bundle_attach = esp_crt_bundle_attach,
    };

This is done to avoid embedding the certificate bundle unless activated by the user.

If using mbedTLS directly then the bundle may be activated by directly calling the attach function during the setup process:

.. code-block:: c

    mbedtls_ssl_config conf;
    mbedtls_ssl_config_init(&conf);

    esp_crt_bundle_attach(&conf);


.. _updating_bundle:

Generating the List of Root Certificates
----------------------------------------

The list of root certificates comes from Mozilla's NSS root certificate store, which can be found `here <https://wiki.mozilla.org/CA/Included_Certificates>`_

The list can be downloaded and created by running the script ``mk-ca-bundle.pl`` that is distributed as a part of `curl <https://github.com/curl/curl>`_.

Another alternative would be to download the finished list directly from the curl website: `CA certificates extracted from Mozilla <https://curl.se/docs/caextract.html>`_

The common certificates bundle were made by selecting the authorities with a market share of more than 1% from w3tech's `SSL Survey <https://w3techs.com/technologies/overview/ssl_certificate>`_.

These authorities were then used to pick the names of the certificates for the filter list, ``cmn_crt_authorities.csv``, from `this list <https://ccadb-public.secure.force.com/mozilla/IncludedCACertificateReportPEMCSV>`_ provided by Mozilla.


Updating the Certificate Bundle
-------------------------------

The bundle is embedded into the app and can be updated along with the app by an OTA update. If you want to include a more up-to-date bundle than the bundle currently included in ESP-IDF, then the certificate list can be downloaded from Mozilla as described in :ref:`updating_bundle`.


Periodic Sync
-------------

The bundle is kept updated by periodic sync with the Mozilla's NSS root certificate store. The deprecated certs from the upstream bundle are added to deprecated list (for compatibility reasons) in ESP-IDF minor or patch release. If required, the deprecated certs can be added to the default bundle by enabling :ref:`CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEPRECATED_LIST`. The deprecated certs shall be removed (reset) on the next major ESP-IDF release.

Application Examples
--------------------

- :example:`protocols/https_x509_bundle` demonstrates how to use ESP-TLS to establish a secure socket connection using the certificate bundle with two custom certificates added for verification.

- :example:`protocols/https_request` demonstrates an HTTPS example that uses ESP-TLS and the default bundle.

- :example:`protocols/https_mbedtls` demonstrates an HTTPS example that uses Mbed TLS and the default bundle.

API Reference
-------------

.. include-build-file:: inc/esp_crt_bundle.inc
