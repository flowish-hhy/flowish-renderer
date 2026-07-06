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

VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool pool) {
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
    return singleCommandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer singleCommandBuffer,VkDevice device, VkCommandPool pool, VkQueue queue) {
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

void copyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

    auto singleCommandBuffer = beginSingleTimeCommands(device, pool);
    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size,
    };
    vkCmdCopyBuffer(singleCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    endSingleTimeCommands(singleCommandBuffer,device,pool,queue);
}


void copyBufferToImage(VkDevice device, VkCommandPool pool, VkQueue queue, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height) {

    auto singleCommandBuffer = beginSingleTimeCommands(device, pool);
    VkBufferImageCopy copyRegion = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {0,0,0},
        .imageExtent = {width, height, 1},
    };
    vkCmdCopyBufferToImage(singleCommandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
    endSingleTimeCommands(singleCommandBuffer,device,pool,queue);

}

void transitionImageLayout(VkDevice device, VkCommandPool pool, VkQueue queue, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer cmd = beginSingleTimeCommands(device, pool);

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };
    VkPipelineStageFlags srcStage, dstStage;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::runtime_error("Invalid layout transition!");
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &barrier);

    endSingleTimeCommands(cmd,device,pool,queue);


}
