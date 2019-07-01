# IDF wrapper tool (idf.py.exe)

This tools helps invoke idf.py in Windows CMD shell.

In Windows CMD shell, python scripts can be executed directly (by typing their name) if `.py` extension is associated with Python. The issue with such association is that it is incompatible with virtual environments. That is, if `.py` extension is associated with a global (or user-specific) copy of `python.exe`, then the virtual environment will not be used when running the script. [Python Launcher](https://www.python.org/dev/peps/pep-0397/) solves this issue, but it is installed by default only with Python 3.6 and newer. In addition to that, the user may choose not to install Python Launcher (for example, to keep `.py` files associated with an editor).

Hence, `idf.py.exe` is introduced. It is a simple program which forwards the command line arguments to `idf.py`. That is,

```
idf.py args...
```
has the same effect as:

```
python.exe %IDF_PATH%\tools\idf.py args...
```

`python.exe` location is determined using the default search rules, which include searching the directories in `%PATH%`. Standard I/O streams are forwarded between `idf.py.exe` and `python.exe` processes. The exit code of `idf.py.exe` is the same as the exit code of `python.exe` process.

For compatibility with `idf_tools.py`, a flag to obtain the version of `idf.py.exe` is provided: `idf.py.exe -v` or `idf.py.exe --version`. Note that this flag only works when `idf.py.exe` is started by the full name (with `.exe` extension). Running `idf.py -v` results in same behavior as `python idf.py -v`, that is `-v` argument is propagated to the Python script.

## Building

On Linux/Mac, install mingw-w64 toolchain (`i686-w64-mingw32-gcc`). Then build `idf.py.exe` using CMake:

```
mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-i686-w64-mingw32.cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

On Windows, it is also possible to build using Visual Studio, with CMake support installed.

