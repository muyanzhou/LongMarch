#pragma once

#include "grassland/vulkan/device.h"

namespace grassland::vulkan {
class Image {
 public:
  Image(const class Device *device,
        VkFormat format,
        VkExtent2D extent,
        VkImageUsageFlags usage,
        VkImageAspectFlags aspect,
        VkSampleCountFlagBits sample_count,
        VkImage image,
        VkImageView image_view,
        VmaAllocation allocation);
  ~Image();

  [[nodiscard]] const Device *Device() const {
    return device_;
  }

  [[nodiscard]] VkImage Handle() const {
    return image_;
  }

  [[nodiscard]] VkImageView ImageView() const {
    return image_view_;
  }

  [[nodiscard]] VkFormat Format() const {
    return format_;
  }

  [[nodiscard]] VkExtent2D Extent() const {
    return extent_;
  }

  [[nodiscard]] VkImageUsageFlags Usage() const {
    return usage_;
  }

  [[nodiscard]] VkImageAspectFlags Aspect() const {
    return aspect_;
  }

  [[nodiscard]] VkSampleCountFlagBits SampleCount() const {
    return sample_count_;
  }

 private:
  const class Device *device_{};
  VkFormat format_{};
  VkExtent2D extent_{};
  VkImageUsageFlags usage_{};
  VkImageAspectFlags aspect_{};
  VkSampleCountFlagBits sample_count_{};
  VkImage image_{};
  VkImageView image_view_{};
  VmaAllocation allocation_{};
};

void TransitImageLayout(VkCommandBuffer command_buffer,
                        VkImage image,
                        VkImageLayout old_layout,
                        VkImageLayout new_layout,
                        VkPipelineStageFlags src_stage_flags,
                        VkPipelineStageFlags dst_stage_flags,
                        VkAccessFlags src_access_flags,
                        VkAccessFlags dst_access_flags,
                        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
}  // namespace grassland::vulkan
