#include "grassland/vulkan/image.h"

#include "grassland/vulkan/buffer.h"
#include "grassland/vulkan/single_time_command.h"

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
  if (image_ || image_view_) {
    vkDestroyImageView(device_->Handle(), image_view_, nullptr);
    vmaDestroyImage(device_->Allocator(), image_, allocation_);
  }
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

VkResult Image::Resize(VkExtent2D extent) {
  vkDestroyImageView(device_->Handle(), image_view_, nullptr);
  vmaDestroyImage(device_->Allocator(), image_, allocation_);

  Image *new_image = nullptr;
  RETURN_IF_FAILED_VK(device_->CreateImage(format_, extent, usage_, aspect_,
                                           sample_count_, &new_image),
                      "Failed to create resized image.");
  *this = *new_image;
  new_image->image_ = VK_NULL_HANDLE;
  new_image->image_view_ = VK_NULL_HANDLE;
  delete new_image;
  return VK_SUCCESS;
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

void UploadImage(Queue *queue,
                 CommandPool *command_pool,
                 Image *image,
                 const void *data,
                 VkDeviceSize size) {
  std::unique_ptr<Buffer> staging_buffer;
  image->Device()->CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VMA_MEMORY_USAGE_CPU_TO_GPU, &staging_buffer);
  void *staging_data = staging_buffer->Map();
  std::memcpy(staging_data, data, size);
  staging_buffer->Unmap();

  SingleTimeCommand(queue, command_pool, [&](VkCommandBuffer cmd_buffer) {
    TransitImageLayout(cmd_buffer, image->Handle(), VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                       VK_ACCESS_TRANSFER_WRITE_BIT, image->Aspect());
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = image->Aspect();
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {image->Width(), image->Height(), 1};
    vkCmdCopyBufferToImage(cmd_buffer, staging_buffer->Handle(),
                           image->Handle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    TransitImageLayout(
        cmd_buffer, image->Handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
        image->Aspect());
  });
}
}  // namespace grassland::vulkan
