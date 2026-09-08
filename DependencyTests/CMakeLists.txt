cmake_minimum_required(VERSION 3.15)
project(OpenEphysFFTWDependencySmokeTest LANGUAGES C)

if(NOT DEFINED FFTW_ROOT)
    message(FATAL_ERROR "Configure with -DFFTW_ROOT=<extracted dependency bundle>")
endif()

find_path(FFTW_INCLUDE_DIR fftw3.h PATHS "${FFTW_ROOT}/include" NO_DEFAULT_PATH)
find_library(FFTW_DOUBLE_LIBRARY NAMES fftw3 libfftw3-3
             PATHS "${FFTW_ROOT}/lib" NO_DEFAULT_PATH)
find_library(FFTW_FLOAT_LIBRARY NAMES fftw3f libfftw3f-3
             PATHS "${FFTW_ROOT}/lib" NO_DEFAULT_PATH)
if(NOT FFTW_INCLUDE_DIR OR NOT FFTW_DOUBLE_LIBRARY OR NOT FFTW_FLOAT_LIBRARY)
    message(FATAL_ERROR "The FFTW bundle is incomplete")
endif()

add_executable(fftw_dependency_smoke fftw_dependency_smoke.c)
target_include_directories(fftw_dependency_smoke PRIVATE "${FFTW_INCLUDE_DIR}")
target_link_libraries(fftw_dependency_smoke PRIVATE
                      "${FFTW_DOUBLE_LIBRARY}" "${FFTW_FLOAT_LIBRARY}")

enable_testing()
add_test(NAME fftw_dependency_smoke COMMAND fftw_dependency_smoke)
set_tests_properties(fftw_dependency_smoke PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:${FFTW_ROOT}/bin;LD_LIBRARY_PATH=path_list_prepend:${FFTW_ROOT}/bin;DYLD_LIBRARY_PATH=path_list_prepend:${FFTW_ROOT}/bin")
