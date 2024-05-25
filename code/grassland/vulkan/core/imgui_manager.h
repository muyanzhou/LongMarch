#pragma once
#include "grassland/vulkan/core/core_object.h"
#include "grassland/vulkan/descriptor_pool.h"
#include "grassland/vulkan/framebuffer.h"
#include "grassland/vulkan/image.h"
#include "grassland/imgui/imgui_impl_glfw.h"
#include "grassland/imgui/imgui_impl_vulkan.h"

namespace grassland::vulkan {
class ImGuiManager {
 public:
  ImGuiManager(Core *core,
               Image *image,
               const char *font_file_path,
               float font_size);

  ~ImGuiManager();

  void BeginFrame();
  void EndFrame();

  void Render(VkCommandBuffer cmd_buffer) const;

 private:
  Core *core_;
  Image *image_;
  std::unique_ptr<DescriptorPool> descriptor_pool_;
  std::unique_ptr<RenderPass> render_pass_;
  std::unique_ptr<Framebuffer> framebuffer_;
};
}  // namespace grassland::vulkan
