Documenting Code
================

Introduction
------------

When documenting code for this repository, please follow `Doxygen style <http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html#specialblock>`_. You are doing it by inserting special commands, for instance ``@param``, into standard comments blocks like for example ``/* @param ratio this is oxygen to air ratio */``. 

Doxygen is phrasing the code, extracting the commands together with subsequent text, and building documentation out of it.

Typical comment block, that contains documentation of a function, looks like below.

.. image:: _static/doc-code-documentation-inline.png
   :align: center
   :alt: Sample inline code documentation
 
Doxygen supports couple of formatting styles. It also gives you great flexibility on level of details to include in documentation. To get the taste of available features please check data reach and very well organized `Doxygen Manual <http://www.stack.nl/~dimitri/doxygen/manual/index.html>`_.

Why we need it?
---------------

The purpose of this description is to provide quick summary on documentation style used in `espressif/esp-idf <https://github.com/espressif/esp-idf>`_ repository. 

The ultimate goal is to ensure that all the code is consistently documented, so we can use tools like `Sphinx <http://www.sphinx-doc.org/>`_ and `Breathe <https://breathe.readthedocs.io/>`_ to aid preparation and automatic updates of API documentation when the code changes. The above piece of code renders in Sphinx like below:

.. image:: _static/doc-code-documentation-rendered.png
   :align: center
   :alt: Sample inline code after rendering

 
Go for it!
----------

When writing code for this repository, please follow guidelines below.

  1. Document all building blocks of code: functions, structs, typedefs, enums, macros, etc. Provide enough information on purpose, functionality and limitations of documented items, as you would like to see them documented when reading the code by others.

  2. Documentation of function should describe what this function does. If it accepts input parameters and returns some value, all of them should be explained.

  3. Do not add a data type before parameter or any other characters besides spaces. All spaces and line breaks are compressed into a single space. If you like to break a line, then break it twice.

    .. image:: _static/doc-code-function.png
       :align: center
       :alt: Sample function documented inline and after rendering

  4. If function has void input or does not return any value, then skip ``@param`` or ``@return``

    .. image:: _static/doc-code-void-function.png
       :align: center
       :alt: Sample void function documented inline and after rendering
 
  5. When documenting members of a ``struct``, ``typedef`` or ``enum``, place specific comment like below after each member.

    .. image:: _static/doc-code-member.png
       :align: center
       :alt: Sample of member documentation inline and after rendering
 
  6. To provide well formatted lists, break the line after command (like ``@return`` in example below).

    ::

      ...
      *
      * @return
      *    - ESP_OK if erase operation was successful
      *    - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
      *    - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
      *    - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
      *    - other error codes from the underlying storage driver
      *
      ...
 

  7. Overview of functionality of documented header file, or group of files that make a library, should be placed in separate ``README.rst`` file. 

Go one extra mile
-----------------

There are couple of tips how you can make your documentation even better and more useful to the reader.

Add code snippets to illustrate implementation. To do so, enclose the snippet using ``@code{c}`` and ``@endcode`` commands.

::

   ...
   *
   * @code{c}
   * // Example of using nvs_get_i32:
   * int32_t max_buffer_size = 4096; // default value
   * esp_err_t err = nvs_get_i32(my_handle, "max_buffer_size", &max_buffer_size);
   * assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
   * // if ESP_ERR_NVS_NOT_FOUND was returned, max_buffer_size will still
   * // have its default value.
   * @endcode
   *
   ...

To highlight some information use command ``@attention`` or ``@note``. Example below also shows how to use a numbered list.

::

   ...
   *
   * @attention
   *     1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
   *     2. If the ESP32 is connected to an AP, call esp_wifi_disconnect to disconnect.
   *
   ...


Use markdown to make your documentation even more readable. With markdown you can add headers, links, tables and more.

::

   ...
   *
   * [ESP32 Technical Reference](http://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
   *
   ...

Wrap up
-------

We love good code that is doing cool things. 
We love it even better, if it is well documented, so we can quickly make it run and also do the cool things.

