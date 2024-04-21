#pragma once
#include "grassland/vulkan/device.h"

namespace grassland::vulkan {
class Buffer {
 public:
  Buffer(const class Device *device,
         VkDeviceSize size,
         VkBuffer buffer,
         VmaAllocation allocation);

  ~Buffer();

  [[nodiscard]] const class Device *Device() const {
    return device_;
  }

  [[nodiscard]] VkBuffer Handle() const {
    return buffer_;
  }

  [[nodiscard]] VmaAllocation Allocation() const {
    return allocation_;
  }

  [[nodiscard]] VkDeviceSize Size() const {
    return size_;
  }

  [[nodiscard]] void *Map() const {
    void *data;
    vmaMapMemory(device_->Allocator(), allocation_, &data);
    return data;
  }

  void Unmap() const {
    vmaUnmapMemory(device_->Allocator(), allocation_);
  }

 private:
  const class Device *device_{};
  VkDeviceSize size_{};
  VkBuffer buffer_{};
  VmaAllocation allocation_{};
};

void CopyBuffer(VkCommandBuffer command_buffer,
                Buffer *src_buffer,
                Buffer *dst_buffer,
                VkDeviceSize size);

void UploadBuffer(Queue *queue,
                  CommandPool *command_pool,
                  Buffer *buffer,
                  const void *data,
                  VkDeviceSize size);

void DownloadBuffer(Queue *queue,
                    CommandPool *command_pool,
                    Buffer *buffer,
                    void *data,
                    VkDeviceSize size);
}  // namespace grassland::vulkan
