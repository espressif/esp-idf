IDF Windows Installer
=====================

Command-line parameters
-----------------------

Windows Installer `esp-idf-tools-setup` provides the following command-line parameters:

* ``/CONFIG=[PATH]`` - Path to ``ini`` configuration file to override default configuration of the installer. Default: ``config.ini``.
* ``/GITCLEAN=[yes|no]`` - Perform git clean and remove untracked directories in Offline mode installation. Default: yes.
* ``/GITRECURSIVE=[yes|no]`` - Clone recursively all git repository submodules. Default: yes
* ``/GITREPO=[URL|PATH]`` - URL of repository to clone ESP-IDF. Default: https://github.com/espressif/esp-idf.git
* ``/GITRESET=[yes|no]`` - Enable/Disable git reset of repository during installation. Default: yes.
* ``/HELP`` - Display command line options provided by Inno Setup installer.
* ``/IDFDIR=[PATH]`` - Path to directory where it will be installed. Default: ``{userdesktop}\esp-idf}``
* ``/IDFVERSION=[v4.3|v4.1|master]`` - Use specific IDF version. E.g. v4.1, v4.2, master. Default: empty, pick the first version in the list.
* ``/IDFVERSIONSURL=[URL]`` - Use URL to download list of IDF versions. Default: https://dl.espressif.com/dl/esp-idf/idf_versions.txt
* ``/LOG=[PATH]`` - Store installation log file in specific directory. Default: empty.
* ``/OFFLINE=[yes|no]`` - Execute installation of Python packages by PIP in offline mode. The same result can be achieved by setting the environment variable PIP_NO_INDEX. Default: no.
* ``/USEEMBEDDEDPYTHON=[yes|no]`` - Use Embedded Python version for the installation. Set to ``no`` to allow Python selection screen in the installer. Default: yes.
* ``/PYTHONNOUSERSITE=[yes|no]`` - Set PYTHONNOUSERSITE variable before launching any Python command to avoid loading Python packages from AppData\Roaming. Default: yes.
* ``/PYTHONWHEELSURL=[URL]`` - Specify URLs to PyPi repositories for resolving binary Python Wheel dependencies. The same result can be achieved by setting the environment variable PIP_EXTRA_INDEX_URL. Default: https://dl.espressif.com/pypi
* ``/SKIPSYSTEMCHECK=[yes|no]`` - Skip System Check page. Default: no.
* ``/VERYSILENT /SUPPRESSMSGBOXES /SP- /NOCANCEL`` - Perform silent installation.

Unattended installation
-----------------------

The unattended installation of IDF can be achieved by following command-line parameters:

.. code-block:: batch

    esp-idf-tools-setup-x.x.exe /VERYSILENT /SUPPRESSMSGBOXES /SP- /NOCANCEL

The installer detaches its process from the command-line. Waiting for installation to finish could be achieved by following PowerShell script:

.. code-block:: powershell

    esp-idf-tools-setup-x.x.exe /VERYSILENT /SUPPRESSMSGBOXES /SP- /NOCANCEL
    $InstallerProcess = Get-Process esp-idf-tools-setup
    Wait-Process -Id $InstallerProcess.id

Custom Python and custom location of Python wheels
--------------------------------------------------

The IDF installer is using by default embedded Python with reference to Python Wheel mirror.

Following parameters allows to select custom Python and custom location of Python wheels:

.. code-block:: batch

    esp-idf-tools-setup-x.x.exe /USEEMBEDDEDPYTHON=no /PYTHONWHEELSURL=https://pypi.org/simple/
