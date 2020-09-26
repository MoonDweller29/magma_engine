cmake_minimum_required (VERSION 3.8)
set(PROJECT_NAME vulkan_3d)
set(EXE_NAME vk_3D)
project (${PROJECT_NAME})

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
# set cpp std=c++17
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)


# //////////////////////////////////////////////////////
# include Vulkan
# //////////////////////////////////////////////////////
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
set(ALL_LIBS ${Vulkan_LIBRARY} )

message("+++ THIS is CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
# //////////////////////////////////////////////////////
#
# //////////////////////////////////////////////////////

# compiling glfw
	set(GLFW_BUILD_DOCS OFF
		GLFW_BUILD_TESTS OFF
		GLFW_BUILD_EXAMPLES OFF
	)
	add_subdirectory(external/glfw)
	set(ALL_LIBS  ${ALL_LIBS} glfw)

# compiling assimp
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF
		ASSIMP_BUILD_TESTS OFF
		ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF
		ASSIMP_BUILD_OBJ_IMPORTER ON
	)
	add_subdirectory(external/assimp)
	set(ALL_LIBS ${ALL_LIBS} assimp)

# //////////////////////////////////////////////////////
#
# //////////////////////////////////////////////////////


add_executable(${EXE_NAME}
		src/main.cpp
		include/glm_inc.h
		include/app/App.h
		src/app/App.cpp
		include/app/keyboard.h
		src/app/keyboard.cpp
		include/app/camera.h
		src/app/camera.cpp
		include/app/mouse.h
		src/app/mouse.cpp
		include/app/clock.h
		src/app/clock.cpp
		include/app/image.h
		src/app/image.cpp

		include/app/scene/mesh.h
		src/app/scene/mesh.cpp
		include/app/scene/meshReader.h
		src/app/scene/meshReader.cpp
		include/app/scene/directLight.h
		src/app/scene/directLight.cpp

		include/app/render/colorPass.h
		src/app/render/colorPass.cpp
		include/app/render/depthPass.h
		src/app/render/depthPass.cpp

		include/vk/vulkan_common.h
		src/vk/vulkan_common.cpp
		include/vk/vkInstanceHolder.h
		src/vk/vkInstanceHolder.cpp
		include/vk/vk_extentions.h
		src/vk/vk_extentions.cpp
		include/vk/validationLayers.h
		src/vk/validationLayers.cpp
		include/vk/physicalDevice.h
		src/vk/physicalDevice.cpp
		include/vk/logicalDevice.h
		src/vk/logicalDevice.cpp
		include/vk/logicalDevice.hpp
		include/vk/window.h
		src/vk/window.cpp
		include/vk/swapChain.h
		src/vk/swapChain.cpp
		include/vk/graphicsPipeline.h
		src/vk/graphicsPipeline.cpp
		include/vk/shaderModule.h
		src/vk/shaderModule.cpp
		include/vk/renderPass.h
		src/vk/renderPass.cpp
		include/vk/frameBuffer.h
		src/vk/frameBuffer.cpp
		include/vk/commandPool.h
		src/vk/commandPool.cpp
		include/vk/commandBuffer.h
		src/vk/commandBuffer.cpp
		include/vk/buffer.h
		src/vk/buffer.cpp
		include/vk/texture.h
		src/vk/texture.cpp
		include/vk/descriptors/descriptorSetLayout.h
		src/vk/descriptors/descriptorSetLayout.cpp
		include/vk/descriptors/descriptorPool.h
		src/vk/descriptors/descriptorPool.cpp
		include/vk/descriptors/descriptorSetInfo.h
		src/vk/descriptors/descriptorSetInfo.cpp
		include/vk/cmdSync.h
		src/vk/cmdSync.cpp
)

target_include_directories(${EXE_NAME}
		PUBLIC include
		PUBLIC external/glm
		PRIVATE external/stb
)
target_link_libraries(${EXE_NAME} ${ALL_LIBS})



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
add_dependencies(${EXE_NAME} shaders_compilation)

# //////////////////////////////////////////////////////
#
# //////////////////////////////////////////////////////