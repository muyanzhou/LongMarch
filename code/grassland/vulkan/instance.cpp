#include "grassland/vulkan/instance.h"

#include <utility>

#include "grassland/vulkan/surface.h"
#include "grassland/vulkan/validation_layer.h"

namespace grassland::vulkan {
InstanceCreateInfo::InstanceCreateInfo(bool surface_support) {
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Grassland";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Grassland";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  if (surface_support) {
    ApplyGLFWSurfaceSupport();
  }
}

void InstanceCreateInfo::SetValidationLayersEnabled(bool enabled) {
  enable_validation_layers = enabled;
}

void InstanceCreateInfo::AddExtension(const char *extension) {
  extensions.push_back(extension);
}

void InstanceCreateInfo::ApplyGLFWSurfaceSupport() {
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  for (uint32_t i = 0; i < glfw_extension_count; i++) {
    bool found = false;
    for (const auto &ext : extensions) {
      if (strcmp(ext, glfw_extensions[i]) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      extensions.push_back(glfw_extensions[i]);
    }
  }
  glfw_surface_support = true;
}

VkResult CreateInstance(InstanceCreateInfo create_info,
                        double_ptr<Instance> pp_instance) {
  VkInstanceCreateInfo instance_create_info{};
  VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};

  auto validation_layers = GetValidationLayers();
  if (create_info.enable_validation_layers) {
    if (!CheckValidationLayerSupport()) {
      SetErrorMessage("validation layer is required, but not supported.");
      return VK_ERROR_UNKNOWN;
    }
    create_info.AddExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance_create_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    instance_create_info.ppEnabledLayerNames = validation_layers.data();
    instance_create_info.pNext = &debug_create_info;

    debug_create_info = {};
    debug_create_info.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = DebugUtilsMessengerUserCallback;
  } else {
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.pNext = nullptr;
  }

#ifdef __APPLE__
  create_info.AddExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pApplicationInfo = &create_info.app_info;
  instance_create_info.enabledExtensionCount =
      static_cast<uint32_t>(create_info.extensions.size());
  instance_create_info.ppEnabledExtensionNames = create_info.extensions.data();

#ifdef __APPLE__
  instance_create_info.flags |=
      VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  VkInstance instance{nullptr};
  VkDebugUtilsMessengerEXT debug_messenger{nullptr};
  InstanceProcedures instance_procedures;

  RETURN_IF_FAILED_VK(
      vkCreateInstance(&instance_create_info, nullptr, &instance),
      "failed to create instance.");

  instance_procedures.Initialize(instance,
                                 create_info.enable_validation_layers);

  if (create_info.enable_validation_layers) {
    RETURN_IF_FAILED_VK(
        instance_procedures.vkCreateDebugUtilsMessengerEXT(
            instance, &debug_create_info, nullptr, &debug_messenger),
        "failed to set up debug messenger.");
  }

  if (pp_instance) {
    pp_instance.set(create_info, instance, debug_messenger,
                    instance_procedures);
  } else {
    if (create_info.enable_validation_layers) {
      instance_procedures.vkDestroyDebugUtilsMessengerEXT(
          instance, debug_messenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
    SetErrorMessage("pp_instance is nullptr.");
    return VK_ERROR_UNKNOWN;
  }

  return VK_SUCCESS;
}

Instance::Instance(InstanceCreateInfo create_info,
                   VkInstance instance,
                   VkDebugUtilsMessengerEXT debug_messenger,
                   InstanceProcedures instance_procedures)
    : create_info_(std::move(create_info)),
      instance_(instance),
      debug_messenger_(debug_messenger),
      instance_procedures_(instance_procedures) {
}

Instance::~Instance() {
  if (create_info_.enable_validation_layers) {
    instance_procedures_.vkDestroyDebugUtilsMessengerEXT(
        instance_, debug_messenger_, nullptr);
  }

  vkDestroyInstance(instance_, nullptr);
}

VkResult Instance::CreateSurfaceFromGLFWWindow(
    GLFWwindow *window,
    double_ptr<Surface> pp_surface) const {
  if (!create_info_.glfw_surface_support) {
    Warning("GLFW surface support is not enabled.");
  }

  VkSurfaceKHR surface{nullptr};
  VkResult result =
      glfwCreateWindowSurface(instance_, window, nullptr, &surface);
  if (result != VK_SUCCESS) {
    SetErrorMessage("failed to create window surface.");
    return result;
  }

  if (pp_surface) {
    pp_surface.set(this, window, surface);
  } else {
    vkDestroySurfaceKHR(instance_, surface, nullptr);
    SetErrorMessage("pp_surface is nullptr.");
    return VK_ERROR_UNKNOWN;
  }

  return VK_SUCCESS;
}

}  // namespace grassland::vulkan
