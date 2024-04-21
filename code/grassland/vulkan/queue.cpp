#include "grassland/vulkan/queue.h"

#include "grassland/vulkan/device.h"

namespace grassland::vulkan {
Queue::Queue(struct Device *device,
             uint32_t queue_family_index,
             uint32_t queue_index)
    : device_(device), queue_family_index_(queue_family_index) {
  if (device) {
    vkGetDeviceQueue(device->Handle(), queue_family_index, queue_index,
                     &queue_);
  }
}

VkQueue Queue::Handle() const {
  return queue_;
}

VkResult Queue::WaitIdle() const {
  return vkQueueWaitIdle(queue_);
}
}  // namespace grassland::vulkan
