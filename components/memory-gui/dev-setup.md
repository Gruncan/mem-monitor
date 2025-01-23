# Download Qt Development pack from the online installer

https://www.qt.io/download-dev

Select development pack for desktop.

# Update Qt path cmake

```cmake
if (UNIX)
    set(CMAKE_PREFIX_PATH /<path to>/gcc_64)
else if (WIN32)
    set(CMAKE_PREFIX_PATH C:\\<path to>\\mingw_64)
endif ()

```
# Update Qt path environment (Win)

For windows you need to add the Qt dll bin to the environment path variable

```
C:\\<path to>\\mingw_64\\bin
```