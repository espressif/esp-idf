Thread Certification
=====================

:link_to_translation:`zh_CN:[中文]`

Espressif’s Thread solutions, including all Thread-enabled ESP32 series SoCs and ESP Thread Border Router solutions, are certified by the Thread Group as Thread Certified Components.

Each certified component is based on a specific ESP-IDF release tag and can serve as a foundation for product development. If a product is developed using the same ESP-IDF release tag, without any modifications to the Thread-related source code or default configuration, it may be eligible for Certification by Inheritance.

If any modifications are made to the Thread-related source code or default configuration, please consult the Thread Group to determine whether Certification by Inheritance remains applicable or whether testing by an authorized test laboratory (ATL) is required.

When the applicable inheritance requirements are satisfied, a product may apply to the Thread Group for Certification by Inheritance and obtain an independent Thread certificate.

Certified Component List
------------------------

Use the following tables to select an Espressif certified component that matches your product. In an inheritance application, the cited Espressif component is the parent product and your product is the child product.

CID stands for Certification ID.

Thread Devices
~~~~~~~~~~~~~~

.. list-table::
    :width: 100%
    :widths: 20 20 20 20 20
    :header-rows: 1

    * - Component
      - Chip Models
      - CID
      - ESP-IDF Version
      - Thread Version
    * - ESP32-H2
      - ESP32-H2
      - 14A204
      - v5.3.2
      - v1.4.0
    * - ESP32-C6
      - ESP32-C6
      - 14A205
      - v5.1.5
      - v1.4.0
    * - ESP32-C5
      - ESP32-C5
      - 14A240
      - v5.3.3
      - v1.4.0
    * - |ESP32 C-Series|
      - ESP32-C5, ESP32-C6
      - 14A416
      - v5.5.4
      - v1.4.0
    * - |ESP32 H-Series|
      - ESP32-H2, ESP32-H4, ESP32-H21
      - 14A446
      - v5.5.4
      - v1.4.0


Thread Border Router
~~~~~~~~~~~~~~~~~~~~

.. list-table::
    :width: 100%
    :widths: 20 20 20 20 20 20
    :header-rows: 1

    * - Component
      - Host Models
      - RCP Models
      - CID
      - ESP-IDF Version
      - Thread Version
    * - `Espressif TBR <https://documentation.espressif.com/Espressif%20Thread%20Border%20Router%20Thread%20V1.4%20Interoperability%20Certification.pdf>`__
      - ESP32-S3
      - ESP32-H2
      - 13A080
      - v5.3.3
      - v1.4.0
    * - |ESP32 C-Series TBR Assembly|
      - ESP32-C2, ESP32-C3, ESP32-C5, ESP32-C6, ESP32-C61
      - ESP32-H2, ESP32-H4, ESP32-H21, ESP32-C5, ESP32-C6, ESP32-S31
      - 14A468
      - v5.5.4
      - v1.4.0
    * - |ESP32 S-Series TBR Assembly|
      - ESP32-S3, ESP32-S31
      - ESP32-H2, ESP32-H4, ESP32-H21, ESP32-C5, ESP32-C6, ESP32-S31
      - 14A513
      - v5.5.4
      - v1.4.0

These certificates cover an arrangement in which an ESP32 Host and an ESP32 RCP (Radio Co-Processor) work together to provide a Thread Border Router solution. Confirm that the chips used in your product are covered by one of the chips listed in the corresponding certificate. If none of the certificates above covers the chips used in your product, contact Espressif.

Conditions for Certification by Inheritance
--------------------------------------------

When you develop an end product based on a certified component, the application generally falls under the component certification inheritance scenario (Type III). In this scenario, the parent product is an Espressif certified component listed above and its corresponding certificate is the parent certificate.

Before applying, confirm the following conditions:

- Your company holds a valid Thread Group membership that supports the applicable certification activity.
- Your product uses the chip and Thread protocol stack corresponding to the parent component, and the certified Thread implementation remains unchanged as required for inheritance.

Application Process
-------------------

1. **Become a Thread Group member**

   If your company has not joined Thread Group, apply through the |join|. Select a membership level that supports your certification scenario. Existing members should confirm their membership status and certification permissions. Refer to |membership| for current membership benefits.

2. **Confirm the parent certificate and software configuration**

   Compare your product type and intended Thread device role with the certified components above. For a Thread device, verify the chip model and required Thread functions. For a Border Router, also verify the Host and RCP combination. Select the appropriate parent certificate CID for the inheritance application.

3. **Prepare the application materials**

   Sign in to the |portal| to obtain the current Certification by Inheritance application form and follow its instructions. As evidence of inheritance, provide a build log for the product firmware that shows the ESP-IDF version used.

4. **Submit the inheritance application**

   Submit the application through the certification channel in the member portal. Enter the parent certificate CID and provide the required supporting materials. Pay any applicable certification administration fee according to the Thread Group notification. Thread Group reviews the application and may request additional or corrected information. A product CID assigned after the application is accepted identifies the product, but does not mean that certification is complete.

5. **Obtain the product certificate**

   After approval, Thread Group issues a certificate for your product and provides the applicable certification mark license agreement and mark materials. Check the product name, model, hardware and software versions, and Thread features stated on the certificate. Only after certification is complete may you claim Thread certification and use the applicable certification mark in accordance with Thread Group branding and licensing requirements. You may also choose to list the certified product publicly on the Thread Group website.

FAQ
---

Does using an Espressif-certified chip automatically make my product Thread certified?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No. You must submit an application using your own product information and obtain a certificate for your product. An Espressif parent certificate CID is used as a reference in the application and does not replace product certification.

Does Certification by Inheritance require submitting a sample for testing?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An application that meets the inheritance requirements and is accepted by Thread Group does not need ATL certification testing, but it must still provide the required inheritance evidence. If the requirements are not met, confirm the testing and certification process with Thread Group.

What if my ESP-IDF version differs from the version in the certificate?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If your ESP-IDF version differs from the version corresponding to the Espressif certificate, consider the following:

- **An earlier version:** Espressif recommends upgrading ESP-IDF to the version corresponding to the certificate and then applying based on that certificate.
- **A later version:** Contact Espressif. Espressif may certify an updated component for the ESP-IDF version you use and obtain a corresponding inheritable certificate. You can then use the updated certificate when applying for Certification by Inheritance.

References
----------

- |certification|
- |portal|

.. |ESP32 C-Series| replace:: `ESP32 C-Series <https://documentation.espressif.com/ESP32-C%20series%20Thread%2014A416.0_2026_06_30.pdf>`__
.. |ESP32 H-Series| replace:: `ESP32 H-Series <https://documentation.espressif.com/ESP32-H%20series%20Thread%2014A446.0_2026_08_09.pdf>`__
.. |ESP32 C-Series TBR Assembly| replace:: `ESP32 C-Series TBR Assembly <https://documentation.espressif.com/ESP32-C%20series%20TBR%2014A468.0_2026_08_10.pdf>`__
.. |ESP32 S-Series TBR Assembly| replace:: `ESP32 S-Series TBR Assembly <https://documentation.espressif.com/esp32_s_series_host_esp32_rcp_thread_br_cert_inheritable_cid_14a513.pdf>`__
.. |certification| replace:: `Thread Group certification guide <https://threadgroup.org/What-is-Thread/Certification>`__
.. |portal| replace:: `Thread Group member portal <https://portal.threadgroup.org/>`__
.. |join| replace:: `Thread Group membership application page <https://threadgroup.org/Becomemember>`__
.. |membership| replace:: `Thread Group membership information <https://threadgroup.org/thread-group#Membershipbenefits>`__
