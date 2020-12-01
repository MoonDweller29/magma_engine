#include "magma/vk/commandBuffer.h"
#include "magma/vk/vulkan_common.h"
#include "magma/vk/graphicsPipeline.h"
#include <array>



CommandBufferArr::CommandBufferArr(VkDevice device, VkCommandPool commandPool, uint32_t count)
{
    allocate(device, commandPool, count);
}

void CommandBufferArr::allocate(VkDevice device, VkCommandPool commandPool, uint32_t count)
{
    commandBuffers.resize(count);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
    vk_check_err(result, "failed to allocate command buffers!");
}

VkCommandBuffer CommandBufferArr::beginCmdBuf(uint32_t i)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    //resets command buffer
    VkResult result = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
    vk_check_err(result, "failed to begin recording command buffer!");

    return commandBuffers.at(i);
}

void CommandBufferArr::endCmdBuf(uint32_t i)
{
    VkResult result = vkEndCommandBuffer(commandBuffers[i]);
    vk_check_err(result, "failed to record command buffer!");
}

//void CommandBufferArr::record(
//        VkBuffer indexBuffer,
//        VkBuffer vertexBuffer,
//        uint32_t vertexCount,
//        const std::vector<VkDescriptorSet> &descriptorSets,
//        VkRenderPass renderPass,
//        VkExtent2D extent,
//        const std::vector<VkFramebuffer> &frameBuffers,
//        const GraphicsPipeline &graphicsPipeline)
//{
//    for (size_t i = 0; i < commandBuffers.size(); i++)
//    {
//        VkCommandBufferBeginInfo beginInfo{};
//        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        beginInfo.flags = 0; // Optional
//        beginInfo.pInheritanceInfo = nullptr; // Optional
//
//        //resets command buffer
//        VkResult result = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
//        vk_check_err(result, "failed to begin recording command buffer!");
//        {
//            VkRenderPassBeginInfo renderPassInfo{};
//            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//            renderPassInfo.renderPass = renderPass;
//            renderPassInfo.framebuffer = frameBuffers[i];
//
//            renderPassInfo.renderArea.offset = {0, 0};
//            renderPassInfo.renderArea.extent = extent;
//
//            std::array<VkClearValue, 2> clearValues{};
//            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
//            clearValues[1].depthStencil = {1.0f, 0};
//            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
//            renderPassInfo.pClearValues = clearValues.data();
//
//            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//            {
//                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getHandler());
//
//                VkBuffer vertexBuffers[] = {vertexBuffer};
//                VkDeviceSize offsets[] = {0};
//                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
//                vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
//                vkCmdBindDescriptorSets(commandBuffers[i],
//                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
//                                        graphicsPipeline.getPipelineLayout(), 0, 1, &descriptorSets[i], 0, nullptr);
//
//                vkCmdDrawIndexed(commandBuffers[i], vertexCount, 1, 0, 0, 0);
//            }
//            vkCmdEndRenderPass(commandBuffers[i]);
//        }
//        result = vkEndCommandBuffer(commandBuffers[i]);
//        vk_check_err(result, "failed to record command buffer!");
//    }
//}

const VkCommandBuffer &CommandBufferArr::operator[] (const size_t index)
{
    return commandBuffers.at(index);
}


CommandBufferArr::~CommandBufferArr()
{

}



SingleTimeCommandBuffer::SingleTimeCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue):
    device(device), commandPool(commandPool), queue(queue)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    beginInfo={};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
}

const VkCommandBuffer &SingleTimeCommandBuffer::startRecording()
{
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void SingleTimeCommandBuffer::endRecordingAndSubmit()
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
}

SingleTimeCommandBuffer::~SingleTimeCommandBuffer()
{
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
