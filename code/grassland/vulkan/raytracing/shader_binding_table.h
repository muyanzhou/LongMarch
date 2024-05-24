#pragma once
#include "grassland/vulkan/device.h"

namespace grassland::vulkan {

class ShaderBindingTable {
 public:
  explicit ShaderBindingTable(std::unique_ptr<Buffer> buffer,
                              VkDeviceAddress buffer_address,
                              VkDeviceAddress ray_gen_offset,
                              VkDeviceAddress miss_offset,
                              VkDeviceAddress closest_hit_offset);
  [[nodiscard]] VkDeviceAddress GetRayGenDeviceAddress() const;
  [[nodiscard]] VkDeviceAddress GetMissDeviceAddress() const;
  [[nodiscard]] VkDeviceAddress GetClosestHitDeviceAddress() const;

 private:
  std::unique_ptr<Buffer> buffer_;
  VkDeviceAddress buffer_address_;
  VkDeviceAddress ray_gen_offset_;
  VkDeviceAddress miss_offset_;
  VkDeviceAddress closest_hit_offset_;
};

}  // namespace grassland::vulkan
