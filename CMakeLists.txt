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

FetchContent_Declare(
        tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
        GIT_TAG        v2.0.0rc13
)
FetchContent_MakeAvailable(tinyobjloader)
set(GLSLC "C:/VulkanSDK/1.4.350.0/Bin/glslc.exe")

function(compile_shaders TARGET_NAME SHADER_DIR)
    file(GLOB_RECURSE SHADER_SOURCES
            ${SHADER_DIR}/*.vert
            ${SHADER_DIR}/*.frag
            ${SHADER_DIR}/*.comp
            ${SHADER_DIR}/*.geom
            ${SHADER_DIR}/*.tesc
            ${SHADER_DIR}/*.tese
    )
    set(SHADER_COMMANDS "")
    foreach(SHADER ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        list(APPEND SHADER_COMMANDS
                COMMAND ${GLSLC} ${SHADER} -o ${SHADER}.spv
                COMMAND ${CMAKE_COMMAND} -E echo "Compiled: ${SHADER_NAME}"
        )
    endforeach()
    add_custom_target(shaders ALL ${SHADER_COMMANDS})
    add_dependencies(${TARGET_NAME} shaders)
endfunction()


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
        Base/FlowishBuffer.h
        Base/FlowishDescriptor.cpp
        Base/FlowishDescriptor.h
        Base/FlowishImage.cpp
        Base/FlowishImage.h
        Utils/Utils.cpp
        Model/FlowishModel.cpp
        Model/FlowishModel.h
        Base/FlowishSampler.cpp
        Base/FlowishSampler.h
        3rdparty/stb/stb_image.h)
compile_shaders(flowish ${CMAKE_SOURCE_DIR}/Shader)

target_link_libraries(flowish PRIVATE Vulkan::Vulkan glfw glm::glm tinyobjloader)
target_include_directories(flowish PRIVATE ${Vulkan_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/3rdparty/stb)

