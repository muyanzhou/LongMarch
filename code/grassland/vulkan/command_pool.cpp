#include "grassland/vulkan/command_pool.h"

#include "grassland/vulkan/command_buffer.h"

namespace grassland::vulkan {

CommandPool::CommandPool(const struct Device *device,
                         VkCommandPool command_pool)
    : device_(device), command_pool_(command_pool) {
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(device_->Handle(), command_pool_, nullptr);
}

VkResult CommandPool::AllocateCommandBuffer(
    VkCommandBufferLevel level,
    double_ptr<CommandBuffer> pp_command_buffer) {
  if (!pp_command_buffer) {
    SetErrorMessage("pp_command_buffer is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkCommandBufferAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = command_pool_;
  allocate_info.level = level;
  allocate_info.commandBufferCount = 1;
  VkCommandBuffer command_buffer;

  RETURN_IF_FAILED_VK(vkAllocateCommandBuffers(device_->Handle(),
                                               &allocate_info, &command_buffer),
                      "failed to allocate command buffer!");

  pp_command_buffer.construct(this, command_buffer);

  return VK_SUCCESS;
}

VkResult CommandPool::AllocateCommandBuffer(
    double_ptr<CommandBuffer> pp_command_buffer) {
  return AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                               pp_command_buffer);
}
}  // namespace grassland::vulkan
