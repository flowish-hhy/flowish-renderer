#include "Utils.h"

uint32_t findMemoryType(VkPhysicalDevice _physicalDevice, uint32_t typeBits, VkMemoryPropertyFlags properties)
{
     VkPhysicalDeviceMemoryProperties memProperties;
     vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);
     for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
         bool allowByBuffer = (typeBits & (1 << i));
         bool hasProperties = (memProperties.memoryTypes[i].propertyFlags & properties) == properties;
         if (allowByBuffer && hasProperties) {
             return i;
         }
     }
     throw std::runtime_error("failed to find suitable memory type");
 }


void copyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkCommandBuffer singleCommandBuffer;
    vkAllocateCommandBuffers(device,&allocInfo,&singleCommandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(singleCommandBuffer, &beginInfo);
    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size,
    };
    vkCmdCopyBuffer(singleCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(singleCommandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &singleCommandBuffer,
    };

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device,pool,1, &singleCommandBuffer);
}
