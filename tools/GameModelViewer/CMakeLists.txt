cmake_minimum_required(VERSION 3.25)

project(GameModelViewer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(GameModelViewer
    src/main.cpp
)

target_link_libraries(GameModelViewer PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    glm::glm
)

target_compile_definitions(GameModelViewer PRIVATE
    GAMEVIEWER_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders"
)

if(MSVC)
    target_compile_options(GameModelViewer PRIVATE /W4 /permissive-)
else()
    target_compile_options(GameModelViewer PRIVATE -Wall -Wextra -Wpedantic)
endif()
