# @TODO: proper subproject

set(TEST_EXE_NAME ${LIBRARY_NAME}_tests)

add_executable(${TEST_EXE_NAME}
        test_config.cpp
        test_logger.cpp
        test_main.cpp
)

target_link_libraries(${TEST_EXE_NAME}
        ${LIBRARY_NAME}
)
