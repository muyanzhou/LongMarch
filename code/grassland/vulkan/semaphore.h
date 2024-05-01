#pragma once

#include "grassland/vulkan/device.h"
#include "grassland/vulkan/vulkan_utils.h"

namespace grassland::vulkan {
class Semaphore {
 public:
  Semaphore(const class Device *device, VkSemaphore semaphore);

  ~Semaphore();

  [[nodiscard]] VkSemaphore Handle() const {
    return semaphore_;
  }

  [[nodiscard]] const class Device *Device() const {
    return device_;
  }

 private:
  const class Device *device_{};
  VkSemaphore semaphore_{};
};
}  // namespace grassland::vulkan
