file (GLOB_RECURSE TARGET_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

set (TARGET_NAME PatchSection)

add_executable (${TARGET_NAME}
	${TARGET_SOURCE}
)

set_target_properties (${TARGET_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/$<CONFIG>
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/$<CONFIG>
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/$<CONFIG>
	C_STANDARD 99
)

target_compile_options (${TARGET_NAME} PRIVATE
	$<$<CONFIG:Debug>:
		$<$<C_COMPILER_ID:MSVC>:/MDd>
		$<$<C_COMPILER_ID:GNU>:-g>
		$<$<C_COMPILER_ID:Clang>:-g>
	>

	$<$<CONFIG:Release>:
		$<$<C_COMPILER_ID:MSVC>:/MD>
		$<$<C_COMPILER_ID:GNU>:-O3>
		$<$<C_COMPILER_ID:Clang>:-O3>
	>
)

target_compile_definitions (${TARGET_NAME} PRIVATE
	$<$<CONFIG:Debug>:HF_DEBUG>

	$<$<PLATFORM_ID:Windows>:
		HF_PLATFORM_WINDOWS
		HF_PAGE_SIZE=0x1000
	>

	$<$<PLATFORM_ID:Linux>:HF_PLATFORM_LINUX>
)

target_include_directories (${TARGET_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(TARGET ${TARGET_NAME}
	POST_BUILD
	COMMAND ${CMAKE_SOURCE_DIR}/$<CONFIG>/PatchSection.exe Write ${CMAKE_SOURCE_DIR}/$<CONFIG>/HexFlow.exe r ${CMAKE_SOURCE_DIR}/Shaders/FontVertexShader.glsl .vshdr
	COMMAND ${CMAKE_SOURCE_DIR}/$<CONFIG>/PatchSection.exe Write ${CMAKE_SOURCE_DIR}/$<CONFIG>/HexFlow.exe r ${CMAKE_SOURCE_DIR}/Shaders/FontFragmentShader.glsl .fshdr
	COMMAND ${CMAKE_SOURCE_DIR}/$<CONFIG>/PatchSection.exe Write ${CMAKE_SOURCE_DIR}/$<CONFIG>/HexFlow.exe rb ${CMAKE_SOURCE_DIR}/Fonts/CommitMono-400-Regular.otf .dfont
	COMMAND ${CMAKE_SOURCE_DIR}/$<CONFIG>/PatchSection.exe Write ${CMAKE_SOURCE_DIR}/$<CONFIG>/HexFlow.exe r ${CMAKE_SOURCE_DIR}/Configs/DefaultUserConfig.txt .ucfg
)