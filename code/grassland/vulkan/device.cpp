#include "grassland/vulkan/device.h"

#include <utility>

namespace grassland::vulkan {
Device::Device(const class Instance *instance,
               const class PhysicalDevice &physical_device,
               DeviceCreateInfo create_info,
               VkDevice device)
    : instance_(instance),
      physical_device_(physical_device),
      create_info_(std::move(create_info)),
      device_(device) {
  VmaAllocatorCreateInfo allocator_info = {};
  allocator_info.physicalDevice = physical_device_.Handle();
  allocator_info.device = device_;
  allocator_info.instance = instance->Handle();
  allocator_info.vulkanApiVersion = instance_->CreateHint().app_info.apiVersion;
  //        vmaCreateAllocator(&allocator_info, &allocator_);
}

Device::~Device() {
  //        vmaDestroyAllocator(allocator_);
  vkDestroyDevice(device_, nullptr);
}
}  // namespace grassland::vulkan
