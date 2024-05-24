#include "grassland/vulkan/image.h"

namespace grassland::vulkan {
Image::Image(const class Device *device,
             VkFormat format,
             VkExtent2D extent,
             VkImageUsageFlags usage,
             VkImageAspectFlags aspect,
             VkSampleCountFlagBits sample_count,
             VkImage image,
             VkImageView image_view,
             VmaAllocation allocation)
    : device_(device),
      format_(format),
      extent_(extent),
      usage_(usage),
      aspect_(aspect),
      sample_count_(sample_count),
      image_(image),
      image_view_(image_view),
      allocation_(allocation) {
}

Image::~Image() {
  vkDestroyImageView(device_->Handle(), image_view_, nullptr);
  vmaDestroyImage(device_->Allocator(), image_, allocation_);
}

void Image::ClearColor(VkCommandBuffer command_buffer,
                       VkClearColorValue clear_color) {
  VkImageSubresourceRange subresource_range{};
  subresource_range.aspectMask = aspect_;
  subresource_range.baseMipLevel = 0;
  subresource_range.levelCount = 1;
  subresource_range.baseArrayLayer = 0;
  subresource_range.layerCount = 1;

  TransitImageLayout(
      command_buffer, image_, VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, aspect_);

  vkCmdClearColorImage(command_buffer, image_,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1,
                       &subresource_range);
}

void TransitImageLayout(VkCommandBuffer command_buffer,
                        VkImage image,
                        VkImageLayout old_layout,
                        VkImageLayout new_layout,
                        VkPipelineStageFlags src_stage_flags,
                        VkPipelineStageFlags dst_stage_flags,
                        VkAccessFlags src_access_flags,
                        VkAccessFlags dst_access_flags,
                        VkImageAspectFlags aspect) {
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = aspect;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
  barrier.srcAccessMask = src_access_flags;
  barrier.dstAccessMask = dst_access_flags;

  vkCmdPipelineBarrier(command_buffer, src_stage_flags, dst_stage_flags, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
}
}  // namespace grassland::vulkan
