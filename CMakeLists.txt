cmake_minimum_required(VERSION 3.24)

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING ""
)
set(VCPKG_MANIFEST_MODE ON)

project(ShaderProcessor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    "src/example.cpp"
    "src/shader_processor.h")

find_package(bgfx CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    bgfx::bx
    bgfx::bgfx
    bgfx::bimg
    bgfx::bimg_decode)

# Shader root folder
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADER_ROOT_PATH="${CMAKE_SOURCE_DIR}/shaders")
# Shader binary folder
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADER_BIN_PATH="${CMAKE_SOURCE_DIR}/build/shaders")
# Shader tool folder
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADER_TOOL_PATH="${CMAKE_SOURCE_DIR}/tools")