cmake_minimum_required(VERSION 3.26)
project(SoftRenderer)

set(CMAKE_CXX_STANDARD 20)
set(THIRD_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")
project ("OpenGI")

if (WIN32)
    link_libraries("${THIRD_LIBRARY_DIR}/glfw/glfw3.lib")
    find_package(OpenGL REQUIRED)
endif ()

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/Renderer/include"
        "${THIRD_LIBRARY_DIR}/glfw/include"
        "${THIRD_LIBRARY_DIR}/glad/include"
        "${THIRD_LIBRARY_DIR}/glm/include"
        "${THIRD_LIBRARY_DIR}/imgui"
)

add_subdirectory ("Renderer")
