#pragma once

#include "grassland/vulkan/instance.h"

namespace grassland::vulkan {

// Physical Device
class PhysicalDevice {
 public:
  explicit PhysicalDevice(VkPhysicalDevice physical_device);

  ~PhysicalDevice();

  [[nodiscard]] VkPhysicalDevice Handle() const;

  // Functions that Return Key Features and Properties
  [[nodiscard]] VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const;

  [[nodiscard]] VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;

  [[nodiscard]] VkPhysicalDeviceMemoryProperties
  GetPhysicalDeviceMemoryProperties() const;

  [[nodiscard]] VkPhysicalDeviceRayTracingPipelinePropertiesKHR
  GetPhysicalDeviceRayTracingPipelineProperties() const;

  [[nodiscard]] VkPhysicalDeviceRayTracingPipelineFeaturesKHR
  GetPhysicalDeviceRayTracingPipelineFeatures() const;

  [[nodiscard]] uint64_t GetDeviceLocalMemorySize() const;

  [[nodiscard]] std::vector<VkExtensionProperties> GetDeviceExtensions() const;

  [[nodiscard]] std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties()
      const;

  bool IsExtensionSupported(const char *extension_name) const;

  [[nodiscard]] bool SupportGeometryShader() const;

  [[nodiscard]] bool SupportRayTracing() const;

  [[nodiscard]] uint64_t Evaluate() const;

  [[nodiscard]] uint32_t GraphicsFamilyIndex() const;

  [[nodiscard]] uint32_t PresentFamilyIndex(const Surface *surface) const;

  [[nodiscard]] uint32_t ComputeFamilyIndex() const;

  [[nodiscard]] uint32_t TransferFamilyIndex() const;

  [[nodiscard]] VkSampleCountFlagBits GetMaxUsableSampleCount() const;

  [[nodiscard]] bool CheckFeatureSupport(
      const struct DeviceFeatureRequirement &feature_requirement) const;

 private:
  VkPhysicalDevice physical_device_{};
};

}  // namespace grassland::vulkan
