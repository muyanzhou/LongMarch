#pragma once

#include "grassland/vulkan/device_creation_assist.h"
#include "grassland/vulkan/device_procedures.h"
#include "grassland/vulkan/instance.h"
#include "grassland/vulkan/physical_device.h"

namespace grassland::vulkan {

class Device {
 public:
  Device(const class Instance *instance,
         const class PhysicalDevice &physical_device,
         DeviceCreateInfo create_info,
         VkDevice device);

  ~Device();

  VkDevice Handle() const {
    return device_;
  }

  const class Instance *Instance() const {
    return instance_;
  }

  const class PhysicalDevice &PhysicalDevice() const {
    return physical_device_;
  }

  const DeviceCreateInfo &CreateInfo() const {
    return create_info_;
  }

  DeviceProcedures &Procedures() {
    return procedures_;
  }

  const DeviceProcedures &Procedures() const {
    return procedures_;
  }

  VmaAllocator Allocator() const {
    return allocator_;
  }

  VkResult WaitIdle() const {
    return vkDeviceWaitIdle(device_);
  }

  VkResult GetQueue(uint32_t queue_family_index,
                    int queue_index,
                    double_ptr<Queue> pp_queue) const;

  VkResult CreateSwapchain(const Surface *surface,
                           double_ptr<Swapchain> pp_swapchain) const;

  VkResult CreateSemaphore(double_ptr<Semaphore> pp_semaphore) const;

  VkResult CreateFence(bool signaled, double_ptr<Fence> pp_fence) const;

  VkResult CreateCommandPool(uint32_t queue_family_index,
                             VkCommandPoolCreateFlags flags,
                             double_ptr<CommandPool> pp_command_pool) const;

  VkResult CreateCommandPool(double_ptr<CommandPool> pp_command_pool) const;

  VkResult CreateShaderModule(const std::vector<uint32_t> &code,
                              double_ptr<ShaderModule> pp_shader_module) const;

  VkResult CreateDescriptorPool(
      const std::vector<VkDescriptorPoolSize> &pool_sizes,
      uint32_t max_sets,
      double_ptr<DescriptorPool> pp_descriptor_pool) const;

  VkResult CreateDescriptorSetLayout(
      const std::vector<VkDescriptorSetLayoutBinding> &bindings,
      double_ptr<DescriptorSetLayout> pp_descriptor_set_layout) const;

  VkResult CreateRenderPass(
      const std::vector<VkAttachmentDescription> &attachment_descriptions,
      const std::vector<struct SubpassSettings> &subpass_settings,
      const std::vector<VkSubpassDependency> &dependencies,
      double_ptr<RenderPass> pp_render_pass) const;

  VkResult CreateRenderPass(
      const std::vector<VkAttachmentDescription> &attachment_descriptions,
      const std::vector<VkAttachmentReference> &color_attachment_references,
      const std::optional<VkAttachmentReference> &depth_attachment_reference,
      const std::vector<VkAttachmentReference> &resolve_attachment_references,
      double_ptr<RenderPass> pp_render_pass) const;

  VkResult CreateRenderPass(
      const std::vector<VkAttachmentDescription> &attachment_descriptions,
      const std::vector<VkAttachmentReference> &color_attachment_references,
      const std::optional<VkAttachmentReference> &depth_attachment_reference,
      double_ptr<RenderPass> pp_render_pass) const;

  VkResult CreateRenderPass(
      const std::vector<VkAttachmentDescription> &attachment_descriptions,
      const std::vector<VkAttachmentReference> &color_attachment_references,
      double_ptr<RenderPass> pp_render_pass) const;

  VkResult CreateImage(VkFormat format,
                       VkExtent2D extent,
                       VkImageUsageFlags usage,
                       VkImageAspectFlags aspect,
                       VkSampleCountFlagBits sample_count,
                       VmaMemoryUsage mem_usage,
                       double_ptr<Image> pp_image) const;

  VkResult CreateImage(VkFormat format,
                       VkExtent2D extent,
                       VkImageUsageFlags usage,
                       VkImageAspectFlags aspect,
                       VkSampleCountFlagBits sample_count,
                       double_ptr<Image> pp_image) const;

  VkResult CreateImage(VkFormat format,
                       VkExtent2D extent,
                       VkImageUsageFlags usage,
                       VkImageAspectFlags aspect,
                       double_ptr<Image> pp_image) const;

  VkResult CreateImage(VkFormat format,
                       VkExtent2D extent,
                       VkImageUsageFlags usage,
                       double_ptr<Image> pp_image) const;

  VkResult CreateImage(VkFormat format,
                       VkExtent2D extent,
                       double_ptr<Image> pp_image) const;

  VkResult CreateSampler(VkFilter mag_filter,
                         VkFilter min_filter,
                         VkSamplerAddressMode address_mode_u,
                         VkSamplerAddressMode address_mode_v,
                         VkSamplerAddressMode address_mode_w,
                         VkBool32 anisotropy_enable,
                         VkBorderColor border_color,
                         VkSamplerMipmapMode mipmap_mode,
                         double_ptr<Sampler> pp_sampler) const;

  VkResult CreateBuffer(VkDeviceSize size,
                        VkBufferUsageFlags usage,
                        VmaMemoryUsage memory_usage,
                        VmaAllocationCreateFlags flags,
                        double_ptr<Buffer> pp_buffer) const;

  VkResult CreateBuffer(VkDeviceSize size,
                        VkBufferUsageFlags usage,
                        VmaMemoryUsage memory_usage,
                        double_ptr<Buffer> pp_buffer) const;

  VkResult CreatePipelineLayout(
      const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts,
      double_ptr<PipelineLayout> pp_pipeline_layout) const;

  VkResult CreatePipeline(struct PipelineSettings settings,
                          double_ptr<Pipeline> pp_pipeline) const;

 private:
  const class Instance *instance_{};

  class PhysicalDevice physical_device_;

  const DeviceCreateInfo create_info_;

  VkDevice device_{};

  DeviceProcedures procedures_{};

  VmaAllocator allocator_{};
};

}  // namespace grassland::vulkan
