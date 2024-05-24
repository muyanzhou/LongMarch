#pragma once

#include "grassland/vulkan/buffer.h"
#include "grassland/vulkan/core/core_object.h"

namespace grassland::vulkan {
template <class Type = uint8_t>
struct StaticBuffer : public BufferObject {
 public:
  StaticBuffer() = default;

  StaticBuffer(class Core *core,
               size_t length = 1,
               VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
    Init(core, length, usage);
  }

  VkResult Init(class Core *core,
                size_t length = 1,
                VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
    core_ = core;
    if (!buffer_) {
      length_ = length;
      RETURN_IF_FAILED_VK(core_->Device()->CreateBuffer(
                              static_cast<VkDeviceSize>(sizeof(Type) * length),
                              usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, &buffer_),
                          "Failed to create buffer");
    }
    return VK_SUCCESS;
  }

  VkResult UploadContents(const Type *contents,
                          size_t length = 1,
                          size_t offset = 0) const {
    return UploadData(reinterpret_cast<const uint8_t *>(contents),
                      sizeof(Type) * length, sizeof(Type) * offset);
  }

  VkResult UploadData(const uint8_t *data,
                      size_t size,
                      size_t offset = 0) const {
    // Upload with staging buffer
    std::unique_ptr<Buffer> staging_buffer;
    RETURN_IF_FAILED_VK(core_->Device()->CreateBuffer(
                            size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VMA_MEMORY_USAGE_CPU_TO_GPU, &staging_buffer),
                        "Failed to create staging buffer");
    memcpy(staging_buffer->Map(), data, size);
    staging_buffer->Unmap();
    core_->TransferCommandPool()->SingleTimeCommands(
        core_->TransferQueue(), [&](VkCommandBuffer cmd_buffer) {
          VkBufferCopy copy_region{};
          copy_region.size = size;
          copy_region.dstOffset = offset;
          vkCmdCopyBuffer(cmd_buffer, staging_buffer->Handle(),
                          buffer_->Handle(), 1, &copy_region);
        });
    return VK_SUCCESS;
  }

  VkResult DownloadContents(Type *contents,
                            size_t length = 1,
                            size_t offset = 0) const {
    return DownloadData(reinterpret_cast<uint8_t *>(contents),
                        sizeof(Type) * length, sizeof(Type) * offset);
  }

  VkResult DownloadData(uint8_t *data, size_t size, size_t offset = 0) const {
    // Download with staging buffer
    std::unique_ptr<Buffer> staging_buffer;

    RETURN_IF_FAILED_VK(
        core_->Device()->CreateBuffer(
            size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU,
            VMA_ALLOCATION_CREATE_MAPPED_BIT |
                VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
            &staging_buffer),
        "Failed to create staging buffer");

    core_->TransferCommandPool()->SingleTimeCommands(
        core_->TransferQueue(), [&](VkCommandBuffer cmd_buffer) {
          VkBufferCopy copy_region{};
          copy_region.size = size;
          copy_region.srcOffset = offset;
          vkCmdCopyBuffer(cmd_buffer, buffer_->Handle(),
                          staging_buffer->Handle(), 1, &copy_region);
        });
    memcpy(data, staging_buffer->Map(), size);
    staging_buffer->Unmap();
    return VK_SUCCESS;
  }

  [[nodiscard]] Buffer *GetBuffer() const {
    return buffer_.get();
  }

  [[nodiscard]] Buffer *GetBuffer(int frame_index) const override {
    return GetBuffer();
  }

  size_t Length() const {
    return length_;
  }

  VkDeviceSize Size() const {
    return buffer_->Size();
  }

  class Core *Core() const {
    return core_;
  }

 private:
  class Core *core_;
  std::unique_ptr<class Buffer> buffer_{};
  size_t length_;
};

template <class Type>
VkResult Core::CreateStaticBuffer(size_t length,
                                  VkBufferUsageFlags usage,
                                  double_ptr<StaticBuffer<Type>> pp_buffer) {
  pp_buffer.construct();
  RETURN_IF_FAILED_VK(pp_buffer->Init(this, length, usage),
                      "Failed to create static buffer");
  return VK_SUCCESS;
}

}  // namespace grassland::vulkan
