#pragma once

#include "grassland/vulkan/instance_procedures.h"
#include "grassland/vulkan/surface.h"
#include "grassland/vulkan/vulkan_utils.h"

namespace grassland::vulkan {

struct InstanceCreateInfo {
  bool enable_validation_layers{kDefaultEnableValidationLayers};

  std::vector<const char *> extensions{};

  VkApplicationInfo app_info{};

  bool glfw_surface_support{false};

  explicit InstanceCreateInfo(bool surface_support = true);

  void SetValidationLayersEnabled(
      bool enabled = kDefaultEnableValidationLayers);

  void AddExtension(const char *extension);

  void ApplyGLFWSurfaceSupport();
};

class Instance {
 public:
  Instance(InstanceCreateInfo create_info,
           VkInstance instance,
           VkDebugUtilsMessengerEXT debug_messenger,
           InstanceProcedures instance_procedures);

  ~Instance();

  [[nodiscard]] VkInstance Handle() const {
    return instance_;
  }

  [[nodiscard]] const InstanceCreateInfo &Settings() const {
    return create_info_;
  }

  VkResult CreateSurfaceFromGLFWWindow(GLFWwindow *window,
                                       double_ptr<Surface> pp_surface) const;

 private:
  InstanceCreateInfo create_info_;

  VkInstance instance_{};
  VkDebugUtilsMessengerEXT debug_messenger_{};
  InstanceProcedures instance_procedures_{};
};

VkResult CreateInstance(
    InstanceCreateInfo create_info,
    double_ptr<Instance> pp_instance = static_cast<Instance **>(nullptr));

}  // namespace grassland::vulkan
