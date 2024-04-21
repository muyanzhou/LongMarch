#pragma once

#include "grassland/vulkan/device_creation_assist.h"
#include "grassland/vulkan/device_procedures.h"
#include "grassland/vulkan/instance.h"
#include "grassland/vulkan/physical_device.h"

namespace grassland::vulkan {

class Device {
 public:
  Device(const class Instance *instance,
         const class PhysicalDevice &physical_device,
         DeviceCreateInfo create_info,
         VkDevice device);

  ~Device();

  [[nodiscard]] VkDevice Handle() const {
    return device_;
  }

  [[nodiscard]] const class Instance *Instance() const {
    return instance_;
  }

  [[nodiscard]] const class PhysicalDevice &PhysicalDevice() const {
    return physical_device_;
  }

  [[nodiscard]] const DeviceCreateInfo &CreateInfo() const {
    return create_info_;
  }

  [[nodiscard]] DeviceProcedures &Procedures() {
    return procedures_;
  }

  [[nodiscard]] const DeviceProcedures &Procedures() const {
    return procedures_;
  }

  [[nodiscard]] VmaAllocator Allocator() const {
    return allocator_;
  }

  [[nodiscard]] VkResult WaitIdle() const {
    return vkDeviceWaitIdle(device_);
  }

  VkResult GetQueue(uint32_t queue_family_index,
                    int queue_index,
                    double_ptr<Queue> pp_queue) const;

  [[nodiscard]] VkResult CreateSwapchain(
      const Surface *surface,
      double_ptr<Swapchain> pp_swapchain) const;

  [[nodiscard]] VkResult CreateSemaphore(
      double_ptr<Semaphore> pp_semaphore) const;

  [[nodiscard]] VkResult CreateFence(bool signaled,
                                     double_ptr<Fence> pp_fence) const;

  [[nodiscard]] VkResult CreateCommandPool(
      uint32_t queue_family_index,
      VkCommandPoolCreateFlags flags,
      double_ptr<CommandPool> pp_command_pool) const;

  [[nodiscard]] VkResult CreateCommandPool(
      double_ptr<CommandPool> pp_command_pool) const;

  [[nodiscard]] VkResult CreateShaderModule(
      const std::vector<uint32_t> &code,
      double_ptr<ShaderModule> pp_shader_module) const;

 private:
  const class Instance *instance_{};

  class PhysicalDevice physical_device_;

  const DeviceCreateInfo create_info_;

  VkDevice device_{};

  DeviceProcedures procedures_{};

  VmaAllocator allocator_{};
};

}  // namespace grassland::vulkan
