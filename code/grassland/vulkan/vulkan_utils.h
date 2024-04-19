#pragma once
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace grassland::vulkan {
#define GRASSLAND_VULKAN_PROCEDURE_VAR(function_name) \
  PFN_##function_name function_name{};

#ifdef NDEBUG
constexpr bool kDefaultEnableValidationLayers = false;
#else
constexpr bool kDefaultEnableValidationLayers = true;
#endif

std::string PCIVendorIDToName(uint32_t vendor_id);

std::string VkFormatToName(VkFormat format);
std::string VkColorSpaceToName(VkColorSpaceKHR color_space);
std::string VkPresentModeToName(VkPresentModeKHR present_mode);

void ThrowError(const std::string &message);

template <class... Args>
void ThrowError(const std::string &message, Args &&...args) {
  ThrowError(fmt::format(message, std::forward<Args>(args)...));
}

void ThrowIfFailed(VkResult result, const std::string &message);

template <class... Args>
void ThrowIfFailed(VkResult result,
                   const std::string &message,
                   Args &&...args) {
  ThrowIfFailed(result, fmt::format(message, std::forward<Args>(args)...));
}

void Warning(const std::string &message);

template <class... Args>
void Warning(const std::string &message, Args &&...args) {
  Warning(fmt::format(message, std::forward<Args>(args)...));
}
}  // namespace grassland::vulkan
