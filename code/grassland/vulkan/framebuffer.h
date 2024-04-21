#pragma once

#include "grassland/vulkan/device.h"
#include "grassland/vulkan/render_pass.h"

namespace grassland::vulkan {
class Framebuffer {
 public:
  Framebuffer(const class RenderPass *render_pass,
              VkExtent2D extent,
              VkFramebuffer framebuffer);

  ~Framebuffer();

  [[nodiscard]] VkFramebuffer Handle() const {
    return framebuffer_;
  }

  [[nodiscard]] const class RenderPass *RenderPass() const {
    return render_pass_;
  }

  [[nodiscard]] VkExtent2D Extent() const {
    return extent_;
  }

 private:
  const class RenderPass *render_pass_;
  VkExtent2D extent_{};
  VkFramebuffer framebuffer_{};
};
}  // namespace grassland::vulkan
