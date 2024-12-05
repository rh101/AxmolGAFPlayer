set(target_name gafplayer)

file(GLOB_RECURSE gaf_source_files Sources/*.cpp Sources/*.h)
file(GLOB SHADER_FILES Sources/Shaders/*.*)

add_library(${target_name} ${gaf_source_files} ${SHADER_FILES})

add_dependencies(${target_name} ${_AX_CORE_LIB})

target_link_libraries(${target_name} 
    PRIVATE ${_AX_CORE_LIB}
)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(${target_name}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Sources
    )

ax_find_shaders(${CMAKE_CURRENT_LIST_DIR}/Sources/Shaders GAF_SHADER_SOURCES)
ax_target_compile_shaders(${target_name} FILES ${GAF_SHADER_SOURCES} CUSTOM)
