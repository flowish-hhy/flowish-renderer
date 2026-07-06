cmake_minimum_required(VERSION 3.20)
project(flowish)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

if(MSVC)
    add_compile_options($<$<CONFIG:Debug>:/Zi>)
    add_link_options($<$<CONFIG:Debug>:/DEBUG>)
endif()

add_executable(flowish main.cpp
        Base/FlowishInstance.cpp
        Base/FlowishInstance.h
        Base/FlowishSurface.cpp
        Base/FlowishSurface.h
        Base/FlowishDevice.cpp
        Base/FlowishDevice.h
        Base/FlowishSwapchain.cpp
        Base/FlowishSwapchain.h
        Base/FlowishRenderPass.cpp
        Base/FlowishRenderPass.h
        Base/FlowishShaderModule.cpp
        Base/FlowishShaderModule.h
        Utils/Utils.h
        Base/FlowishPipeline.cpp
        Base/FlowishPipeline.h
        Base/FlowishFrameBuffer.cpp
        Base/FlowishFrameBuffer.h
        Base/FlowishCommandPool.cpp
        Base/FlowishCommandPool.h
        Base/FlowishSyncObjects.cpp
        Base/FlowishSyncObjects.h)

target_link_libraries(flowish PRIVATE Vulkan::Vulkan glfw)
target_include_directories(flowish PRIVATE ${Vulkan_INCLUDE_DIRS})
