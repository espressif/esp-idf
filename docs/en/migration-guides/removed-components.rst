Removed or deprecated components
================================

Following components are removed from ESP-IDF and moved to `IDF Component Registry <https://components.espressif.com/>`_:

* `libsodium <https://components.espressif.com/component/espressif/libsodium>`_
* `cbor <https://components.espressif.com/component/espressif/cbor>`_
* `jsmn <https://components.espressif.com/component/espressif/jsmn>`_
* `esp_modem <https://components.espressif.com/component/espressif/esp_modem>`_
* `nghttp <https://components.espressif.com/component/espressif/nghttp>`_

.. note:: Please note that http parser functionality which was previously part of ``nghttp`` component is now part of :component:`http_parser <http_parser>` component.

* `sh2lib <https://components.espressif.com/component/espressif/sh2lib>`_
* `expat <https://components.espressif.com/component/espressif/expat>`_

These components can be installed using ``idf.py add-dependency`` command.

For example, to install libsodium component with exact version X.Y, run: ``idf.py add-dependency libsodium==X.Y``.

To install libsodium component with the latest version compatible to X.Y according to `semver <https://semver.org/>`_ rules, run: ``idf.py add-dependency libsodium~X.Y``.

To find out which versions of each component are available, open https://components.espressif.com, search for the component by its name and check the versions listed on the component page.
