#pragma once

#include "grassland/vulkan/instance_procedures.h"
#include "grassland/vulkan/vulkan_utils.h"

namespace grassland::vulkan {

struct InstanceCreateInfo {
  bool enable_validation_layers{kDefaultEnableValidationLayers};

  std::vector<const char *> extensions{};

  VkApplicationInfo app_info{};

  explicit InstanceCreateInfo(bool surface_support = true);

  void SetValidationLayersEnabled(
      bool enabled = kDefaultEnableValidationLayers);

  void AddExtension(const char *extension);

  void ApplyGLFWSurfaceSupport();
};

class Instance {
 public:
  explicit Instance(InstanceCreateInfo create_info = InstanceCreateInfo(false));

  ~Instance();

  [[nodiscard]] VkInstance Handle() const {
    return instance_;
  }

  [[nodiscard]] const InstanceCreateInfo &Settings() const {
    return create_info_;
  }

 private:
  InstanceCreateInfo create_info_;

  VkInstance instance_{};
  VkDebugUtilsMessengerEXT debug_messenger_{};
  InstanceProcedures instance_procedures_{};
};
}  // namespace grassland::vulkan
