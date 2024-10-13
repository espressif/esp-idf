# ESP-IDF Project Roadmap 2024

This document outlines the goals of ESP-IDF project and is shared for the convenience of our customers. It is important to clarify that this document is not a binding commitment to our customers. Instead, its primary purpose is to offer a clear roadmap and direction for the project's development. By openly sharing this information, we aim to enhance our customers' understanding, promote transparency and ensure alignment with the overarching objectives of the ESP-IDF project.

## Project Overview

### Project Goals

In both minor and major releases, we integrate new chip support to enhance our product range. By expanding the chip matrix, we broaden the scope of our offerings, catering to a wider audience with diverse needs. This proactive approach ensures that our products remain at the forefront of technological advancements, consistently meeting and exceeding customer expectations.

Furthermore, we prioritize bugfix releases for active branches, focusing on improving the stability and performance of products already in production. By addressing bugs promptly, we aim to enhance the overall user experience and provide tangible benefits to customers relying on our solutions. This proactive maintenance strategy reflects our commitment to delivering reliable, high-quality products to our valued customer base.

Below are the main objectives that ESP-IDF project/teams would like to implement in 2024.

-   New Chip Support

    -   Add support for ESP32-P4
    -   Add support for ESP32-C5
    -   Add support for ESP32-C61

-   More Minor Releases

    -   Release IDF v5.3 in 2024
    -   Release IDF v5.4 at the start of 2025

-   More Bugfix Releases

    -   Release v4.4.8 for IDF v4.4 before ESP-IDF v4.4 goes End of Life in July 2024
    -   Do more bugfix releases for IDF v5.1 before release/5.1 enters maintenance period in June 2024
    -   Do more bug fixes releases for release/5.2 and release/5.3, and push the two releases to be more stable and production-ready

-   Updates of Libraries

    -   Update GDB to 14.2
    -   Update LLVM to 18.1.2
    -   Update MbedTLS to 3.6 (LTS)
    -   Update LWIP to 2.2.0
    -   Change minimal Python requirement to 3.9

Please note that support status of previous silicones could be found on [ESP-IDF Release and SoC Compatibility](https://github.com/espressif/esp-idf#esp-idf-release-and-soc-compatibility).

### Roadmap Details

The ESP-IDF project prioritizes consistent maintenance and updates to ensure our customers remain at the forefront of technological advancements. Our commitment to ongoing development ensures that customers continuously benefit from the latest innovations in the field.

Moreover, we are dedicated to empowering our customers to leverage newly implemented features and enhanced functionalities through iterative improvements. Our steadfast commitment to pushing boundaries ensures that clients not only keep pace with evolving technology but also extract optimal value from the cutting-edge capabilities of our products.

Below are the main roadmap details for functional areas inside ESP-IDF.

-   New Chip Support

    -   Add full support for ESP32-P4 in ESP-IDF v5.3, refer to [ESP32-P4 Support Status](https://github.com/espressif/esp-idf/issues/12996)
    -   Add preview support for ESP32-C5 in ESP-IDF v5.3 and full support for ESP32-C5 in ESP-IDF v5.4, refer to [ESP32-C5 Support Status](https://github.com/espressif/esp-idf/issues/14021)
    -   Add preview support for the early samples of ESP32-C61 in ESP-IDF v5.4 and full support for mass production version in ESP-IDF v5.4.x. Refer to [ESP32-C61 Support Status](https://developer.espressif.com/pages/chip-support-status/esp32c61/#esp-idf)

-   Bugfix releases

    -   Do bugfix release IDF v4.4.8 and stop maintaining ESP-IDF v4.4 in July 2024
    -   Release bugfix IDF v5.0.6 and IDF v5.0.7 in 2024 (maintenance period)
    -   Release bugfix IDF v5.1.3 and IDF v5.1.4 in H1 of 2024, and release IDF v5.1.5 in H2 of 2024
    -   Push release/5.1 to maintenance period from June 2024
    -   Do more bug fixes releases for release/5.2 (IDF v5.2.1, IDF v5.2.2, IDF v5.2.3) and release/5.3 (IDF v5.3.1, IDF v5.3.2), and push releases to be more stable and more production-ready

## ESP-IDF Planning information

For the full list of ESP-IDF releases, please visit https://github.com/espressif/esp-idf/releases

### ESP-IDF Major Releases

No Major Releases planned

### ESP-IDF Minor Releases

#### Quarter One

-   v5.2-RC1, estimate release date: 2024/02/02
-   v5.2 final release, estimate release date: 2024/02/08

#### Quarter Two

-   v5.3-beta1, estimate release date:: 2024/05/13
-   v5.3-beta2, estimate release date:: 2024/05/31
-   v5.3-RC1, estimate release date:: 2024/07/08

#### Quarter Three

-   v5.3-RC2, estimate release date:: 2024/07/26
-   v5.3 final release, estimate release date:: 2024/08/02

#### Quarter Four

-   v5.4-beta1, estimate release date:: 2024/11/08
-   v5.4-beta2, estimate release date:: 2024/11/29
-   v5.4-RC1, estimate release date:: 2025/01/09
-   v5.4-RC2, estimate release date:: 2025/01/29
-   v5.4 final release, estimate release date: 2025/02/05

### ESP-IDF Bugfix Releases

#### Quarter One

-   v5.1.3, estimate release date: 2024/02/08
-   v5.0.6, estimate release date: 2024/02/18
-   v4.4.7, estimate release date:: 2024/03/19
-   v5.2.1, estimate release date:: 2024/03/31

#### Quarter Two

-   v5.1.4, estimate release date:: 2024/05/06
-   v5.2.2, estimate release date:: 2024/06/17
-   v4.4.8, estimate release date:: 2024/07/19

#### Quarter Three

-   v5.0.7, estimate release date: 2024/08/22
-   v5.3.1, estimate release date: 2024/09/16
-   v5.2.3, estimate release date: 2024/10/10

#### Quarter Four

-   v5.1.5, estimate release date: 2024/11/04  (Maintenance period since June 2024)
-   v5.3.2, estimate release date: 2024/12/31
-   v5.2.4, estimate release date: 2025/02/20 (Service period ends Feb. 2025)
