# OpenEphysFFTW

This is a small library to wrap FFTW functionality useful for Open Ephys.

It is currently used by the [Phase Calculator](https://github.com/tne-lab/phase-calculator) and Real-Time Coherence plugins (under development).

## Dependency

On Windows, Linux, and Mac, the precompiled FFTW library is included in the `libs` folder - no need to install it manually.

## Installation

This library should be built outside of the main GUI file tree using CMake. In order to do so, it must be in a sibling directory (e.g. OEPlugins) to `plugin-GUI` and the GUI must have already been compiled.

See the wiki page [here](https://open-ephys.github.io/gui-docs/Developer-Guide/Compiling-plugins.html) for build instructions.