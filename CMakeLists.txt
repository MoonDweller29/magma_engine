cmake_minimum_required (VERSION 3.14)
set(PROJECT_NAME vtb)
project (${PROJECT_NAME})

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# set cpp std=c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# //////////////////////////////////////////////////////
# external_libs
# //////////////////////////////////////////////////////

# including Vulkan
find_package(Vulkan REQUIRED)

add_subdirectory(external)

# //////////////////////////////////////////////////////
# main library
# //////////////////////////////////////////////////////

set(LIBRARY_NAME vtb)

add_library(${LIBRARY_NAME} STATIC
		include/vtb/glm_inc.h
		include/vtb/app/keyboard.h
		src/vtb/app/keyboard.cpp
		include/vtb/app/camera.h
		src/vtb/app/camera.cpp
		include/vtb/app/mouse.h
		src/vtb/app/mouse.cpp
		include/vtb/app/clock.h
		src/vtb/app/clock.cpp
		include/vtb/app/image.h
		src/vtb/app/image.cpp

		include/vtb/app/scene/mesh.h
		src/vtb/app/scene/mesh.cpp
		include/vtb/app/scene/meshReader.h
		src/vtb/app/scene/meshReader.cpp
		include/vtb/app/scene/directLight.h
		src/vtb/app/scene/directLight.cpp

		include/vtb/app/render/colorPass.h
		src/vtb/app/render/colorPass.cpp
		include/vtb/app/render/depthPass.h
		src/vtb/app/render/depthPass.cpp

		include/vtb/vk/vulkan_common.h
		src/vtb/vk/vulkan_common.cpp
		include/vtb/vk/vkInstanceHolder.h
		src/vtb/vk/vkInstanceHolder.cpp
		include/vtb/vk/vk_extensions.h
		src/vtb/vk/vk_extensions.cpp
		include/vtb/vk/validationLayers.h
		src/vtb/vk/validationLayers.cpp
		include/vtb/vk/physicalDevice.h
		src/vtb/vk/physicalDevice.cpp
		include/vtb/vk/logicalDevice.h
		src/vtb/vk/logicalDevice.cpp
		include/vtb/vk/logicalDevice.hpp
		include/vtb/vk/window.h
		src/vtb/vk/window.cpp
		include/vtb/vk/swapChain.h
		src/vtb/vk/swapChain.cpp
		include/vtb/vk/graphicsPipeline.h
		src/vtb/vk/graphicsPipeline.cpp
		include/vtb/vk/shaderModule.h
		src/vtb/vk/shaderModule.cpp
		include/vtb/vk/renderPass.h
		src/vtb/vk/renderPass.cpp
		include/vtb/vk/frameBuffer.h
		src/vtb/vk/frameBuffer.cpp
		include/vtb/vk/commandPool.h
		src/vtb/vk/commandPool.cpp
		include/vtb/vk/commandBuffer.h
		src/vtb/vk/commandBuffer.cpp
		include/vtb/vk/buffer.h
		src/vtb/vk/buffer.cpp
		include/vtb/vk/texture.h
		src/vtb/vk/texture.cpp
		include/vtb/vk/descriptors/descriptorSetLayout.h
		src/vtb/vk/descriptors/descriptorSetLayout.cpp
		include/vtb/vk/descriptors/descriptorPool.h
		src/vtb/vk/descriptors/descriptorPool.cpp
		include/vtb/vk/descriptors/descriptorSetInfo.h
		src/vtb/vk/descriptors/descriptorSetInfo.cpp
		include/vtb/vk/cmdSync.h
		src/vtb/vk/cmdSync.cpp
		include/vtb/app/config/JSON.h
		include/vtb/app/log.hpp
		src/vtb/app/log.cpp
)

target_include_directories(${LIBRARY_NAME}
		PUBLIC include
		PUBLIC ${Vulkan_INCLUDE_DIR}
)
target_link_libraries(${LIBRARY_NAME}
		${Vulkan_LIBRARY}
		glfw
		glm
		assimp
		nlohmann_json
		stb
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
		include/vtb/app/App.h
		src/vtb/app/App.cpp
)
target_link_libraries(${EDITOR_EXE_NAME}
        ${LIBRARY_NAME}
)


# //////////////////////////////////////////////////////
# shaders compilation
# //////////////////////////////////////////////////////

add_subdirectory(shaders)
add_dependencies(${LIBRARY_NAME} shaders_compilation)

# //////////////////////////////////////////////////////
# tests
# //////////////////////////////////////////////////////

add_subdirectory(test)

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
