# FFTW Dependency Releases

OpenEphysFFTW publishes third-party FFTW binaries separately from the wrapper
library. Dependency updates intentionally use two pull requests so reviewed
build machinery never depends on assets that do not yet exist.

## PR 1: Prepare the dependency release

Update the pinned conda-forge package names and SHA-256 hashes in
`tools/PrepareFFTWBundle.cmake`. Update the FFTW source version and hash in the
packaging workflow at the same time. PR 1 may change packaging, provenance,
smoke tests, and CI, but must not remove the currently usable dependency files
or make normal builds require the proposed release.

The pull request and branch-push workflows must successfully prepare and test
Linux x86-64, Windows x86-64, and universal macOS bundles.

After PR 1 merges:

1. Enable **Release immutability** under the repository's release settings.
2. Update local `main` to the reviewed merge commit.
3. Create and push the versioned dependency tag:

   ```bash
   git tag -a fftw-dependencies-3.3.11-1 \
     -m "FFTW dependency bundles 3.3.11-1"
   git push origin fftw-dependencies-3.3.11-1
   ```

4. Confirm that the `package FFTW dependencies` workflow passes on all three
   platforms. A failure does not create a release.
5. Inspect the draft release created by the workflow. It must contain three
   bundles and checksum sidecars, the corresponding FFTW source and checksum,
   and the expected license, recipe, and provenance inside each bundle.
6. Publish the draft. Publication locks its tag and assets and creates GitHub's
   release attestation.

Do not publish an incomplete draft. If a published release is wrong, correct
the packaging and use a new revision tag such as `-2`; immutable tags cannot be
reused.

## PR 2: Consume the dependency release

Only after publication, open a second PR that makes CMake download the
versioned platform archive and checksum sidecar. It must retain an explicit
local/offline bundle override. Remove the old files from the current `libs/`
tree without rewriting Git history.

PR 2 is complete when clean Linux, Windows, and macOS jobs can configure,
build, install, and load both `fftw3` and `fftw3f` from the immutable release.
