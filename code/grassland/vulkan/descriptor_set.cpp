#include "grassland/vulkan/descriptor_set.h"

#include "grassland/vulkan/buffer.h"
#include "grassland/vulkan/image.h"

namespace grassland::vulkan {
DescriptorSet::DescriptorSet(const struct DescriptorPool *descriptor_pool,
                             VkDescriptorSet set)
    : descriptor_pool_(descriptor_pool), set_(set) {
}

DescriptorSet::~DescriptorSet() {
  vkFreeDescriptorSets(descriptor_pool_->Device()->Handle(),
                       descriptor_pool_->Handle(), 1, &set_);
}

void DescriptorSet::BindUniformBuffer(uint32_t binding,
                                      const struct Buffer *buffer,
                                      VkDeviceSize offset,
                                      VkDeviceSize range) const {
  VkDescriptorBufferInfo buffer_info{};
  buffer_info.buffer = buffer->Handle();
  buffer_info.offset = offset;
  buffer_info.range = range ? range : buffer->Size();

  VkWriteDescriptorSet write_descriptor_set{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.dstSet = set_;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.pBufferInfo = &buffer_info;

  vkUpdateDescriptorSets(descriptor_pool_->Device()->Handle(), 1,
                         &write_descriptor_set, 0, nullptr);
}

void DescriptorSet::BindCombinedImageSampler(uint32_t binding,
                                             const struct Image *image,
                                             VkSampler sampler) const {
  VkDescriptorImageInfo image_info{};
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = image->ImageView();
  image_info.sampler = sampler;

  VkWriteDescriptorSet write_descriptor_set{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.dstSet = set_;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.pImageInfo = &image_info;

  vkUpdateDescriptorSets(descriptor_pool_->Device()->Handle(), 1,
                         &write_descriptor_set, 0, nullptr);
}
}  // namespace grassland::vulkan
