# OpenEphysFFTW

This is a small library to wrap FFTW functionality useful for Open Ephys.

It is currently used by the [Phase Calculator](https://github.com/tne-lab/phase-calculator) and Real-Time Coherence plugins (under development).

## Dependency

OpenEphysFFTW uses complete non-threaded, shared double- and single-precision
FFTW 3.3.11 libraries. On the first configuration, CMake downloads exact,
hash-pinned conda-forge packages and stages the current platform under `libs/`.
Later configurations validate and reuse those ignored local files; conda itself
is not required.

FFTW is redistributed under GPL-2.0-or-later. Conda-forge's recipe scripts are
redistributed under BSD-3-Clause; their notice and disclaimer remain alongside
the recipes. The bundles identify conda-forge only as their build provenance
and do not imply endorsement.

For an offline build or a read-only source checkout, set `FFTW_ROOT` to a
previously staged platform directory:

```bash
cmake -S . -B Build -DFFTW_ROOT=/path/to/libs/linux
```

Automatic staging requires CMake 3.24 or newer. Builds using `FFTW_ROOT` retain
the project's CMake 3.15 minimum.

The bundles intentionally omit the separate long-double, MPI, OpenMP, and
threaded FFTW libraries. The `fftw3` and `fftw3f` libraries themselves are not
modified or symbol-stripped.

The packaged library includes FFTW's license, source provenance, and the
conda-forge recipe used to build the binaries.

## Installation

This library should be built outside of the main GUI file tree using CMake. In order to do so, it must be in a sibling directory (e.g. OEPlugins) to `plugin-GUI` and the GUI must have already been compiled.

See the wiki page [here](https://open-ephys.github.io/gui-docs/Developer-Guide/Compiling-plugins.html) for build instructions.
