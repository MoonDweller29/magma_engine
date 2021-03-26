cmake_minimum_required (VERSION 3.14)
set(PROJECT_NAME magma-engine)
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

set(LIBRARY_NAME magma)

add_library(${LIBRARY_NAME} STATIC
		include/magma/glm_inc.h
		include/magma/app/keyboard.h
		src/magma/app/keyboard.cpp
		include/magma/app/Camera.h
		src/magma/app/Camera.cpp
		include/magma/app/mouse.h
		src/magma/app/mouse.cpp
		include/magma/app/clock.h
		src/magma/app/clock.cpp
		include/magma/app/image.h
		src/magma/app/image.cpp

		include/magma/app/scene/mesh.h
		src/magma/app/scene/mesh.cpp
		include/magma/app/scene/meshReader.h
		src/magma/app/scene/meshReader.cpp
		include/magma/app/scene/directLight.h
		src/magma/app/scene/directLight.cpp

		include/magma/app/render/depthPass.h
		src/magma/app/render/depthPass.cpp
		include/magma/app/render/GBuffer.h
		src/magma/app/render/GBuffer.cpp
		include/magma/app/render/MainColorPass.h
		src/magma/app/render/MainColorPass.cpp
		include/magma/app/render/GBufferResolve.h
		src/magma/app/render/GBufferResolve.cpp
		include/magma/app/render/SwapChainImageSupplier.h
		src/magma/app/render/SwapChainImageSupplier.cpp

		include/magma/vk/vulkan_common.h
		src/magma/vk/vulkan_common.cpp
		include/magma/vk/Context.h
		src/magma/vk/Context.cpp
		include/magma/vk/Extensions.h
		src/magma/vk/Extensions.cpp
		include/magma/vk/validationLayers/ValidationLayers.h
		src/magma/vk/validationLayers/ValidationLayers.cpp
		include/magma/vk/validationLayers/DebugMessenger.h
		src/magma/vk/validationLayers/DebugMessenger.cpp

		include/magma/vk/physicalDevice/PhysicalDevice.h
		src/magma/vk/physicalDevice/PhysicalDevice.cpp
		include/magma/vk/physicalDevice/HardwareManager.h
		src/magma/vk/physicalDevice/HardwareManager.cpp
		include/magma/vk/physicalDevice/DeviceRequirements.h
		src/magma/vk/physicalDevice/DeviceRequirements.cpp

		include/magma/vk/LogicalDevice.h
		src/magma/vk/LogicalDevice.cpp
		include/magma/vk/Window.h
		src/magma/vk/Window.cpp
		include/magma/vk/SwapChain.h
		src/magma/vk/SwapChain.cpp
		include/magma/vk/ComputePipeline.h
		src/magma/vk/ComputePipeline.cpp
		include/magma/vk/graphicsPipeline.h
		src/magma/vk/graphicsPipeline.cpp
		include/magma/vk/ShaderModule.h
		src/magma/vk/ShaderModule.cpp
		include/magma/vk/FrameBuffer.h
		src/magma/vk/FrameBuffer.cpp

		include/magma/vk/commands/CommandBuffer.h
		src/magma/vk/commands/CommandBuffer.cpp
		include/magma/vk/commands/CommandBufferArr.h
		src/magma/vk/commands/CommandBufferArr.cpp
		include/magma/vk/commands/CommandPool.h
		src/magma/vk/commands/CommandPool.cpp
		include/magma/vk/commands/SingleTimeCommandBuffer.h
		src/magma/vk/commands/SingleTimeCommandBuffer.cpp

		include/magma/vk/textures/TextureManager.h
		src/magma/vk/textures/TextureManager.cpp
		include/magma/vk/textures/CustomImageView.h
		src/magma/vk/textures/CustomImageView.cpp
		include/magma/vk/textures/ImageView.h
		src/magma/vk/textures/ImageView.cpp
		include/magma/vk/textures/Texture.h
		src/magma/vk/textures/Texture.cpp

		include/magma/vk/buffers/BufferManager.h
		include/magma/vk/buffers/BufferManager.hpp
		src/magma/vk/buffers/BufferManager.cpp
		include/magma/vk/buffers/Buffer.h
		src/magma/vk/buffers/Buffer.cpp

		include/magma/vk/descriptors/descriptorSetLayout.h
		src/magma/vk/descriptors/descriptorSetLayout.cpp
		include/magma/vk/descriptors/descriptorPool.h
		src/magma/vk/descriptors/descriptorPool.cpp
		include/magma/vk/descriptors/descriptorSetInfo.h
		src/magma/vk/descriptors/descriptorSetInfo.cpp
		include/magma/vk/CmdSync.h
		src/magma/vk/CmdSync.cpp
		include/magma/app/config/JSON.h
		src/magma/app/config/JSON.cpp
		include/magma/app/log.hpp
		src/magma/app/log.cpp
		include/magma/app/Option.hpp
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
# vulkan-hpp defines
target_compile_definitions(${LIBRARY_NAME} PUBLIC VULKAN_HPP_NO_EXCEPTIONS)
target_compile_definitions(${LIBRARY_NAME} PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)


# //////////////////////////////////////////////////////
# editor
# //////////////////////////////////////////////////////

set(EDITOR_EXE_NAME ${LIBRARY_NAME}_editor)

add_executable(${EDITOR_EXE_NAME}
		src/main.cpp
		include/magma/app/App.h
		src/magma/app/App.cpp
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
