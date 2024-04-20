#pragma once
#include "grassland/vulkan/instance.h"

namespace grassland::vulkan {
class Device {
 public:
  Device(class Instance *instance, VkDevice device);

  ~Device();

  [[nodiscard]] VkDevice Handle() const {
    return device_;
  }

  [[nodiscard]] class Instance *Instance() const {
    return instance_;
  }

 private:
  class Instance *instance_;
  VkDevice device_;
};
}  // namespace grassland::vulkan
