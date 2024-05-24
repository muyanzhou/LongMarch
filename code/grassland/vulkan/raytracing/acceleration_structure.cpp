#include "grassland/vulkan/raytracing/acceleration_structure.h"

namespace grassland::vulkan {

AccelerationStructure::AccelerationStructure(
    const struct Device *device,
    std::unique_ptr<class Buffer> buffer,
    VkDeviceAddress device_address,
    VkAccelerationStructureKHR as)
    : device_(device),
      buffer_(std::move(buffer)),
      device_address_(device_address),
      as_(as) {
}

AccelerationStructure::~AccelerationStructure() {
  device_->Procedures().vkDestroyAccelerationStructureKHR(device_->Handle(),
                                                          as_, nullptr);
}

class Buffer *AccelerationStructure::Buffer() const {
  return buffer_.get();
}

VkDeviceAddress AccelerationStructure::DeviceAddress() const {
  return device_address_;
}

}  // namespace grassland::vulkan
