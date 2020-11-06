NVS Page Test for Host
======================

Build
-----

First, make sure that the target is set to linux.
Run ``idf.py --preview set-target linux`` to be sure.
Then do a normal IDF build: ``idf.py build``.

Run
---

IDF monitor doesn't work yet for Linux.
You have to run the app manually: ``./build/host_nvs_page_test.elf``.

Coverage
---

To generate the coverage, run: ``idf.py coverage``.
Afterwards, you can view the coverage by opening ``build/coverage_report/index.html`` with your browser.
Note that you need to run the application at least once before generating the coverage information.
If you run it multiple times, the coverage information adds up.
