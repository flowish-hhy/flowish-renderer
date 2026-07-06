#include <iostream>
#include <GLFW/glfw3.h>

#include "Base/FlowishBuffer.h"
#include "Core/Vertex.h"
#include "Base/FlowishCommandPool.h"
#include "Base/FlowishDevice.h"
#include "Base/FlowishFrameBuffer.h"
#include "Base/FlowishInstance.h"
#include "Base/FlowishPipeline.h"
#include "Base/FlowishRenderPass.h"
#include "Base/FlowishShaderModule.h"
#include "Base/FlowishSurface.h"
#include "Base/FlowishSwapchain.h"
#include "Base/FlowishSyncObjects.h"
#include "Utils/Utils.h"

void recordCommandBuffer(
    VkCommandBuffer commandBuffer,
    VkRenderPass renderPass,
    VkFramebuffer framebuffer,
    VkExtent2D extent,
    VkPipeline pipeline,
    VkBuffer vertexBuffer,
    VkBuffer indexBuffer) {
    vkResetCommandBuffer(commandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkClearValue clearColor = {};
    clearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent = extent;
    scissor.offset = {0, 0};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vbs[] = {vertexBuffer};
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0 , 1, vbs, offsets);
    vkCmdBindIndexBuffer(commandBuffer,indexBuffer,0,VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void drawFrame(
    VkDevice device,
    VkSwapchainKHR swapchain,
    VkQueue graphicsQueue,
    VkQueue presentQueue,
    VkCommandBuffer commandBuffer,
    VkRenderPass renderPass,
    FlowishFramebuffers& framebuffers,
    VkExtent2D extent,
    VkPipeline pipeline,
    FlowishSyncObjects& sync,
    VkBuffer vertexBuffer,
    VkBuffer indexBuffer) {

    VkFence fence = sync.getFence();
    vkWaitForFences(device, 1, &fence,VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fence);

    uint32_t imageIndex = 0;
    vkAcquireNextImageKHR(device,swapchain,UINT64_MAX,sync.imageAvailable(),VK_NULL_HANDLE,&imageIndex);

    recordCommandBuffer(commandBuffer,renderPass,framebuffers.get(imageIndex),extent,pipeline,vertexBuffer,indexBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSems[] = {sync.imageAvailable()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_ALL_COMMANDS_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSems[0];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;


    VkSemaphore signalSems[] = {sync.renderFinished()};
    submitInfo.pSignalSemaphores = &signalSems[0];
    submitInfo.signalSemaphoreCount = 1;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffer command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSems[0];
    VkSwapchainKHR swapchains[] = {swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    vkQueuePresentKHR(presentQueue, &presentInfo);


}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow *window = glfwCreateWindow(1080, 720, "flowish", NULL, NULL);

    FlowishInstance instance;
    FlowishSurface surface(instance.handle(),window);
    FlowishDevice device(instance.handle(),surface.handle());
    FlowishSwapchain swapchain(device.physicalDevice(),device.device(),surface.handle(),window,device.queueFamilyIndices());
    FlowishRenderPass renderpass(device.device(), swapchain.format());
    FlowishCommandPool commandPool(device.device(), device.queueFamilyIndices());

    std::vector<Vertex> vertices = {
        {{-0.5f , -0.5f } , {1, 0 ,0}},
        {{0.5f , -0.5f } , {0, 1 ,0}},
        {{0.5f , 0.5f } , {0, 0 ,1}},
        {{-0.5f , 0.5f } , {1, 1 ,1}},
    };
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
    FlowishBuffer staging(device.physicalDevice(),device.device(),bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void * data;
    vkMapMemory(device.device(), staging.memory(), 0, bufferSize, 0, &data);
    memcpy(data, &vertices[0], bufferSize);
    vkUnmapMemory(device.device(), staging.memory());

    FlowishBuffer buffer(device.physicalDevice(),device.device(),bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    copyBuffer(device.device(), commandPool.handle(), device.graphicsQueue(),
        staging.handle(),buffer.handle(), bufferSize );



    std::vector<uint32_t> indices = {0 , 1, 2, 2 ,3 ,0};
    VkDeviceSize indexSize = sizeof(uint32_t) * 6;
    FlowishBuffer stagingIndexBuffer(device.physicalDevice(),device.device(),indexSize,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkMapMemory(device.device(), stagingIndexBuffer.memory(), 0, indexSize, 0, &data);
    memcpy(data, &indices[0], indexSize);
    vkUnmapMemory(device.device(), stagingIndexBuffer.memory());

    FlowishBuffer indexBuffer(device.physicalDevice(),device.device(),indexSize,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    copyBuffer(device.device(), commandPool.handle(), device.graphicsQueue(),
        stagingIndexBuffer.handle(),indexBuffer.handle(), indexSize );


    auto vertCode = readFile("Shader/triangle.vert.spv");
    auto fragCode = readFile("Shader/triangle.frag.spv");
    FlowishShaderModule vertShader(device.device(), vertCode);
    FlowishShaderModule fragShader(device.device(), fragCode);
    FlowishPipeline pipeline(device.device(),renderpass.handle(), vertShader.handle(), fragShader.handle());
    FlowishFramebuffers framebuffers(device.device(), renderpass.handle(), swapchain.imageViews(), swapchain.extent());
    FlowishSyncObjects sync(device.device());
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame(device.device(), swapchain.handle(), device.graphicsQueue(), device.presentQueue()
            , commandPool.commandBuffer(), renderpass.handle(), framebuffers, swapchain.extent(),
            pipeline.handle(), sync, buffer.handle(),indexBuffer.handle());

    }

    vkDeviceWaitIdle(device.device());


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
