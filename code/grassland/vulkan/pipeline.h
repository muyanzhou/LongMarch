#pragma once
#include "grassland/vulkan/device.h"
#include "grassland/vulkan/render_pass.h"
#include "grassland/vulkan/shader_module.h"

namespace grassland::vulkan {
struct PipelineSettings {
  explicit PipelineSettings(const RenderPass *render_pass = nullptr,
                            const PipelineLayout *pipeline_layout = nullptr,
                            int subpass = 0);

  void AddShaderStage(ShaderModule *shader_module, VkShaderStageFlagBits stage);

  void AddInputBinding(
      uint32_t binding,
      uint32_t stride,
      VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX);

  void AddInputAttribute(uint32_t binding,
                         uint32_t location,
                         VkFormat format,
                         uint32_t offset);

  void SetPrimitiveTopology(VkPrimitiveTopology topology);

  void SetMultiSampleState(VkSampleCountFlagBits sample_count);

  void SetCullMode(VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT);

  void SetSubpass(int subpass);

  void SetBlendState(
      int color_attachment_index,
      VkPipelineColorBlendAttachmentState blend_attachment_state = {
          VK_FALSE,
          VK_BLEND_FACTOR_ONE,
          VK_BLEND_FACTOR_ZERO,
          VK_BLEND_OP_ADD,
          VK_BLEND_FACTOR_ONE,
          VK_BLEND_FACTOR_ZERO,
          VK_BLEND_OP_ADD,
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
      });

  // Render pass
  const RenderPass *render_pass;

  // Pipeline layout
  const PipelineLayout *pipeline_layout;

  // Shader stages
  std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;

  // Vertex input
  std::vector<VkVertexInputBindingDescription>
      vertex_input_binding_descriptions;
  std::vector<VkVertexInputAttributeDescription>
      vertex_input_attribute_descriptions;

  // Blend state
  std::vector<VkPipelineColorBlendAttachmentState>
      pipeline_color_blend_attachment_states;

  // Primitive topology
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{};

  // Depth stencil state
  std::optional<VkPipelineDepthStencilStateCreateInfo>
      depth_stencil_state_create_info;

  // Multisample state
  VkPipelineMultisampleStateCreateInfo multisample_state_create_info{};

  // Rasterization state
  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{};

  int subpass{};
};

class Pipeline {
 public:
  explicit Pipeline(const class Device *device, VkPipeline pipeline);

  ~Pipeline();

  [[nodiscard]] VkPipeline Handle() const {
    return pipeline_;
  }

  [[nodiscard]] const class Device *Device() const {
    return device_;
  }

 private:
  const class Device *device_{};
  VkPipeline pipeline_{};
};
}  // namespace grassland::vulkan
