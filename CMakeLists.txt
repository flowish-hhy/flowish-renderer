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

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

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
        Base/FlowishSyncObjects.h
        Core/Vertex.h
        Base/FlowishBuffer.cpp
        Base/FlowishBuffer.h)

target_link_libraries(flowish PRIVATE Vulkan::Vulkan glfw glm::glm)
target_include_directories(flowish PRIVATE ${Vulkan_INCLUDE_DIRS})

find_program(GLSLC glslc HINTS ${Vulkan_GLSLC_EXECUTABLE} $ENV{VULKAN_SDK}/Bin)

file(GLOB SHADER_SOURCES
        ${CMAKE_SOURCE_DIR}/Shader/*.vert
        ${CMAKE_SOURCE_DIR}/Shader/*.frag
)

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPV ${CMAKE_SOURCE_DIR}/Shader/${SHADER_NAME}.spv)
    add_custom_command(
            OUTPUT ${SPV}
            COMMAND ${GLSLC} ${SHADER} -o ${SPV}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader: ${SHADER_NAME}"
    )
    list(APPEND SPV_FILES ${SPV})
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_FILES})
