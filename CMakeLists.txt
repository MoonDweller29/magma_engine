cmake_minimum_required (VERSION 3.14)
set(PROJECT_NAME vulkan_3d)
project (${PROJECT_NAME})

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
# set cpp std=c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# //////////////////////////////////////////////////////
# external_libs
# //////////////////////////////////////////////////////

# including Vulkan
find_package(Vulkan REQUIRED)

# compiling glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

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
add_subdirectory(external/assimp)

# including glm
add_subdirectory(external/glm)

# JSON
set(JSON_BuildTests OFF)
add_subdirectory(external/json)

# //////////////////////////////////////////////////////
# main library
# //////////////////////////////////////////////////////

set(LIBRARY_NAME vk_3D)

add_library(${LIBRARY_NAME} STATIC
		include/vk3d/glm_inc.h
		include/vk3d/app/keyboard.h
		src/vk3d/app/keyboard.cpp
		include/vk3d/app/camera.h
		src/vk3d/app/camera.cpp
		include/vk3d/app/mouse.h
		src/vk3d/app/mouse.cpp
		include/vk3d/app/clock.h
		src/vk3d/app/clock.cpp
		include/vk3d/app/image.h
		src/vk3d/app/image.cpp

		include/vk3d/app/scene/mesh.h
		src/vk3d/app/scene/mesh.cpp
		include/vk3d/app/scene/meshReader.h
		src/vk3d/app/scene/meshReader.cpp
		include/vk3d/app/scene/directLight.h
		src/vk3d/app/scene/directLight.cpp

		include/vk3d/app/render/colorPass.h
		src/vk3d/app/render/colorPass.cpp
		include/vk3d/app/render/depthPass.h
		src/vk3d/app/render/depthPass.cpp

		include/vk3d/vk/vulkan_common.h
		src/vk3d/vk/vulkan_common.cpp
		include/vk3d/vk/vkInstanceHolder.h
		src/vk3d/vk/vkInstanceHolder.cpp
		include/vk3d/vk/vk_extentions.h
		src/vk3d/vk/vk_extentions.cpp
		include/vk3d/vk/validationLayers.h
		src/vk3d/vk/validationLayers.cpp
		include/vk3d/vk/physicalDevice.h
		src/vk3d/vk/physicalDevice.cpp
		include/vk3d/vk/logicalDevice.h
		src/vk3d/vk/logicalDevice.cpp
		include/vk3d/vk/logicalDevice.hpp
		include/vk3d/vk/window.h
		src/vk3d/vk/window.cpp
		include/vk3d/vk/swapChain.h
		src/vk3d/vk/swapChain.cpp
		include/vk3d/vk/graphicsPipeline.h
		src/vk3d/vk/graphicsPipeline.cpp
		include/vk3d/vk/shaderModule.h
		src/vk3d/vk/shaderModule.cpp
		include/vk3d/vk/renderPass.h
		src/vk3d/vk/renderPass.cpp
		include/vk3d/vk/frameBuffer.h
		src/vk3d/vk/frameBuffer.cpp
		include/vk3d/vk/commandPool.h
		src/vk3d/vk/commandPool.cpp
		include/vk3d/vk/commandBuffer.h
		src/vk3d/vk/commandBuffer.cpp
		include/vk3d/vk/buffer.h
		src/vk3d/vk/buffer.cpp
		include/vk3d/vk/texture.h
		src/vk3d/vk/texture.cpp
		include/vk3d/vk/descriptors/descriptorSetLayout.h
		src/vk3d/vk/descriptors/descriptorSetLayout.cpp
		include/vk3d/vk/descriptors/descriptorPool.h
		src/vk3d/vk/descriptors/descriptorPool.cpp
		include/vk3d/vk/descriptors/descriptorSetInfo.h
		src/vk3d/vk/descriptors/descriptorSetInfo.cpp
		include/vk3d/vk/cmdSync.h
		src/vk3d/vk/cmdSync.cpp
		include/vk3d/app/config/JSON.h
		include/vk3d/app/log.hpp
		src/app/log.cpp
)

target_include_directories(${LIBRARY_NAME}
		PUBLIC include
		PUBLIC ${Vulkan_INCLUDE_DIR}
		PUBLIC external/stb
)
target_link_libraries(${LIBRARY_NAME}
		${Vulkan_LIBRARY}
		glfw
		glm
		assimp
		nlohmann_json
)
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_DIR_PATH_LENGTH)
target_compile_definitions(${LIBRARY_NAME}
		PUBLIC __FILENAME__=&__FILE__[${SOURCE_DIR_PATH_LENGTH}]
)

# //////////////////////////////////////////////////////
# editor
# //////////////////////////////////////////////////////

set(EDITOR_EXE_NAME ${LIBRARY_NAME}_editor)

add_executable(${EDITOR_EXE_NAME}
		src/main.cpp
		include/vk3d/app/App.h
		src/vk3d/app/App.cpp
)
target_link_libraries(${EDITOR_EXE_NAME}
        ${LIBRARY_NAME}
)


# //////////////////////////////////////////////////////
# shaders compilation
# //////////////////////////////////////////////////////

# create dir for shaders
set(SHADER_EXE_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_EXE_DIR})

# shaders_compilation for linux
set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADERS
	shader.vert
	shader.frag
	depth.vert
)

set(COMPILED_SHADERS)
foreach(SHADER_NAME ${SHADERS})
	set(COMPILED_SHADERS ${COMPILED_SHADERS} ${SHADER_EXE_DIR}/${SHADER_NAME}.spv)
endforeach(SHADER_NAME)

foreach(SHADER_NAME ${SHADERS})
	add_custom_command(OUTPUT ${SHADER_EXE_DIR}/${SHADER_NAME}.spv
	  # COMMAND glslc shader.vert -o vert.spv
	  COMMAND glslc ${SHADER_SRC_DIR}/${SHADER_NAME} -o ${SHADER_EXE_DIR}/${SHADER_NAME}.spv
	  # COMMAND echo ${CMAKE_SOURCE_DIR}
	  WORKING_DIRECTORY "./"
	  DEPENDS ${SHADER_SRC_DIR}/${SHADER_NAME}
	  COMMENT "compiling shader: ${SHADER_NAME}"
	  VERBATIM
	)
endforeach()

add_custom_target(shaders_compilation ALL DEPENDS ${COMPILED_SHADERS})
add_dependencies(${LIBRARY_NAME} shaders_compilation)

# //////////////////////////////////////////////////////
# tests
# //////////////////////////////////////////////////////

set(TEST_EXE_NAME ${LIBRARY_NAME}_tests)
add_executable(${TEST_EXE_NAME}
		test/test_config.cpp
		test/test_logger.cpp
		test/test_main.cpp
)
# TODO: prorer setup
target_link_libraries(${TEST_EXE_NAME}
		${LIBRARY_NAME}
)

# //////////////////////////////////////////////////////
# creating data directory
# //////////////////////////////////////////////////////

set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)
file(MAKE_DIRECTORY ${DATA_DIR})

# creating log file
set(DATA_DOWNLOADING_LOG ${CMAKE_CURRENT_BINARY_DIR}/data_downloading.log)
file(TOUCH ${DATA_DOWNLOADING_LOG})

# create build_info.config
set(BUILD_INFO_CONF_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/build_info.config)
file(TOUCH ${BUILD_INFO_CONF_FILENAME})
file(WRITE ${BUILD_INFO_CONF_FILENAME}
"{
    \"build_info\" : {
        \"src_dir\" : \"${CMAKE_SOURCE_DIR}\"
    }
}\n"
)

# //////////////////////////////////////////////////////
# loading models and textures
# //////////////////////////////////////////////////////
set(MODELS_DIR ${DATA_DIR}/models)
file(MAKE_DIRECTORY ${MODELS_DIR})
set(TEX_DIR ${DATA_DIR}/textures)
file(MAKE_DIRECTORY ${TEX_DIR})

set(MODELS
	viking_room.obj
)

set(TEXTURES
	viking_room.png
)

# loading models
foreach(MODEL_NAME ${MODELS})
	file(DOWNLOAD
		https://vulkan-tutorial.com/resources/${MODEL_NAME}
		${MODELS_DIR}/${MODEL_NAME}
		LOG DOWNLOADING_LOG
	)
	file(APPEND ${DATA_DOWNLOADING_LOG} ${DOWNLOADING_LOG})
endforeach(MODEL_NAME)

# loading textures
foreach(TEX_NAME ${TEXTURES})
	file(DOWNLOAD
		https://vulkan-tutorial.com/resources/${TEX_NAME}
		${TEX_DIR}/${TEX_NAME}
		LOG DOWNLOADING_LOG
	)
	file(APPEND ${DATA_DOWNLOADING_LOG} ${DOWNLOADING_LOG})
endforeach(TEX_NAME)
# //////////////////////////////////////////////////////
#
# //////////////////////////////////////////////////////
