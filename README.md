
<pre>
▐▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▌
▐                                                                                 ▌
▐  ______  ___                   ______  ___            __________                ▌
▐  ___   |/  /___________ ___    ___   |/  /_______________(_)_  /______________  ▌
▐  __  /|_/ /_  _ \_  __ `__ \   __  /|_/ /_  __ \_  __ \_  /_  __/  __ \_  ___/  ▌
▐  _  /  / / /  __/  / / / / /   _  /  / / / /_/ /  / / /  / / /_ / /_/ /  /      ▌
▐  /_/  /_/  \___//_/ /_/ /_/    /_/  /_/  \____//_/ /_//_/  \__/ \____//_/       ▌
▐                                                                                 ▌
▐▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▌
</pre>

[![codecov](https://codecov.io/github/Gruncan/mem-monitor/graph/badge.svg?token=KQ9ULYVIQJ)](https://codecov.io/github/Gruncan/mem-monitor)


## Build instructions

**You must** include the instructions necessary to build and deploy this project successfully. If appropriate, also include
instructions to run automated tests.

### Requirements

All components have been developed on Linux kernel 6.11 with `gcc` 13.3.0. 
For the MemoryGui and decoder library some support for windows have been implemented however Linux is advised. 

All other components are Linux only.

MemoryGui requires QT Gui to be installed and modification of the CMake, see [dev-setup.md](components/memory-gui/dev-setup.md).

Python scripts have been developed for 3.12.3 however any modern python version should work.
All requirements for said scripts are available within their respective directories

### Build steps

The easy setup would be to execute the shell script `gen-release-artifacts.sh`
```
$ cd scripts
$ ./gen-release-artifacts.sh
```
This will generate all components into `components` directory, this navigates the directory and executes `make` in each sub directory

CMake support is also available from `components/CMakeLists.txt` if QT is not found MemoryGui will not compile. 
CMake is intended for debug only `make` is preferred for release



