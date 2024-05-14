#pragma once

#include "grassland/vulkan/descriptor_pool.h"
#include "grassland/vulkan/descriptor_set_layout.h"

namespace grassland::vulkan {
class DescriptorSet {
 public:
  DescriptorSet(const class DescriptorPool *descriptor_pool,
                VkDescriptorSet set);

  ~DescriptorSet();

  VkDescriptorSet Handle() const {
    return set_;
  }

  const class DescriptorPool *Pool() const {
    return descriptor_pool_;
  }

 private:
  const class DescriptorPool *descriptor_pool_{};
  VkDescriptorSet set_{};
};
}  // namespace grassland::vulkan
