# @file    CMakeLists.txt
# @brief   Test directory CMake configuration.

# Custom target to run ceedling tests
add_custom_target(run_ceedling_tests
    COMMAND ceedling test:all
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running Ceedling tests..."
)

# Create an alias "make test" target if it doesn't conflict with CTest
if(NOT TARGET test)
    add_custom_target(test DEPENDS run_ceedling_tests)
endif()
