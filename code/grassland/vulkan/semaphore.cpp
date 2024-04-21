#include "grassland/vulkan/semaphore.h"

namespace grassland::vulkan {
Semaphore::Semaphore(const struct Device *device, VkSemaphore semaphore)
    : device_(device), semaphore_(semaphore) {
}

Semaphore::~Semaphore() {
  vkDestroySemaphore(device_->Handle(), semaphore_, nullptr);
}
}  // namespace grassland::vulkan
