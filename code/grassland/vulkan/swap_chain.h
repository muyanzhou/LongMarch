#pragma once

#include "grassland/vulkan/device.h"
#include "grassland/vulkan/surface.h"
#include "grassland/vulkan/vulkan_utils.h"

namespace grassland::vulkan {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain {
 public:
  Swapchain(const class grassland::vulkan::Device *device,
            const class grassland::vulkan::Surface *surface,
            VkSwapchainKHR swapchain,
            VkFormat format,
            VkExtent2D extent);

  ~Swapchain();

  VkSwapchainKHR Handle() {
    return swapchain_;
  }

  const class Device *Device() {
    return device_;
  }

  const class Surface *Surface() {
    return surface_;
  }

  VkFormat Format() {
    return format_;
  }

  VkExtent2D Extent() {
    return extent_;
  }

  [[nodiscard]] const std::vector<VkImage> &Images() const {
    return images_;
  }

  [[nodiscard]] const std::vector<VkImageView> &ImageViews() const {
    return image_views_;
  }

  VkImage Image(uint32_t index) {
    return images_[index];
  }

  VkImageView ImageView(uint32_t index) {
    return image_views_[index];
  }

  // Acquire an image from the swap chain
  VkResult AcquireNextImage(uint64_t timeout,
                            VkSemaphore semaphore,
                            VkFence fence,
                            uint32_t *image_index);

  static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                                       VkSurfaceKHR surface);

  static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  static VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  static VkExtent2D ChooseSwapExtent(
      const VkSurfaceCapabilitiesKHR &capabilities,
      GLFWwindow *window);

 private:
  void CreateImageViews();

  const class Device *device_;
  const class Surface *surface_;

  VkSwapchainKHR swapchain_{};
  VkFormat format_{};
  VkExtent2D extent_{};
  uint32_t image_count_{};
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
};
}  // namespace grassland::vulkan
