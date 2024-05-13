file(GLOB GLAD_SRC ${THIRD_LIBRARY_DIR}/glad/src/*.c)

add_custom_target(CopyShaders ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${CMAKE_BINARY_DIR}/Renderer/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Shaders"
        "${CMAKE_BINARY_DIR}/Renderer/Shaders"
        COMMENT "Copying shaders to build directory")

add_executable(OpenGI Renderer.cpp ${GLAD_SRC})
add_dependencies(${CMAKE_PROJECT_NAME} CopyShaders)