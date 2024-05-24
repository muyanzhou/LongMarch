#include "grassland/vulkan/raytracing/shader_binding_table.h"

#include "grassland/vulkan/buffer.h"

namespace grassland::vulkan {

ShaderBindingTable::ShaderBindingTable(std::unique_ptr<Buffer> buffer,
                                       VkDeviceAddress buffer_address,
                                       VkDeviceAddress ray_gen_offset,
                                       VkDeviceAddress miss_offset,
                                       VkDeviceAddress closest_hit_offset)
    : buffer_(std::move(buffer)),
      buffer_address_(buffer_address),
      ray_gen_offset_(ray_gen_offset),
      miss_offset_(miss_offset),
      closest_hit_offset_(closest_hit_offset) {
}

VkDeviceAddress ShaderBindingTable::GetRayGenDeviceAddress() const {
  return buffer_address_ + ray_gen_offset_;
}

VkDeviceAddress ShaderBindingTable::GetMissDeviceAddress() const {
  return buffer_address_ + miss_offset_;
}

VkDeviceAddress ShaderBindingTable::GetClosestHitDeviceAddress() const {
  return buffer_address_ + closest_hit_offset_;
}

}  // namespace grassland::vulkan
