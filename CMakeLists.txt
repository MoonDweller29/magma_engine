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
find_package(Vulkan)
if (NOT Vulkan_FOUND)
	message(SEND_ERROR "FAILED to find Vulkan")
	return()
else()
	include_directories(${Vulkan_INCLUDE_DIR})
	set(ALL_LIBS  ${Vulkan_LIBRARY} )
endif()

message("+++ THIS is CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
# //////////////////////////////////////////////////////
#
# //////////////////////////////////////////////////////


# //////////////////////////////////////////////////////
# external_libs
# //////////////////////////////////////////////////////
include_directories(src)
include_directories(external_libs/include)
include_directories(external_libs/glfw-3.3.2/include)
include_directories(external_libs/assimp/include)

# compiling glfw
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	add_subdirectory(external_libs/glfw-3.3.2/)
	set(ALL_LIBS  ${ALL_LIBS} glfw)

# compiling assimp
	# ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT:False
	# ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT:False
	set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
	# ASSIMP_BUILD_STL_IMPORTER:True
	set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	add_subdirectory(external_libs/assimp)
	set(ALL_LIBS ${ALL_LIBS} assimp)

# //////////////////////////////////////////////////////
#
# //////////////////////////////////////////////////////


add_executable(${EXE_NAME} 
	src/main.cpp
	src/glm_inc.h

	src/app/App.h
	src/app/App.cpp
	src/app/keyboard.h
	src/app/keyboard.cpp
	src/app/camera.h
	src/app/camera.cpp
	src/app/mouse.h
	src/app/mouse.cpp
	src/app/clock.h
	src/app/clock.cpp
	src/app/image.h
	src/app/image.cpp

	src/app/scene/mesh.h
	src/app/scene/mesh.cpp
	src/app/scene/meshReader.h
	src/app/scene/meshReader.cpp
	src/app/scene/directLight.h
	src/app/scene/directLight.cpp

	src/app/render/colorPass.h
	src/app/render/colorPass.cpp
	src/app/render/depthPass.h
	src/app/render/depthPass.cpp

	src/vk/vulkan_common.h
	src/vk/vulkan_common.cpp
	src/vk/vkInstanceHolder.h
	src/vk/vkInstanceHolder.cpp
	src/vk/vk_extentions.h
	src/vk/vk_extentions.cpp
	src/vk/validationLayers.h
	src/vk/validationLayers.cpp
	src/vk/physicalDevice.h
	src/vk/physicalDevice.cpp
	src/vk/logicalDevice.h
	src/vk/logicalDevice.cpp
	src/vk/logicalDevice.hpp
	src/vk/window.h
	src/vk/window.cpp
	src/vk/swapChain.h
	src/vk/swapChain.cpp
	src/vk/graphicsPipeline.h
	src/vk/graphicsPipeline.cpp
	src/vk/shaderModule.h
	src/vk/shaderModule.cpp
	src/vk/renderPass.h
	src/vk/renderPass.cpp
	src/vk/frameBuffer.h
	src/vk/frameBuffer.cpp
	src/vk/commandPool.h
	src/vk/commandPool.cpp
	src/vk/commandBuffer.h
	src/vk/commandBuffer.cpp
	src/vk/buffer.h
	src/vk/buffer.cpp
	src/vk/texture.h
	src/vk/texture.cpp
	src/vk/descriptors/descriptorSetLayout.h
	src/vk/descriptors/descriptorSetLayout.cpp
	src/vk/descriptors/descriptorPool.h
	src/vk/descriptors/descriptorPool.cpp
	src/vk/descriptors/descriptorSetInfo.h
	src/vk/descriptors/descriptorSetInfo.cpp
	src/vk/cmdSync.h
	src/vk/cmdSync.cpp
)

target_link_libraries(${EXE_NAME} ${ALL_LIBS} )



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