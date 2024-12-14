#include "grassland/vulkan/device_creation_assist.h"

namespace grassland::vulkan {

VkDeviceCreateInfo DeviceCreateInfo::CompileVkDeviceCreateInfo(
    bool enable_validation_layers,
    const PhysicalDevice &physical_device) {
  queue_create_infos_.clear();
  queue_create_infos_.reserve(queue_families.size());
  for (auto &[family_index, priorities] : queue_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family_index;
    queue_create_info.queueCount = priorities.size();
    queue_create_info.pQueuePriorities = priorities.data();
    queue_create_infos_.push_back(queue_create_info);
  }

  physical_device_features_ = physical_device.GetPhysicalDeviceFeatures();

  void *pNext = nullptr;
  for (auto &feature : features) {
    pNext = feature->LinkNext(pNext);
  }

  if (enable_validation_layers) {
    enabled_layers_ = GetValidationLayers();
  } else {
    enabled_layers_.clear();
  }

  VkDeviceCreateInfo create_info{};

  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount =
      static_cast<uint32_t>(queue_create_infos_.size());
  create_info.pQueueCreateInfos = queue_create_infos_.data();
  create_info.pEnabledFeatures = &physical_device_features_;
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  if (create_info.enabledExtensionCount) {
    create_info.ppEnabledExtensionNames = extensions.data();
  }
  create_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers_.size());
  if (create_info.enabledLayerCount) {
    create_info.ppEnabledLayerNames = enabled_layers_.data();
  }
  create_info.pNext = pNext;
  return create_info;
}

class DeviceCreateInfo
DeviceFeatureRequirement::GenerateRecommendedDeviceCreateInfo(
    const PhysicalDevice &physical_device) const {
  DeviceCreateInfo create_info;

  if (surface) {
    create_info.AddExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  }

#ifdef __APPLE__
  create_info.AddExtension("VK_KHR_portability_subset");
#endif
  create_info.AddExtension(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
  create_info.AddExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
  create_info.AddExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

  if (enable_raytracing_extension) {
    create_info.AddExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
    create_info.AddExtension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
    create_info.AddExtension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
    create_info.AddExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
    create_info.AddExtension(VK_KHR_RAY_QUERY_EXTENSION_NAME);

    VkPhysicalDeviceBufferDeviceAddressFeatures
        physical_device_buffer_device_address_features{};
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR
        physical_device_ray_tracing_pipeline_features{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR
        physical_device_acceleration_structure_features{};
    VkPhysicalDeviceRayQueryFeaturesKHR physical_device_ray_query_features{};

    physical_device_buffer_device_address_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    physical_device_buffer_device_address_features.bufferDeviceAddress =
        VK_TRUE;

    physical_device_ray_tracing_pipeline_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    physical_device_ray_tracing_pipeline_features.rayTracingPipeline = VK_TRUE;

    physical_device_acceleration_structure_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    physical_device_acceleration_structure_features.accelerationStructure =
        VK_TRUE;

    physical_device_ray_query_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
    physical_device_ray_query_features.rayQuery = VK_TRUE;

    create_info.AddFeature(physical_device_buffer_device_address_features);
    create_info.AddFeature(physical_device_ray_tracing_pipeline_features);
    create_info.AddFeature(physical_device_acceleration_structure_features);
    create_info.AddFeature(physical_device_ray_query_features);
  }

  VkPhysicalDeviceDescriptorIndexingFeaturesEXT
      physical_device_descriptor_indexing_features{};
  physical_device_descriptor_indexing_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
  physical_device_descriptor_indexing_features
      .shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
  physical_device_descriptor_indexing_features.runtimeDescriptorArray = VK_TRUE;
  physical_device_descriptor_indexing_features
      .descriptorBindingVariableDescriptorCount = VK_TRUE;
  VkPhysicalDeviceDynamicRenderingFeaturesKHR
      physical_device_dynamic_rendering_features{};
  physical_device_dynamic_rendering_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
  physical_device_dynamic_rendering_features.dynamicRendering = VK_TRUE;
  create_info.AddFeature(physical_device_descriptor_indexing_features);
  create_info.AddFeature(physical_device_dynamic_rendering_features);

  //        VkPhysicalDeviceSynchronization2FeaturesKHR
  //                physical_device_synchronization2_features{};
  //        physical_device_synchronization2_features.sType =
  //                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
  //        physical_device_synchronization2_features.synchronization2 =
  //        VK_TRUE;
  //        create_info.AddFeature(physical_device_synchronization2_features);

  std::map<int, int> queue_count;

  if (num_graphics_queue > 0) {
    queue_count[physical_device.GraphicsFamilyIndex()] += num_graphics_queue;
  }
  if (num_transfer_queue > 0) {
    queue_count[physical_device.TransferFamilyIndex()] += num_transfer_queue;
  }
  if (num_compute_queue > 0) {
    queue_count[physical_device.ComputeFamilyIndex()] += num_compute_queue;
  }

  if (surface) {
    queue_count[physical_device.PresentFamilyIndex(surface)] =
        std::max(queue_count[physical_device.PresentFamilyIndex(surface)], 1);
  }

  auto queue_family_properties = physical_device.GetQueueFamilyProperties();

  for (auto &[family_index, count] : queue_count) {
    count =
        std::min(count, int(queue_family_properties[family_index].queueCount));
    create_info.AddQueueFamily(family_index, std::vector<float>(count, 1.0f));
  }

  return create_info;
}

VmaAllocatorCreateFlags DeviceFeatureRequirement::GetVmaAllocatorCreateFlags()
    const {
  VmaAllocatorCreateFlags flags = 0;
  if (enable_raytracing_extension) {
    flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  }
  return flags;
}
}  // namespace grassland::vulkan
