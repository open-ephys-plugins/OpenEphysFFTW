# OpenEphysFFTW

This is a small library to wrap FFTW functionality useful for Open Ephys.

It is currently used by the [Phase Calculator](https://github.com/tne-lab/phase-calculator) and Real-Time Coherence plugins (under development).

## Dependency

* On Mac, you need to install FFTW 3 (if not already present) into the standard library/header locations. See [here](http://www.fftw.org/download.html) for instructions on building from source.

* On Windows and Linux, the precompiled FFTW library is included in the `libs` folder - no need to install it manually.

## Installation

This library should be built outside of the main GUI file tree using CMake. In order to do so, it must be in a sibling directory to plugin-GUI\* and the main GUI must have already been compiled.

See `OpenEphysFFTW/CMAKE_README.txt` and/or the wiki page [here](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/1259110401/Plugin+CMake+Builds) for build instructions.

\* If you have the GUI built somewhere else, you can specify its location by setting the environment variable `GUI_BASE_DIR` or defining it when calling cmake with the option `-DGUI_BASE_DIR=<location>`.
