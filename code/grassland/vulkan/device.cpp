#include "grassland/vulkan/device.h"

#include <utility>

#include "grassland/vulkan/command_pool.h"
#include "grassland/vulkan/fence.h"
#include "grassland/vulkan/queue.h"
#include "grassland/vulkan/semaphore.h"
#include "grassland/vulkan/swap_chain.h"

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

VkResult Device::CreateSwapchain(const Surface *surface,
                                 double_ptr<Swapchain> pp_swapchain) const {
  if (!pp_swapchain) {
    SetErrorMessage("pp_swapchain is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }
  SwapChainSupportDetails swapChainSupport = Swapchain::QuerySwapChainSupport(
      PhysicalDevice().Handle(), surface->Handle());

  // List all available surface formats, print both VkFormat and VkColorSpace
  spdlog::info("Available surface formats:");
  for (const auto &format : swapChainSupport.formats) {
    spdlog::info("  Format: {}, Color space: {}", VkFormatToName(format.format),
                 VkColorSpaceToName(format.colorSpace));
  }

  VkSurfaceFormatKHR surfaceFormat =
      Swapchain::ChooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      Swapchain::ChooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = Swapchain::ChooseSwapExtent(swapChainSupport.capabilities,
                                                  surface->Window());

  spdlog::info("Swap chain extent: {}x{}", extent.width, extent.height);
  spdlog::info("Swap chain format: {}", VkFormatToName(surfaceFormat.format));
  spdlog::info("Swap chain color space: {}",
               VkColorSpaceToName(surfaceFormat.colorSpace));
  spdlog::info("Swap chain present mode: {}", VkPresentModeToName(presentMode));

  // Print selected surface format and present mode

  uint32_t image_count = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      image_count > swapChainSupport.capabilities.maxImageCount) {
    image_count = swapChainSupport.capabilities.maxImageCount;
  }
  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface->Handle();
  createInfo.minImageCount = image_count;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  uint32_t queueFamilyIndices[] = {
      PhysicalDevice().GraphicsFamilyIndex(),
      PhysicalDevice().PresentFamilyIndex(surface)};

  if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;      // Optional
    createInfo.pQueueFamilyIndices = nullptr;  // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swapchain;
  RETURN_IF_FAILED_VK(
      vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapchain),
      "failed to create swap chain!");

  pp_swapchain.construct(this, surface, swapchain, surfaceFormat.format,
                         extent);

  return VK_SUCCESS;
}

VkResult Device::GetQueue(uint32_t queue_family_index,
                          int queue_index,
                          double_ptr<Queue> pp_queue) const {
  if (!pp_queue) {
    SetErrorMessage("pp_queue is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (queue_index < 0) {
    queue_index =
        create_info_.queue_families.at(int(queue_family_index)).size() +
        queue_index;
    if (queue_index < 0) {
      SetErrorMessage("queue_index is out of range.");
      return VK_ERROR_INITIALIZATION_FAILED;
    }
  }

  if (queue_index >=
      create_info_.queue_families.at(int(queue_family_index)).size()) {
    Warning(
        "queue_index exceeded the number of queues in the queue family, using "
        "the last queue. this may degrade performance.");
    queue_index =
        create_info_.queue_families.at(int(queue_family_index)).size() - 1;
  }

  VkQueue queue;
  vkGetDeviceQueue(device_, queue_family_index, queue_index, &queue);

  pp_queue.construct(this, queue_family_index, queue);

  return VK_SUCCESS;
}

VkResult Device::CreateSemaphore(double_ptr<Semaphore> pp_semaphore) const {
  if (!pp_semaphore) {
    SetErrorMessage("pp_semaphore is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkSemaphore semaphore;
  VkSemaphoreCreateInfo semaphore_create_info{};

  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.pNext = nullptr;
  semaphore_create_info.flags = 0;

  RETURN_IF_FAILED_VK(
      vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &semaphore),
      "failed to create semaphore!");

  pp_semaphore.construct(this, semaphore);

  return VK_SUCCESS;
}

VkResult Device::CreateFence(bool signaled, double_ptr<Fence> pp_fence) const {
  if (!pp_fence) {
    SetErrorMessage("pp_fence is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkFence fence;
  VkFenceCreateInfo fence_create_info{};

  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.pNext = nullptr;
  fence_create_info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  RETURN_IF_FAILED_VK(
      vkCreateFence(device_, &fence_create_info, nullptr, &fence),
      "failed to create fence!");

  pp_fence.construct(this, fence);

  return VK_SUCCESS;
}

VkResult Device::CreateCommandPool(
    uint32_t queue_family_index,
    VkCommandPoolCreateFlags flags,
    double_ptr<CommandPool> pp_command_pool) const {
  if (!pp_command_pool) {
    SetErrorMessage("pp_command_pool is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkCommandPool command_pool;
  VkCommandPoolCreateInfo command_pool_create_info = {};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.queueFamilyIndex = queue_family_index;
  command_pool_create_info.flags = flags;

  RETURN_IF_FAILED_VK(vkCreateCommandPool(device_, &command_pool_create_info,
                                          nullptr, &command_pool),
                      "failed to create command pool!");

  pp_command_pool.construct(this, command_pool);

  return VK_SUCCESS;
}

VkResult Device::CreateCommandPool(
    double_ptr<CommandPool> pp_command_pool) const {
  return CreateCommandPool(physical_device_.GraphicsFamilyIndex(),
                           VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                           pp_command_pool);
}
}  // namespace grassland::vulkan
