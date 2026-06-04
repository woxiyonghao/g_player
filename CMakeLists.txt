cmake_minimum_required(VERSION 3.20)

project(g_player VERSION 1.0.0 LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/glfw)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)

if(NOT APP_SOURCES)
    message(FATAL_ERROR "No C++ source files were found under src/")
endif()

add_library(bx STATIC
    3rdparty/bx/src/amalgamated.cpp
)

target_include_directories(bx PUBLIC
    3rdparty/bx/include
    3rdparty/bx/3rdparty
)

if(APPLE)
    target_include_directories(bx PUBLIC 3rdparty/bx/include/compat/osx)
elseif(UNIX)
    target_include_directories(bx PUBLIC 3rdparty/bx/include/compat/linux)
endif()

target_compile_definitions(bx PUBLIC
    $<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>
    $<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>
)

add_library(bimg STATIC
    3rdparty/bimg/src/image.cpp
    3rdparty/bimg/src/image_cubemap_filter.cpp
    3rdparty/bimg/src/image_decode.cpp
    3rdparty/bimg/src/image_encode.cpp
    3rdparty/bimg/src/image_gnf.cpp
)

target_include_directories(bimg PUBLIC
    3rdparty/bimg/include
    3rdparty/bimg/3rdparty
    3rdparty/bimg/3rdparty/astc-encoder/include
    3rdparty/bimg/3rdparty/iqa/include
    3rdparty/bimg/3rdparty/tinyexr/deps
)

target_link_libraries(bimg PUBLIC bx)

add_library(bgfx STATIC
    3rdparty/bgfx/src/amalgamated.cpp
)

target_include_directories(bgfx PUBLIC
    3rdparty/bgfx/include
    3rdparty/bgfx/3rdparty
    3rdparty/bgfx/3rdparty/khronos
    3rdparty/bgfx/3rdparty/dawn/include
    3rdparty/bgfx/3rdparty/directx-headers/include
    3rdparty/bgfx/3rdparty/spirv-cross
    3rdparty/bgfx/3rdparty/spirv-cross/include
    3rdparty/bgfx/3rdparty/spirv-headers/include
    3rdparty/bgfx/3rdparty/spirv-tools/include
)

target_link_libraries(bgfx PUBLIC
    bx
    bimg
)

target_compile_definitions(bgfx PUBLIC
    BGFX_CONFIG_MULTITHREADED=1
)

add_library(imgui STATIC
    3rdparty/imgui/imgui.cpp
    3rdparty/imgui/imgui_draw.cpp
    3rdparty/imgui/imgui_tables.cpp
    3rdparty/imgui/imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC
    3rdparty/imgui
)

add_library(imgui_glfw STATIC
    3rdparty/imgui/backends/imgui_impl_glfw.cpp
)

target_include_directories(imgui_glfw PUBLIC
    3rdparty/imgui
    3rdparty/imgui/backends
)

target_link_libraries(imgui_glfw PUBLIC
    imgui
    glfw
)

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
        include/window/window.h
        src/window/window.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    bgfx
    imgui_glfw
)

if(APPLE)
    foreach(target glfw bx bimg bgfx imgui_glfw ${PROJECT_NAME})
        target_compile_definitions(${target} PRIVATE
            TARGET_OS_OSX=1
        )
    endforeach()

    target_compile_definitions(bgfx PUBLIC
        BGFX_CONFIG_RENDERER_METAL=1
    )

    target_link_libraries(bgfx PUBLIC
        "-framework Cocoa"
        "-framework Foundation"
        "-framework IOKit"
        "-framework Metal"
        "-framework QuartzCore"
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(bgfx PUBLIC
        dl
        pthread
    )
endif()
