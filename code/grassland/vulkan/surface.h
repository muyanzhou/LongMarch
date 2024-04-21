#pragma once

#include "grassland/vulkan/vulkan_utils.h"

namespace grassland::vulkan {
class Surface {
 public:
  Surface(const class Instance *instance,
          GLFWwindow *window,
          VkSurfaceKHR surface);

  ~Surface();

  [[nodiscard]] VkSurfaceKHR Handle() const;

  [[nodiscard]] GLFWwindow *Window() const;

  [[nodiscard]] const Instance *Instance() const;

 private:
  const class Instance *instance_{};
  GLFWwindow *window_{};
  VkSurfaceKHR surface_{};
};
}  // namespace grassland::vulkan
