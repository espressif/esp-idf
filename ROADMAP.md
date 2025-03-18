# ESP-IDF Project Roadmap 2025 

* [中文版](./ROADMAP_CN.md)

This document outlines the goals of ESP-IDF project and is shared for the convenience of our customers. It is important to clarify that this document is not a binding commitment to our customers. Instead, its primary purpose is to offer a clear roadmap and direction for the project's development. By openly sharing this information, we aim to enhance our customers' understanding, promote transparency and ensure alignment with the overarching objectives of the ESP-IDF project.

## Project Overview

### Project Goals

In both minor and major releases, we integrate new chip support to enhance our product range. By expanding the chip matrix, we broaden the scope of our offerings, catering to a wider audience with diverse needs. This proactive approach ensures that our products remain at the forefront of technological advancements, consistently meeting and exceeding customer expectations.

Furthermore, we prioritize bugfix releases for active branches, focusing on improving the stability and performance of products already in production. By addressing bugs promptly, we aim to enhance the overall user experience and provide tangible benefits to customers relying on our solutions. This proactive maintenance strategy reflects our commitment to delivering reliable, high-quality products to our valued customer base.

Below are the main objectives that ESP-IDF project/teams would like to implement in 2025.

-   New Chip Support

    -   Add support for ESP32-C5
    -   Add support for ESP32-C61

-   More Minor/Major Releases

    -   Release IDF v5.5 in the middle of 2025
    -   Release IDF v6.0 at the end of 2025

-   More Bugfix Releases

    -   Release v5.0.8 and v5.0.9 before ESP-IDF v5.0 goes End of Life in May 2025
    -   Release v5.1.6 and v5.1.7 before ESP-IDF v5.1 goes End of Life in December 2025
    -   Do more bugfix releases for IDF v5.2 and IDF v5.3 before release/5.2 and release/5.3 enter maintenance period
    -   Do more bug fixes releases for release/5.4 and release/5.5, and push the two releases to be more stable and production-ready

-   Major Changes

    -   We plan to upgrade MbedTLS to v4.x series in IDF v6.0. In addition, we will also be migrating to newer PSA crypto API as part of this upgrade. Please note that this may involve some breaking changes on the application side for the crypto API usage.

Please note that support status of previous silicones could be found on [ESP-IDF Release and SoC Compatibility](https://github.com/espressif/esp-idf#esp-idf-release-and-soc-compatibility).

### Roadmap Details

The ESP-IDF project prioritizes consistent maintenance and updates to ensure our customers remain at the forefront of technological advancements. Our commitment to ongoing development ensures that customers continuously benefit from the latest innovations in the field.

Moreover, we are dedicated to empowering our customers to leverage newly implemented features and enhanced functionalities through iterative improvements. Our steadfast commitment to pushing boundaries ensures that clients not only keep pace with evolving technology but also extract optimal value from the cutting-edge capabilities of our products.

Below are the main roadmap details for functional areas inside ESP-IDF.

-   New Chip Support

    -   Add the initial support for the mass production version of ESP32-C5 in ESP-IDF v5.5, refer to [ESP32-C5 Support Status](https://github.com/espressif/esp-idf/issues/14021)
    -   Add the initial support for the mass production version of ESP32-C61 in ESP-IDF v5.5, refer to [ESP32-C61 Support Status](https://developer.espressif.com/pages/chip-support-status/esp32c61/#esp-idf)

-   Bugfix releases

    -   Do bugfix releases v5.0.8 and v5.0.9 and stop maintaining ESP-IDF v5.0 in May 2025
    -   Do bugfix releases v5.1.6 and v5.1.7 and stop maintaining ESP-IDF v5.1 in December 2025
    -   Release bugfix IDF v5.2.4, IDF v5.2.5 and IDF v5.2.6 in 2025, and push release/5.2 to maintenance period from February 2025
    -   Release bugfix IDF v5.3.3 and IDF v5.3.4 in 2025, and push release/5.3 to maintenance period from July 2025
    -   Do more bug fixes releases for release/5.4 (IDF v5.4.1, IDF v5.4.2, IDF v5.4.3) and release/5.5 (IDF v5.5.1, IDF v5.5.2), and push releases to be more stable and more production-ready

## ESP-IDF Planning information

For the full list of ESP-IDF releases, please visit https://github.com/espressif/esp-idf/releases

All the information provided here is subject to change without notice, due to business reasons and other factors.

### ESP-IDF Major Releases

```mermaid
timeline

        title ESP-IDF Major Releases
        section 2025 Q1 <br> Major Release Planning
          No version planned : N/A             
        section 2025 Q2 <br> Major Release Planning
          No version planned : N/A   
        section 2025 Q3 <br> Major Release Planning
          No version planned : N/A    
        section 2025 Q4 <br> Major Release Planning
          v6.0-beta1 : 2025/11/13
          v6.0-beta2 : 2025/12/05
          v6.0-RC1 : 2026/01/14   
          v6.0-RC2 : 2026/02/06  
          v6.0 : 2026/02/13   
```
  
### ESP-IDF Minor Releases

```mermaid
timeline

        title ESP-IDF Minor Releases
        section 2025 Q1 <br> Minor Release Planning
          No version planned : N/A               
        section 2025 Q2 <br> Minor Release Planning
          v5.5-beta1 : 2025/05/14
          v5.5-beta2 : 2025/06/04    
        section 2025 Q3 <br> Minor Release Planning
          v5.5-RC1 : 2025/07/07  
          v5.5-RC2 : 2025/07/28  
          v5.5 : 2025/08/04       
        section 2025 Q4 <br> Minor Release Planning
          No version planned : N/A    
```

### ESP-IDF Bugfix Releases

```mermaid
timeline

        title ESP-IDF Bugfix Releases
        section 2025 Q1 <br> Bugfix Release Planning
          v5.0.8 : 2025/01/14
          v5.1.6 : 2025/02/18
          v5.2.4 : 2025/02/23
          v5.2.5 : 2025/02/28     
          v5.4.1 : 2025/03/27
          v5.3.3 : 2025/04/04                
        section 2025 Q2 <br> Bugfix Release Planning
          v5.0.9 : 2025/05/16
          v5.4.2 : 2025/06/30 
        section 2025 Q3 <br> Bugfix Release Planning
          v5.3.4 : 2025/08/03
          v5.2.6 : 2025/09/04
          v5.5.1 : 2025/09/11   
          v5.4.3 : 2025/10/08
        section 2025 Q4 <br> Bugfix Release Planning
          v5.5.2 : 2025/11/12
          v5.1.7 : 2026/01/06   
```