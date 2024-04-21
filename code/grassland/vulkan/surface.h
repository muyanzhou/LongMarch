#pragma once
#include "grassland/vulkan/vulkan_utils.h"

namespace grassland::vulkan {
class Surface {
 public:
  Surface(const Instance *instance, GLFWwindow *window, VkSurfaceKHR surface);

  ~Surface();

  [[nodiscard]] VkSurfaceKHR Handle() const;

 private:
  const Instance *instance_{};
  GLFWwindow *window_{};
  VkSurfaceKHR surface_{};
};
}  // namespace grassland::vulkan
