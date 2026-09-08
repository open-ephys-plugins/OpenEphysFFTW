# OpenEphysFFTW

This is a small library to wrap FFTW functionality useful for Open Ephys.

It is currently used by the [Phase Calculator](https://github.com/tne-lab/phase-calculator) and Real-Time Coherence plugins (under development).

## Dependency

OpenEphysFFTW uses the complete non-threaded, shared double- and
single-precision FFTW 3.3.11 libraries. Platform bundles are prepared from
hash-pinned conda-forge packages; FFTW's license, provenance, and exact build
recipe accompany every bundle. The corresponding FFTW source is published with
the dependency release.

Run the packaging workflow without its publishing option to validate all three
bundles. Publishing is a deliberate, manual operation because dependency
release tags are immutable inputs to plugin builds.

The bundles intentionally omit the separate long-double, MPI, OpenMP, and
threaded FFTW libraries. The `fftw3` and `fftw3f` libraries themselves are not
modified or symbol-stripped.

## Installation

This library should be built outside of the main GUI file tree using CMake. In order to do so, it must be in a sibling directory (e.g. OEPlugins) to `plugin-GUI` and the GUI must have already been compiled.

See the wiki page [here](https://open-ephys.github.io/gui-docs/Developer-Guide/Compiling-plugins.html) for build instructions.
