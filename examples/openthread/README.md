# OpenThread Examples

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.

## Overview

In this folder, it contains following OpenThread examples:

* [ot_cli](ot_cli) is an OpenThread Command Line example, in addition to the features listed in [OpenThread CLI](https://github.com/openthread/openthread/blob/master/src/cli/README.md), it supports some additional features such as TCP, UDP and Iperf over lwIP. It runs on an 802.15.4 SoC like ESP32-H2.

* [ot_rcp](ot_rcp) is an [OpenThread RCP](https://openthread.io/platforms/co-processor) example. It runs on an 802.15.4 SoC like ESP32-H2, to extend 802.15.4 radio.

* [ot_br](ot_br) is an [OpenThread Border Router](https://openthread.io/guides/border-router) example. It runs on a Wi-Fi SoC such as ESP32, ESP32-C3 and ESP32-S3. It needs an 802.15.4 SoC like ESP32-H2 running [ot_rcp](ot_rcp) example to provide 802.15.4 radio.

* [ot_sleepy_device](ot_sleepy_device) is an OpenThread sleepy device example, it supports 802.15.4 radio light sleep. It runs on an 802.15.4 SoC.
