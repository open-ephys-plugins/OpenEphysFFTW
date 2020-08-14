# OpenEphysFFTW

This is a small library to wrap FFTW functionality useful for Open Ephys.

It is currently used by the [Phase Calculator](https://github.com/tne-lab/phase-calculator) and Real-Time Coherence plugins (under development).

## Dependency

On Windows, Linux, and Mac, the precompiled FFTW library is included in the `libs` folder - no need to install it manually.

## Installation

This library should be built outside of the main GUI file tree using CMake. In order to do so, it must be in a sibling directory (e.g. OEPlugins) to `plugin-GUI` and the GUI must have already been compiled.

See the wiki page [here](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/1259110401/Plugin+CMake+Builds) for build instructions.

\* If you have the GUI built somewhere else, you can specify its location by setting the environment variable `GUI_BASE_DIR` or defining it when calling cmake with the option `-DGUI_BASE_DIR=<location>`.

#### [MacOS only] Update rpath
Update the rpath of fftw library linked to `libOpenEphysFFTW.dylib` by running the following commands:
```
install_name_tool -change /usr/local/opt/fftw/lib/libfftw3.3.dylib @loader_path/libfftw3.3.dylib path/to/libOpenEphysFFTW.dylib
```