#include "commandBuffer.h"
#include "vulkan_common.h"
#include "graphicsPipeline.h"



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

void CommandBufferArr::record(
        VkBuffer indexBuffer,
        VkBuffer vertexBuffer,
        uint32_t vertexCount,
        const std::vector<VkDescriptorSet> &descriptorSets,
        VkRenderPass renderPass,
        VkExtent2D extent,
        const std::vector<VkFramebuffer> &frameBuffers,
        const GraphicsPipeline &graphicsPipeline)
{
    for (size_t i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        //resets command buffer
        VkResult result = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
        vk_check_err(result, "failed to begin recording command buffer!");
        {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = frameBuffers[i];

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = extent;

            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            {
                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getHandler());

                VkBuffer vertexBuffers[] = {vertexBuffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
                vkCmdBindDescriptorSets(commandBuffers[i],
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        graphicsPipeline.getPipelineLayout(), 0, 1, &descriptorSets[i], 0, nullptr);

                vkCmdDrawIndexed(commandBuffers[i], vertexCount, 1, 0, 0, 0);
            }
            vkCmdEndRenderPass(commandBuffers[i]);
        }
        result = vkEndCommandBuffer(commandBuffers[i]);
        vk_check_err(result, "failed to record command buffer!");
    }
}

const VkCommandBuffer &CommandBufferArr::operator[] (const size_t index)
{
    return commandBuffers.at(index);
}


CommandBufferArr::~CommandBufferArr()
{

}
