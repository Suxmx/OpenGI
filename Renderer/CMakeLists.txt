file(GLOB GLAD_SRC ${THIRD_LIBRARY_DIR}/glad/src/*.c)
file(GLOB IMGUI_SRC
        "${CMAKE_SOURCE_DIR}/3rdparty/imgui/*.cpp"
        "${CMAKE_SOURCE_DIR}/3rdparty/imgui/backend/imgui_impl_glfw.cpp"
        "${CMAKE_SOURCE_DIR}/3rdparty/imgui/backend/imgui_impl_opengl3.cpp"
)

add_custom_target(CopyShaders ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${CMAKE_BINARY_DIR}/Renderer/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Shaders"
        "${CMAKE_BINARY_DIR}/Renderer/Shaders"
        COMMENT "Copying shaders to build directory")

add_executable(OpenGI Renderer.cpp ${GLAD_SRC} ${IMGUI_SRC})

add_dependencies(${CMAKE_PROJECT_NAME} CopyShaders)