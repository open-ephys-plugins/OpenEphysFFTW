include(FetchContent)

function(openephys_acquire_fftw OUT_ROOT)
	set(FFTW_DEPENDENCY_VERSION "3.3.11")
	set(FFTW_DEPENDENCY_TAG "fftw-dependencies-3.3.11-1" CACHE STRING
		"Immutable OpenEphysFFTW dependency release tag")
	set(FFTW_RELEASE_BASE_URL
		"https://github.com/open-ephys-plugins/OpenEphysFFTW/releases/download"
		CACHE STRING "Base URL for OpenEphysFFTW dependency releases")
	set(FFTW_ROOT "" CACHE PATH "Existing extracted FFTW dependency bundle")
	set(FFTW_ARCHIVE "" CACHE FILEPATH "Local FFTW bundle archive for offline builds")
	set(FFTW_CHECKSUM_FILE "" CACHE FILEPATH
		"SHA-256 sidecar for FFTW_ARCHIVE; defaults to <archive>.sha256")

	if(FFTW_ROOT)
		get_filename_component(bundle_root "${FFTW_ROOT}" ABSOLUTE)
	else()
		if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(bundle_name "fftw-${FFTW_DEPENDENCY_VERSION}-linux-x86_64")
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(bundle_name "fftw-${FFTW_DEPENDENCY_VERSION}-windows-x86_64")
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
			set(bundle_name "fftw-${FFTW_DEPENDENCY_VERSION}-macos-universal")
		else()
			message(FATAL_ERROR
				"No released FFTW bundle supports ${CMAKE_SYSTEM_NAME}/${CMAKE_SIZEOF_VOID_P}-byte pointers. "
				"Set FFTW_ROOT to a compatible extracted bundle.")
		endif()
		set(archive_name "${bundle_name}.tar.gz")

		if(FFTW_ARCHIVE)
			get_filename_component(archive_url "${FFTW_ARCHIVE}" ABSOLUTE)
			if(FFTW_CHECKSUM_FILE)
				get_filename_component(checksum_file "${FFTW_CHECKSUM_FILE}" ABSOLUTE)
			else()
				set(checksum_file "${archive_url}.sha256")
			endif()
			if(NOT EXISTS "${archive_url}" OR NOT EXISTS "${checksum_file}")
				message(FATAL_ERROR
					"FFTW_ARCHIVE and its checksum sidecar must both exist")
			endif()
		else()
			set(release_url
				"${FFTW_RELEASE_BASE_URL}/${FFTW_DEPENDENCY_TAG}")
			set(archive_url "${release_url}/${archive_name}")
			set(checksum_dir
				"${CMAKE_BINARY_DIR}/_deps/openephys-fftw-checksums/${FFTW_DEPENDENCY_TAG}")
			set(checksum_file "${checksum_dir}/${archive_name}.sha256")
			if(NOT EXISTS "${checksum_file}")
				file(MAKE_DIRECTORY "${checksum_dir}")
				set(checksum_download "${checksum_file}.download")
				file(REMOVE "${checksum_download}")
				file(DOWNLOAD "${archive_url}.sha256" "${checksum_download}"
					TLS_VERIFY ON STATUS checksum_status)
				list(GET checksum_status 0 checksum_status_code)
				if(NOT checksum_status_code EQUAL 0)
					file(REMOVE "${checksum_download}")
					list(GET checksum_status 1 checksum_status_message)
					message(FATAL_ERROR
						"Could not download FFTW checksum: ${checksum_status_message}")
				endif()
				file(RENAME "${checksum_download}" "${checksum_file}")
			endif()
		endif()

		file(STRINGS "${checksum_file}" checksum_line LIMIT_COUNT 1)
		string(REGEX MATCH "^([0-9A-Fa-f]+)[ \t]+\\*?([^ \t\r\n]+)$"
			checksum_match "${checksum_line}")
		set(expected_sha256 "${CMAKE_MATCH_1}")
		set(checksum_archive_name "${CMAKE_MATCH_2}")
		string(LENGTH "${expected_sha256}" checksum_length)
		if(NOT checksum_match OR NOT checksum_length EQUAL 64 OR
		   NOT checksum_archive_name STREQUAL archive_name)
			message(FATAL_ERROR
				"Invalid FFTW checksum sidecar: ${checksum_file}")
		endif()

		if(POLICY CMP0135)
			cmake_policy(SET CMP0135 NEW)
		endif()
		FetchContent_Declare(OpenEphysFFTWDependency
			URL "${archive_url}"
			URL_HASH "SHA256=${expected_sha256}"
			TLS_VERIFY ON)
		FetchContent_MakeAvailable(OpenEphysFFTWDependency)
		FetchContent_GetProperties(OpenEphysFFTWDependency
			SOURCE_DIR bundle_root)
	endif()

	if(NOT EXISTS "${bundle_root}/include/fftw3.h" OR
	   NOT IS_DIRECTORY "${bundle_root}/lib" OR
	   NOT IS_DIRECTORY "${bundle_root}/bin" OR
	   NOT EXISTS "${bundle_root}/share/fftw/COPYING")
		message(FATAL_ERROR "FFTW_ROOT is not a complete dependency bundle: ${bundle_root}")
	endif()

	set(${OUT_ROOT} "${bundle_root}" PARENT_SCOPE)
endfunction()
