.. include:: ../../components/vfs/README.rst

Application Example
-------------------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

API Reference
-------------

Macros
^^^^^^

.. doxygendefine:: ESP_VFS_PATH_MAX
.. doxygendefine:: ESP_VFS_FLAG_DEFAULT
.. doxygendefine:: ESP_VFS_FLAG_CONTEXT_PTR

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_vfs_t

Functions
^^^^^^^^^

.. doxygenfunction:: esp_vfs_register
.. doxygenfunction:: esp_vfs_write
.. doxygenfunction:: esp_vfs_lseek
.. doxygenfunction:: esp_vfs_read
.. doxygenfunction:: esp_vfs_open
.. doxygenfunction:: esp_vfs_close
.. doxygenfunction:: esp_vfs_fstat
.. doxygenfunction:: esp_vfs_stat
.. doxygenfunction:: esp_vfs_link
.. doxygenfunction:: esp_vfs_unlink
.. doxygenfunction:: esp_vfs_rename
.. doxygenfunction:: esp_vfs_dev_uart_register
