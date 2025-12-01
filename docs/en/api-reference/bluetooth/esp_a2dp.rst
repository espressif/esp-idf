Bluetooth® A2DP API
===================

Overview
--------

A2DP (Advanced Audio Distribution Profile) enables high-quality audio streaming from one device to another over Bluetooth. It is primarily used for streaming audio from source devices such as smartphones, computers, and media players to sink devices such as Bluetooth speakers, headphones, and car audio systems. Users can use the A2DP APIs to transmit or receive audio streams.

Application Examples
--------------------

Check :example:`bluetooth/bluedroid/classic_bt` folder in ESP-IDF examples, which contains the following application:

* This is a A2DP sink client demo. This demo can be discovered and connected by A2DP source device and receive the audio stream from remote device - :example:`bluetooth/bluedroid/classic_bt/a2dp_sink_stream`

API Reference
-------------

.. include-build-file:: inc/esp_a2dp_api.inc

