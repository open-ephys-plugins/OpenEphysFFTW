# OpenEphysFFTW

This is a small library to wrap FFTW functionality useful for Open Ephys.

It is currently used by the [Phase Calculator](https://github.com/tne-lab/phase-calculator) and Real-Time Coherence plugins (under development).

## Dependency

OpenEphysFFTW uses the complete non-threaded, shared double- and
single-precision FFTW 3.3.11 libraries. Platform bundles are prepared from
hash-pinned conda-forge packages; FFTW's license, provenance, and exact build
recipe accompany every bundle. The corresponding FFTW source is published with
the dependency release.

FFTW is redistributed under GPL-2.0-or-later. Conda-forge's recipe scripts are
redistributed under BSD-3-Clause; their notice and disclaimer remain alongside
the recipes. The bundles identify conda-forge only as their build provenance
and do not imply endorsement.

The packaging workflow validates all three bundles. A dependency tag stages a
draft release containing the bundles, checksum sidecars, and corresponding
source. Enable GitHub release immutability for this repository before
publishing the draft. Normal builds can then verify each archive against its
immutable checksum sidecar without copying generated hashes back into Git.

The bundles intentionally omit the separate long-double, MPI, OpenMP, and
threaded FFTW libraries. The `fftw3` and `fftw3f` libraries themselves are not
modified or symbol-stripped.

## Installation

This library should be built outside of the main GUI file tree using CMake. In order to do so, it must be in a sibling directory (e.g. OEPlugins) to `plugin-GUI` and the GUI must have already been compiled.

See the wiki page [here](https://open-ephys.github.io/gui-docs/Developer-Guide/Compiling-plugins.html) for build instructions.
