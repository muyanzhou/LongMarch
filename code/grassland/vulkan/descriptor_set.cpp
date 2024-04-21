#include "grassland/vulkan/descriptor_set.h"

namespace grassland::vulkan {
DescriptorSet::DescriptorSet(const struct DescriptorPool *descriptor_pool,
                             VkDescriptorSet set)
    : descriptor_pool_(descriptor_pool), set_(set) {
}

DescriptorSet::~DescriptorSet() {
  vkFreeDescriptorSets(descriptor_pool_->Device()->Handle(),
                       descriptor_pool_->Handle(), 1, &set_);
}
}  // namespace grassland::vulkan
