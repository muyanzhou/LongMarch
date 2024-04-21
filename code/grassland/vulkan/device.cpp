#include "grassland/vulkan/device.h"

#include <utility>

#include "grassland/vulkan/buffer.h"
#include "grassland/vulkan/command_pool.h"
#include "grassland/vulkan/descriptor_pool.h"
#include "grassland/vulkan/descriptor_set.h"
#include "grassland/vulkan/descriptor_set_layout.h"
#include "grassland/vulkan/fence.h"
#include "grassland/vulkan/image.h"
#include "grassland/vulkan/pipeline.h"
#include "grassland/vulkan/pipeline_layout.h"
#include "grassland/vulkan/queue.h"
#include "grassland/vulkan/render_pass.h"
#include "grassland/vulkan/sampler.h"
#include "grassland/vulkan/semaphore.h"
#include "grassland/vulkan/shader_module.h"
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
  vmaCreateAllocator(&allocator_info, &allocator_);
}

Device::~Device() {
  vmaDestroyAllocator(allocator_);
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

VkResult Device::CreateShaderModule(
    const std::vector<uint32_t> &code,
    double_ptr<ShaderModule> pp_shader_module) const {
  if (!pp_shader_module) {
    SetErrorMessage("pp_shader_module is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkShaderModule shader_module;
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size() * sizeof(uint32_t);
  create_info.pCode = code.data();

  RETURN_IF_FAILED_VK(
      vkCreateShaderModule(device_, &create_info, nullptr, &shader_module),
      "failed to create shader module!");

  pp_shader_module.construct(this, shader_module);

  return VK_SUCCESS;
}

VkResult Device::CreateDescriptorPool(
    const std::vector<VkDescriptorPoolSize> &pool_sizes,
    uint32_t max_sets,
    double_ptr<DescriptorPool> pp_descriptor_pool) const {
  if (!pp_descriptor_pool) {
    SetErrorMessage("pp_descriptor_pool is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkDescriptorPool descriptor_pool;
  VkDescriptorPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.poolSizeCount = pool_sizes.size();
  create_info.pPoolSizes = pool_sizes.data();
  create_info.maxSets = max_sets;

  RETURN_IF_FAILED_VK(
      vkCreateDescriptorPool(device_, &create_info, nullptr, &descriptor_pool),
      "failed to create descriptor pool!");

  pp_descriptor_pool.construct(this, descriptor_pool);

  return VK_SUCCESS;
}

VkResult Device::CreateDescriptorSetLayout(
    const std::vector<VkDescriptorSetLayoutBinding> &bindings,
    double_ptr<DescriptorSetLayout> pp_descriptor_set_layout) const {
  if (!pp_descriptor_set_layout) {
    SetErrorMessage("pp_descriptor_set_layout is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkDescriptorSetLayout descriptor_set_layout;
  VkDescriptorSetLayoutCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.bindingCount = bindings.size();
  create_info.pBindings = bindings.data();

  RETURN_IF_FAILED_VK(
      vkCreateDescriptorSetLayout(device_, &create_info, nullptr,
                                  &descriptor_set_layout),
      "failed to create descriptor set layout!");

  pp_descriptor_set_layout.construct(this, descriptor_set_layout, bindings);

  return VK_SUCCESS;
}

VkResult Device::CreateRenderPass(
    const std::vector<VkAttachmentDescription> &attachment_descriptions,
    const std::vector<struct SubpassSettings> &subpass_settings,
    const std::vector<VkSubpassDependency> &dependencies,
    grassland::double_ptr<grassland::vulkan::RenderPass> pp_render_pass) const {
  if (!pp_render_pass) {
    SetErrorMessage("pp_render_pass is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  std::vector<VkSubpassDescription> subpass_descriptions{};

  for (const auto &subpass_setting : subpass_settings) {
    subpass_descriptions.push_back(subpass_setting.Description());
  }

  // Build RenderPass here
  VkRenderPassCreateInfo render_pass_create_info{};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.attachmentCount = attachment_descriptions.size();
  render_pass_create_info.pAttachments = attachment_descriptions.data();
  render_pass_create_info.subpassCount = subpass_descriptions.size();
  render_pass_create_info.pSubpasses = subpass_descriptions.data();
  render_pass_create_info.dependencyCount = dependencies.size();
  render_pass_create_info.pDependencies = dependencies.data();

  VkRenderPass render_pass;
  RETURN_IF_FAILED_VK(vkCreateRenderPass(device_, &render_pass_create_info,
                                         nullptr, &render_pass),
                      "failed to create render pass!");

  pp_render_pass.construct(this, attachment_descriptions, subpass_settings,
                           render_pass);

  return VK_SUCCESS;
}

VkResult Device::CreateRenderPass(
    const std::vector<VkAttachmentDescription> &attachment_descriptions,
    const std::vector<VkAttachmentReference> &color_attachment_references,
    const std::optional<VkAttachmentReference> &depth_attachment_reference,
    const std::vector<VkAttachmentReference> &resolve_attachment_references,
    double_ptr<RenderPass> pp_render_pass) const {
  return CreateRenderPass(
      attachment_descriptions,
      std::vector<struct SubpassSettings>{{color_attachment_references,
                                           depth_attachment_reference,
                                           resolve_attachment_references}},
      std::vector<VkSubpassDependency>{
          {VK_SUBPASS_EXTERNAL, 0,
           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
           VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0}},
      pp_render_pass);
}

VkResult Device::CreateRenderPass(
    const std::vector<VkAttachmentDescription> &attachment_descriptions,
    const std::vector<VkAttachmentReference> &color_attachment_references,
    const std::optional<VkAttachmentReference> &depth_attachment_reference,
    double_ptr<RenderPass> pp_render_pass) const {
  return CreateRenderPass(attachment_descriptions, color_attachment_references,
                          depth_attachment_reference,
                          std::vector<VkAttachmentReference>{}, pp_render_pass);
}

VkResult Device::CreateRenderPass(
    const std::vector<VkAttachmentDescription> &attachment_descriptions,
    const std::vector<VkAttachmentReference> &color_attachment_references,
    double_ptr<RenderPass> pp_render_pass) const {
  return CreateRenderPass(attachment_descriptions, color_attachment_references,
                          std::nullopt, pp_render_pass);
}

VkResult Device::CreateImage(VkFormat format,
                             VkExtent2D extent,
                             VkImageUsageFlags usage,
                             VkImageAspectFlags aspect,
                             VkSampleCountFlagBits sample_count,
                             VmaMemoryUsage mem_usage,
                             double_ptr<Image> pp_image) const {
  if (!pp_image) {
    SetErrorMessage("pp_image is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkImageCreateInfo image_create_info{};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.format = format;
  image_create_info.extent.width = extent.width;
  image_create_info.extent.height = extent.height;
  image_create_info.extent.depth = 1;
  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.samples = sample_count;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_create_info.usage = usage;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  // Create image from image create info by VMA library

  VmaAllocationCreateInfo allocation_info = {};
  allocation_info.usage = mem_usage;

  VkImage image;
  VmaAllocation allocation;

  RETURN_IF_FAILED_VK(
      vmaCreateImage(allocator_, &image_create_info, &allocation_info, &image,
                     &allocation, nullptr),
      "failed to create image!");

  VkImageViewCreateInfo image_view_create_info{};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.image = image;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.format = format;
  image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.subresourceRange.aspectMask = aspect;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;

  VkImageView image_view;

  RETURN_IF_FAILED_VK(
      vkCreateImageView(device_, &image_view_create_info, nullptr, &image_view),
      "failed to create image view!");

  pp_image.construct(this, format, extent, usage, aspect, sample_count, image,
                     image_view, allocation);

  return VK_SUCCESS;
}

VkResult Device::CreateImage(VkFormat format,
                             VkExtent2D extent,
                             VkImageUsageFlags usage,
                             VkImageAspectFlags aspect,
                             VkSampleCountFlagBits sample_count,
                             double_ptr<Image> pp_image) const {
  return CreateImage(format, extent, usage, aspect, sample_count,
                     VMA_MEMORY_USAGE_GPU_ONLY, pp_image);
}

VkResult Device::CreateImage(VkFormat format,
                             VkExtent2D extent,
                             VkImageUsageFlags usage,
                             VkImageAspectFlags aspect,
                             double_ptr<Image> pp_image) const {
  return CreateImage(format, extent, usage, aspect, VK_SAMPLE_COUNT_1_BIT,
                     pp_image);
}

VkResult Device::CreateImage(VkFormat format,
                             VkExtent2D extent,
                             VkImageUsageFlags usage,
                             double_ptr<Image> pp_image) const {
  return CreateImage(format, extent, usage, VK_IMAGE_ASPECT_COLOR_BIT,
                     pp_image);
}

VkResult Device::CreateImage(VkFormat format,
                             VkExtent2D extent,
                             double_ptr<Image> pp_image) const {
  return CreateImage(
      format, extent,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
          VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
          VK_IMAGE_USAGE_STORAGE_BIT,
      pp_image);
}

VkResult Device::CreateSampler(VkFilter mag_filter,
                               VkFilter min_filter,
                               VkSamplerAddressMode address_mode_u,
                               VkSamplerAddressMode address_mode_v,
                               VkSamplerAddressMode address_mode_w,
                               VkBool32 anisotropy_enable,
                               VkBorderColor border_color,
                               VkSamplerMipmapMode mipmap_mode,
                               double_ptr<Sampler> pp_sampler) const {
  if (!pp_sampler) {
    SetErrorMessage("pp_sampler is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkSamplerCreateInfo sampler_create_info{};
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.magFilter = mag_filter;
  sampler_create_info.minFilter = min_filter;
  sampler_create_info.addressModeU = address_mode_u;
  sampler_create_info.addressModeV = address_mode_v;
  sampler_create_info.addressModeW = address_mode_w;
  sampler_create_info.anisotropyEnable = anisotropy_enable;
  sampler_create_info.maxAnisotropy = 1.0f;
  sampler_create_info.borderColor = border_color;
  sampler_create_info.unnormalizedCoordinates = VK_FALSE;
  sampler_create_info.compareEnable = VK_FALSE;
  sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_create_info.mipmapMode = mipmap_mode;

  VkSampler sampler;

  RETURN_IF_FAILED_VK(
      vkCreateSampler(device_, &sampler_create_info, nullptr, &sampler),
      "failed to create texture sampler!");

  pp_sampler.construct(this, sampler);

  return VK_SUCCESS;
}

VkResult Device::CreateBuffer(VkDeviceSize size,
                              VkBufferUsageFlags usage,
                              VmaMemoryUsage memory_usage,
                              VmaAllocationCreateFlags flags,
                              double_ptr<Buffer> pp_buffer) const {
  if (!pp_buffer) {
    SetErrorMessage("pp_buffer is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.usage = memory_usage;
  alloc_info.flags = flags;

  VkBuffer buffer;
  VmaAllocation allocation;

  RETURN_IF_FAILED_VK(vmaCreateBuffer(allocator_, &buffer_info, &alloc_info,
                                      &buffer, &allocation, nullptr),
                      "failed to create buffer!");

  pp_buffer.construct(this, size, buffer, allocation);

  return VK_SUCCESS;
}

VkResult Device::CreateBuffer(VkDeviceSize size,
                              VkBufferUsageFlags usage,
                              VmaMemoryUsage memory_usage,
                              double_ptr<Buffer> pp_buffer) const {
  return CreateBuffer(size, usage, memory_usage, 0, pp_buffer);
}

VkResult Device::CreatePipelineLayout(
    const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts,
    double_ptr<PipelineLayout> pp_pipeline_layout) const {
  if (!pp_pipeline_layout) {
    SetErrorMessage("pp_pipeline_layout is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkPipelineLayout pipeline_layout;
  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = descriptor_set_layouts.size();
  pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
  pipeline_layout_info.pushConstantRangeCount = 0;
  pipeline_layout_info.pPushConstantRanges = nullptr;

  RETURN_IF_FAILED_VK(vkCreatePipelineLayout(device_, &pipeline_layout_info,
                                             nullptr, &pipeline_layout),
                      "failed to create pipeline layout!");

  pp_pipeline_layout.construct(this, pipeline_layout);

  return VK_SUCCESS;
}

VkResult Device::CreatePipeline(struct PipelineSettings settings,
                                double_ptr<Pipeline> pp_pipeline) const {
  if (!pp_pipeline) {
    SetErrorMessage("pp_pipeline is nullptr");
    return VK_ERROR_INITIALIZATION_FAILED;
  }
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount =
      settings.vertex_input_binding_descriptions.size();
  if (vertex_input_info.vertexBindingDescriptionCount) {
    vertex_input_info.pVertexBindingDescriptions =
        settings.vertex_input_binding_descriptions.data();
  }
  vertex_input_info.vertexAttributeDescriptionCount =
      settings.vertex_input_attribute_descriptions.size();
  if (vertex_input_info.vertexAttributeDescriptionCount) {
    vertex_input_info.pVertexAttributeDescriptions =
        settings.vertex_input_attribute_descriptions.data();
  }

  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = nullptr;  // Dynamic
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = nullptr;  // Dynamic

  VkPipelineColorBlendStateCreateInfo color_blend_state{};
  color_blend_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_state.logicOpEnable = VK_FALSE;
  color_blend_state.logicOp = VK_LOGIC_OP_COPY;
  color_blend_state.attachmentCount =
      settings.pipeline_color_blend_attachment_states.size();
  color_blend_state.pAttachments =
      settings.pipeline_color_blend_attachment_states.data();
  color_blend_state.blendConstants[0] = 0.0f;
  color_blend_state.blendConstants[1] = 0.0f;
  color_blend_state.blendConstants[2] = 0.0f;
  color_blend_state.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount =
      static_cast<uint32_t>(dynamic_states.size());
  if (dynamic_state.dynamicStateCount) {
    dynamic_state.pDynamicStates = dynamic_states.data();
  }

  VkGraphicsPipelineCreateInfo pipeline_create_info{};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = settings.shader_stage_create_infos.size();
  pipeline_create_info.pStages = settings.shader_stage_create_infos.data();
  pipeline_create_info.pVertexInputState = &vertex_input_info;
  pipeline_create_info.pInputAssemblyState =
      &settings.input_assembly_state_create_info;
  pipeline_create_info.pViewportState = &viewport_state;
  pipeline_create_info.pRasterizationState =
      &settings.rasterization_state_create_info;
  pipeline_create_info.pMultisampleState =
      &settings.multisample_state_create_info;
  pipeline_create_info.pColorBlendState = &color_blend_state;
  pipeline_create_info.pDynamicState = &dynamic_state;
  pipeline_create_info.layout = settings.pipeline_layout->Handle();
  pipeline_create_info.renderPass = settings.render_pass->Handle();
  pipeline_create_info.pDepthStencilState =
      settings.depth_stencil_state_create_info.has_value()
          ? &settings.depth_stencil_state_create_info.value()
          : nullptr;
  pipeline_create_info.subpass = settings.subpass;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

  VkPipeline pipeline;
  RETURN_IF_FAILED_VK(
      vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1,
                                &pipeline_create_info, nullptr, &pipeline),
      "failed to create graphics pipeline!");

  pp_pipeline.construct(this, pipeline);

  return VK_SUCCESS;
}

}  // namespace grassland::vulkan
