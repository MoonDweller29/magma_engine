# @TODO: proper paths and subprojects

# create dir for shaders
set(SHADER_EXE_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${SHADER_EXE_DIR})

# shaders_compilation for linux
set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADERS
        shader.vert
        shader.frag
        depth.vert
        mainColorPass.vert
        mainColorPass.frag
        imageProcess.vert
        imageTransfer.frag
        gBufferResolve.frag
        motionVector.comp
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
