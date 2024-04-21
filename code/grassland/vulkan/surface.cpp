#include "grassland/vulkan/surface.h"

#include "grassland/vulkan/instance.h"

namespace grassland::vulkan {
Surface::Surface(const Instance *instance,
                 GLFWwindow *window,
                 VkSurfaceKHR surface)
    : instance_(instance), window_(window), surface_(surface) {
}

Surface::~Surface() {
  vkDestroySurfaceKHR(instance_->Handle(), surface_, nullptr);
}

[[nodiscard]] VkSurfaceKHR Surface::Handle() const {
  return surface_;
}
}  // namespace grassland::vulkan