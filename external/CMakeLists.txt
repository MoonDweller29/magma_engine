#compiling SFML
set(SFML_BUILD_GRAPHICS FALSE CACHE STRING "" FORCE)
set(SFML_BUILD_AUDIO FALSE CACHE STRING "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE STRING "" FORCE)
add_subdirectory(sfml)
add_library(sfml INTERFACE)
target_include_directories(sfml INTERFACE sfml/include)
target_link_libraries(sfml INTERFACE
        sfml-window
        ${CMAKE_DL_LIBS}
)

# compiling assimp
if (MINGW)
    add_compile_definitions(_SSIZE_T_DEFINED)
endif()

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_EXPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(assimp)

# including glm
add_subdirectory(glm)

# JSON
set(JSON_BuildTests OFF)
add_subdirectory(json)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)
