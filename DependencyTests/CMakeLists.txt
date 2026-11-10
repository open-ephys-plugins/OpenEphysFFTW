cmake_minimum_required(VERSION 3.22)
project(OpenEphysFFTWDependencySmokeTest LANGUAGES C)

if(NOT DEFINED FFTW_ROOT)
    message(FATAL_ERROR "Configure with -DFFTW_ROOT=<extracted dependency bundle>")
endif()

find_path(FFTW_INCLUDE_DIR fftw3.h PATHS "${FFTW_ROOT}/include" NO_DEFAULT_PATH)
find_library(FFTW_DOUBLE_LIBRARY NAMES fftw3 libfftw3-3
             PATHS "${FFTW_ROOT}/lib" "${FFTW_ROOT}/lib/x64" NO_DEFAULT_PATH)
find_library(FFTW_FLOAT_LIBRARY NAMES fftw3f libfftw3f-3
             PATHS "${FFTW_ROOT}/lib" "${FFTW_ROOT}/lib/x64" NO_DEFAULT_PATH)
if(NOT FFTW_INCLUDE_DIR OR NOT FFTW_DOUBLE_LIBRARY OR NOT FFTW_FLOAT_LIBRARY)
    message(FATAL_ERROR "The FFTW bundle is incomplete")
endif()

add_executable(fftw_dependency_smoke fftw_dependency_smoke.c)
target_include_directories(fftw_dependency_smoke PRIVATE "${FFTW_INCLUDE_DIR}")
target_link_libraries(fftw_dependency_smoke PRIVATE
                      "${FFTW_DOUBLE_LIBRARY}" "${FFTW_FLOAT_LIBRARY}")

enable_testing()
add_test(NAME fftw_dependency_smoke COMMAND fftw_dependency_smoke)
set(fftw_runtime_dir "${FFTW_ROOT}/bin")
if(WIN32)
    if(EXISTS "${FFTW_ROOT}/bin/x64")
        set(fftw_runtime_dir "${FFTW_ROOT}/bin/x64")
    endif()
endif()
set_tests_properties(fftw_dependency_smoke PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:${fftw_runtime_dir};LD_LIBRARY_PATH=path_list_prepend:${fftw_runtime_dir};DYLD_LIBRARY_PATH=path_list_prepend:${fftw_runtime_dir}")
