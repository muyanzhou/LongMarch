#pragma once
#include "grassland/vulkan/instance.h"

namespace grassland::vulkan {
class Queue {
 public:
  explicit Queue(const class Device *device,
                 uint32_t queue_family_index,
                 VkQueue queue);

  [[nodiscard]] VkQueue Handle() const;

  [[nodiscard]] const Device *Device() const {
    return device_;
  }
  [[nodiscard]] uint32_t QueueFamilyIndex() const {
    return queue_family_index_;
  }

  [[nodiscard]] VkResult WaitIdle() const;

 private:
  const class Device *device_{};
  uint32_t queue_family_index_{};
  VkQueue queue_{};
};
}  // namespace grassland::vulkan
