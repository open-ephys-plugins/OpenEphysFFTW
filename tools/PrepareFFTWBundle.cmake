cmake_minimum_required(VERSION 3.24)

if(NOT DEFINED PLATFORM OR NOT PLATFORM MATCHES "^(linux|macos|windows)$")
    message(FATAL_ERROR "Set PLATFORM to linux, macos, or windows")
endif()
if(NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "Set OUTPUT_DIR to the libs directory the bundle should be staged into")
endif()

set(FFTW_VERSION 3.3.11)
file(MAKE_DIRECTORY "${OUTPUT_DIR}")
file(LOCK "${OUTPUT_DIR}/.prepare-${PLATFORM}.lock"
    GUARD PROCESS TIMEOUT 300 RESULT_VARIABLE lock_result)
if(lock_result)
    message(FATAL_ERROR "Could not lock the local FFTW stage: ${lock_result}")
endif()

set(WORK_DIR "${OUTPUT_DIR}/.fftw-package-work-${PLATFORM}")
set(FINAL_BUNDLE_DIR "${OUTPUT_DIR}/${PLATFORM}")
set(BUNDLE_DIR "${OUTPUT_DIR}/.${PLATFORM}-staging")
file(SHA256 "${CMAKE_CURRENT_LIST_FILE}" prepare_sha256)
string(CONCAT expected_stage_marker
    "FFTW ${FFTW_VERSION}\n"
    "Prepare script SHA-256: ${prepare_sha256}\n")

# Only the x86_64/universal binaries are fetched, so Windows stages under an
# arch subfolder to match the existing libs/windows/{lib,bin}/x64 layout.
set(LIB_SUBDIR "lib")
set(BIN_SUBDIR "bin")
if(PLATFORM STREQUAL "windows")
    set(LIB_SUBDIR "lib/x64")
    set(BIN_SUBDIR "bin/x64")
endif()

set(required_files
    "include/fftw3.h"
    "share/fftw/COPYING"
    "share/fftw/PROVENANCE.txt")
if(PLATFORM STREQUAL "linux")
    list(APPEND required_files
        "lib/libfftw3.so" "lib/libfftw3f.so"
        "bin/libfftw3.so.3" "bin/libfftw3f.so.3")
elseif(PLATFORM STREQUAL "windows")
    list(APPEND required_files
        "lib/x64/fftw3.lib" "lib/x64/fftw3f.lib"
        "bin/x64/fftw3.dll" "bin/x64/fftw3f.dll")
elseif(PLATFORM STREQUAL "macos")
    list(APPEND required_files
        "lib/libfftw3.dylib" "lib/libfftw3f.dylib"
        "bin/libfftw3.3.dylib" "bin/libfftw3f.3.dylib")
endif()

set(bundle_complete TRUE)
foreach(required_file IN LISTS required_files)
    if(NOT EXISTS "${FINAL_BUNDLE_DIR}/${required_file}")
        set(bundle_complete FALSE)
    endif()
endforeach()
set(stage_marker "${FINAL_BUNDLE_DIR}/share/fftw/STAGE.txt")
if(EXISTS "${stage_marker}")
    file(READ "${stage_marker}" actual_stage_marker)
else()
    set(actual_stage_marker "")
endif()
if(NOT actual_stage_marker STREQUAL expected_stage_marker)
    set(bundle_complete FALSE)
endif()
if(bundle_complete)
    message(STATUS "Using staged FFTW ${FFTW_VERSION}: ${FINAL_BUNDLE_DIR}")
    return()
endif()

file(REMOVE_RECURSE "${WORK_DIR}" "${BUNDLE_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}" "${BUNDLE_DIR}/include"
                    "${BUNDLE_DIR}/${LIB_SUBDIR}" "${BUNDLE_DIR}/${BIN_SUBDIR}"
                    "${BUNDLE_DIR}/share/fftw")

function(download_conda_package LABEL SUBDIR FILENAME SHA256 OUT_ROOT)
    set(package "${WORK_DIR}/${FILENAME}")
    set(container "${WORK_DIR}/${LABEL}-container")
    set(root "${WORK_DIR}/${LABEL}-root")

    file(DOWNLOAD
        "https://conda.anaconda.org/conda-forge/${SUBDIR}/${FILENAME}"
        "${package}"
        EXPECTED_HASH "SHA256=${SHA256}"
        TLS_VERIFY ON
        SHOW_PROGRESS)
    file(MAKE_DIRECTORY "${container}" "${root}")
    file(ARCHIVE_EXTRACT INPUT "${package}" DESTINATION "${container}")
    file(GLOB payload "${container}/pkg-*.tar.zst")
    file(GLOB package_info "${container}/info-*.tar.zst")
    list(LENGTH payload payload_count)
    list(LENGTH package_info info_count)
    if(NOT payload_count EQUAL 1 OR NOT info_count EQUAL 1)
        message(FATAL_ERROR "Unexpected ${LABEL} .conda package structure")
    endif()
    file(ARCHIVE_EXTRACT INPUT "${payload}" DESTINATION "${root}")
    file(ARCHIVE_EXTRACT INPUT "${package_info}" DESTINATION "${root}")
    set(${OUT_ROOT} "${root}" PARENT_SCOPE)
endfunction()

function(copy_file SOURCE DESTINATION)
    if(NOT EXISTS "${SOURCE}")
        message(FATAL_ERROR "Required package file is missing: ${SOURCE}")
    endif()
    get_filename_component(destination_dir "${DESTINATION}" DIRECTORY)
    file(MAKE_DIRECTORY "${destination_dir}")
    configure_file("${SOURCE}" "${DESTINATION}" COPYONLY)
endfunction()

if(PLATFORM STREQUAL "linux")
    download_conda_package(
        linux linux-64 fftw-3.3.11-nompi_h3b011a4_100.conda
        6fd5d681fba20adaca771f138ac52dbf0a52e0dc2ac31b9ce7406068d102a9a7
        package_root)
    copy_file("${package_root}/include/fftw3.h" "${BUNDLE_DIR}/include/fftw3.h")
    foreach(precision IN ITEMS "" f)
        copy_file("${package_root}/lib/libfftw3${precision}.so.3.7.11"
                  "${BUNDLE_DIR}/${LIB_SUBDIR}/libfftw3${precision}.so")
        copy_file("${package_root}/lib/libfftw3${precision}.so.3.7.11"
                  "${BUNDLE_DIR}/${BIN_SUBDIR}/libfftw3${precision}.so.3")
    endforeach()
    set(license_root "${package_root}")
    string(CONCAT binary_provenance
        "linux-64/fftw-3.3.11-nompi_h3b011a4_100.conda\n"
        "SHA-256: 6fd5d681fba20adaca771f138ac52dbf0a52e0dc2ac31b9ce7406068d102a9a7\n")
elseif(PLATFORM STREQUAL "windows")
    download_conda_package(
        windows win-64 fftw-3.3.11-nompi_h6877c38_100.conda
        bee7a80261a6344597125ecf4af4405d841c81b46461e3327f7fe6a148652426
        package_root)
    copy_file("${package_root}/Library/include/fftw3.h" "${BUNDLE_DIR}/include/fftw3.h")
    foreach(precision IN ITEMS "" f)
        copy_file("${package_root}/Library/lib/fftw3${precision}.lib"
                  "${BUNDLE_DIR}/${LIB_SUBDIR}/fftw3${precision}.lib")
        copy_file("${package_root}/Library/bin/fftw3${precision}.dll"
                  "${BUNDLE_DIR}/${BIN_SUBDIR}/fftw3${precision}.dll")
    endforeach()
    set(license_root "${package_root}")
    string(CONCAT binary_provenance
        "win-64/fftw-3.3.11-nompi_h6877c38_100.conda\n"
        "SHA-256: bee7a80261a6344597125ecf4af4405d841c81b46461e3327f7fe6a148652426\n")
elseif(PLATFORM STREQUAL "macos")
    download_conda_package(
        macos_arm osx-arm64 fftw-3.3.11-nompi_haf1500d_100.conda
        fc6c507d7c68db156d6c8c5f6a79ca6b34c2a4c0c6222d8d4ecd0e4b97d3fd5e
        arm_root)
    download_conda_package(
        macos_x64 osx-64 fftw-3.3.11-nompi_h54214ab_100.conda
        c234b8f1be5b630236675a006172edd768ca2685fbf0713ec007f3d373f5ee27
        x64_root)
    copy_file("${arm_root}/include/fftw3.h" "${BUNDLE_DIR}/include/fftw3.h")
    foreach(precision IN ITEMS "" f)
        set(runtime_name "libfftw3${precision}.3.dylib")
        execute_process(
            COMMAND lipo -create
                "${arm_root}/lib/${runtime_name}"
                "${x64_root}/lib/${runtime_name}"
                -output "${BUNDLE_DIR}/${BIN_SUBDIR}/${runtime_name}"
            COMMAND_ERROR_IS_FATAL ANY)
        execute_process(
            COMMAND install_name_tool -id "@rpath/${runtime_name}"
                "${BUNDLE_DIR}/${BIN_SUBDIR}/${runtime_name}"
            COMMAND_ERROR_IS_FATAL ANY)
        copy_file("${BUNDLE_DIR}/${BIN_SUBDIR}/${runtime_name}"
                  "${BUNDLE_DIR}/${LIB_SUBDIR}/libfftw3${precision}.dylib")
    endforeach()
    set(license_root "${arm_root}")
    string(CONCAT binary_provenance
        "osx-arm64/fftw-3.3.11-nompi_haf1500d_100.conda\n"
        "SHA-256: fc6c507d7c68db156d6c8c5f6a79ca6b34c2a4c0c6222d8d4ecd0e4b97d3fd5e\n"
        "osx-64/fftw-3.3.11-nompi_h54214ab_100.conda\n"
        "SHA-256: c234b8f1be5b630236675a006172edd768ca2685fbf0713ec007f3d373f5ee27\n"
        "The two architecture slices were combined with Apple's lipo tool.\n")
endif()

copy_file("${license_root}/info/licenses/COPYING"
          "${BUNDLE_DIR}/share/fftw/COPYING")
if(PLATFORM STREQUAL "macos")
    file(COPY "${arm_root}/info/recipe" DESTINATION "${BUNDLE_DIR}/share/fftw")
    file(RENAME "${BUNDLE_DIR}/share/fftw/recipe"
                "${BUNDLE_DIR}/share/fftw/conda-recipe-osx-arm64")
    file(COPY "${x64_root}/info/recipe" DESTINATION "${BUNDLE_DIR}/share/fftw")
    file(RENAME "${BUNDLE_DIR}/share/fftw/recipe"
                "${BUNDLE_DIR}/share/fftw/conda-recipe-osx-64")
else()
    file(COPY "${package_root}/info/recipe" DESTINATION "${BUNDLE_DIR}/share/fftw")
    file(RENAME "${BUNDLE_DIR}/share/fftw/recipe"
                "${BUNDLE_DIR}/share/fftw/conda-recipe")
endif()

file(WRITE "${BUNDLE_DIR}/share/fftw/PROVENANCE.txt"
    "FFTW ${FFTW_VERSION}\n"
    "Source: https://www.fftw.org/fftw-${FFTW_VERSION}.tar.gz\n"
    "Source SHA-256: 5630c24cdeb33b131612f7eb4b1a9934234754f9f388ff8617458d0be6f239a1\n"
    "Binary source: https://conda.anaconda.org/conda-forge/\n"
    "${binary_provenance}"
    "The exact conda build recipe and its BSD-3-Clause license are included in this directory.\n"
    "Neither conda-forge nor its contributors endorse this redistribution.\n")

file(WRITE "${BUNDLE_DIR}/share/fftw/STAGE.txt" "${expected_stage_marker}")
file(REMOVE_RECURSE "${FINAL_BUNDLE_DIR}")
file(RENAME "${BUNDLE_DIR}" "${FINAL_BUNDLE_DIR}")
file(REMOVE_RECURSE "${WORK_DIR}")
message(STATUS "Staged FFTW ${FFTW_VERSION} (${PLATFORM}) into ${FINAL_BUNDLE_DIR}")
