cmake_minimum_required(VERSION 3.15)
if (NOT DEFINED GUI_BASE_DIR)
	if (DEFINED ENV{GUI_BASE_DIR})
		set(GUI_BASE_DIR $ENV{GUI_BASE_DIR})
	else()
		set(GUI_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin-GUI)
	endif()
endif()

get_filename_component(PROJECT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(COMMONLIB_NAME ${PROJECT_FOLDER} NAME)

set(CMAKE_XCODE_GENERATE_SCHEME YES)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architecture for Mac OS X" FORCE)

project(OE_COMMONLIB_${COMMONLIB_NAME})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(LINUX 1)
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Debug)
	endif()
endif()

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
	"$<$<PLATFORM_ID:Windows>:JUCE_API=__declspec(dllimport)>"
	$<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
	$<$<PLATFORM_ID:Linux>:JUCE_DISABLE_NATIVE_FILECHOOSERS=1>
	$<$<CONFIG:Debug>:DEBUG=1>
	$<$<CONFIG:Debug>:_DEBUG=1>
	$<$<NOT:$<CONFIG:Debug>>:NDEBUG=1>
	)

set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Source)
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false "${SOURCE_PATH}/*.cpp")
file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false "${SOURCE_PATH}/*.h")
list(APPEND SRC_FILES ${HEADER_FILES})
set(GUI_COMMONLIB_DIR ${GUI_BASE_DIR}/installed_libs)

list(APPEND CMAKE_PREFIX_PATH 
	${GUI_COMMONLIB_DIR}
	${GUI_COMMONLIB_DIR}/$<CONFIG>
	${GUI_COMMONLIB_DIR}/Release # default to Release if, e.g., a RelWithDebInfo build of a library is not available
	)

add_library(${COMMONLIB_NAME} SHARED ${SRC_FILES})

target_compile_features(${COMMONLIB_NAME} PUBLIC cxx_auto_type cxx_generalized_initializers cxx_std_17)
target_include_directories(${COMMONLIB_NAME} PUBLIC
	${GUI_BASE_DIR}/JuceLibraryCode
	${GUI_BASE_DIR}/JuceLibraryCode/modules
	${GUI_BASE_DIR}/Plugins/Headers
	${GUI_COMMONLIB_DIR}/include)

set(GUI_BIN_DIR ${GUI_BASE_DIR}/Build/$<CONFIG>)

if (NOT CMAKE_LIBRARY_ARCHITECTURE)
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CMAKE_LIBRARY_ARCHITECTURE "x64")
	else()
		set(CMAKE_LIBRARY_ARCHITECTURE "x86")
	endif()
endif()

set(FFTW_ROOT "" CACHE PATH "Existing staged FFTW dependency root")
if(FFTW_ROOT)
	get_filename_component(FFTW_DEPENDENCY_ROOT "${FFTW_ROOT}" ABSOLUTE)
else()
	if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FFTW_PLATFORM windows)
	elseif(LINUX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FFTW_PLATFORM linux)
	elseif(APPLE)
		set(FFTW_PLATFORM macos)
	else()
		message(FATAL_ERROR
			"No managed FFTW bundle supports this platform. Set FFTW_ROOT explicitly.")
	endif()

	set(FFTW_DEPENDENCY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libs/${FFTW_PLATFORM}")
	execute_process(
		COMMAND "${CMAKE_COMMAND}"
			-DPLATFORM=${FFTW_PLATFORM}
			-DOUTPUT_DIR=${CMAKE_CURRENT_SOURCE_DIR}/libs
			-P ${CMAKE_CURRENT_SOURCE_DIR}/tools/PrepareFFTWBundle.cmake
		RESULT_VARIABLE FFTW_PREPARE_RESULT)
	if(NOT FFTW_PREPARE_RESULT EQUAL 0)
		message(FATAL_ERROR
			"Could not stage FFTW. Connect to the network or configure with FFTW_ROOT.")
	endif()
endif()

install(TARGETS ${COMMONLIB_NAME}
	LIBRARY DESTINATION ${GUI_COMMONLIB_DIR}/$<CONFIG>/lib/${CMAKE_LIBRARY_ARCHITECTURE} # .so, .dylib files
	ARCHIVE DESTINATION ${GUI_COMMONLIB_DIR}/$<CONFIG>/lib/${CMAKE_LIBRARY_ARCHITECTURE} # .lib files
	RUNTIME DESTINATION ${GUI_COMMONLIB_DIR}/$<CONFIG>/bin/${CMAKE_LIBRARY_ARCHITECTURE} # .dll files
	)
install(FILES ${HEADER_FILES} DESTINATION ${GUI_COMMONLIB_DIR}/include)
install(DIRECTORY ${FFTW_DEPENDENCY_ROOT}/share/fftw
	DESTINATION ${GUI_COMMONLIB_DIR}/share)

#Libraries and compiler options
if(MSVC)
	target_link_libraries(${COMMONLIB_NAME} ${GUI_BIN_DIR}/open-ephys.lib)
	target_compile_options(${COMMONLIB_NAME} PRIVATE /sdl- /W0)

	install(TARGETS ${COMMONLIB_NAME} RUNTIME DESTINATION ${GUI_BIN_DIR}/shared)

elseif(LINUX)
	target_link_libraries(${COMMONLIB_NAME} GL X11 Xext Xinerama asound dl freetype pthread rt)
	set_property(TARGET ${COMMONLIB_NAME} APPEND_STRING PROPERTY LINK_FLAGS
		"-fvisibility=hidden -fPIC -rdynamic -Wl,-rpath,'$ORIGIN'")
	target_compile_options(${COMMONLIB_NAME} PRIVATE -fPIC -rdynamic)
	target_compile_options(${COMMONLIB_NAME} PRIVATE -O3) #enable optimization for linux debug
	
	install(TARGETS ${COMMONLIB_NAME} LIBRARY DESTINATION ${GUI_BIN_DIR}/shared)

elseif(APPLE)
	set_target_properties(${COMMONLIB_NAME} PROPERTIES BUNDLE FALSE)
	set_property(TARGET ${COMMONLIB_NAME} APPEND_STRING PROPERTY LINK_FLAGS
	"-undefined dynamic_lookup")

	set_target_properties(${COMMONLIB_NAME} PROPERTIES
		XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)

	install(TARGETS ${COMMONLIB_NAME} DESTINATION $ENV{HOME}/Library/Application\ Support/open-ephys/shared-api10)
endif()

#create filters for vs and xcode

foreach( src_file IN ITEMS ${SRC_FILES})
	get_filename_component(src_path "${src_file}" PATH)
	file(RELATIVE_PATH src_path_rel "${SOURCE_PATH}" "${src_path}")
	string(REPLACE "/" "\\" group_name "${src_path_rel}")
	source_group("${group_name}" FILES "${src_file}")
endforeach()

#additional libraries

find_library(FFTW_LIBRARY NAMES fftw3
	PATHS ${FFTW_DEPENDENCY_ROOT}/lib
		${FFTW_DEPENDENCY_ROOT}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
	NO_DEFAULT_PATH)
find_library(FFTW_F_LIBRARY NAMES fftw3f
	PATHS ${FFTW_DEPENDENCY_ROOT}/lib
		${FFTW_DEPENDENCY_ROOT}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
	NO_DEFAULT_PATH)
find_path(FFTW_INCLUDE_DIRS fftw3.h
	PATHS ${FFTW_DEPENDENCY_ROOT}/include
	NO_DEFAULT_PATH)

if (NOT FFTW_LIBRARY)
	message(FATAL_ERROR "FFTW double-precision library not found")
else()
	message(STATUS "FFTW library: ${FFTW_LIBRARY}")
endif()

if (NOT FFTW_F_LIBRARY)
	message(FATAL_ERROR "FFTW single-precision library not found")
else()
	message(STATUS "FFTW single-precision library: ${FFTW_F_LIBRARY}")
endif()

if (NOT FFTW_INCLUDE_DIRS)
	message(FATAL_ERROR "FFTW header not found")
endif()


if (MSVC)
	install(DIRECTORY ${FFTW_DEPENDENCY_ROOT}/bin/${CMAKE_LIBRARY_ARCHITECTURE}/ DESTINATION ${GUI_BIN_DIR}/shared CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
elseif(LINUX)
	install(DIRECTORY ${FFTW_DEPENDENCY_ROOT}/bin/ DESTINATION ${GUI_BIN_DIR}/shared)
elseif(APPLE)
	install(DIRECTORY ${FFTW_DEPENDENCY_ROOT}/bin/ DESTINATION $ENV{HOME}/Library/Application\ Support/open-ephys/shared-api10)
endif()

target_link_libraries(${COMMONLIB_NAME} ${FFTW_LIBRARY} ${FFTW_F_LIBRARY})
target_include_directories(${COMMONLIB_NAME} PRIVATE ${FFTW_INCLUDE_DIRS})
