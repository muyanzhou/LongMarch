#pragma once

#include "grassland/vulkan/device.h"

namespace grassland::vulkan {
class ShaderModule {
 public:
  ShaderModule(const class Device *device, VkShaderModule shader_module);

  ~ShaderModule();

  [[nodiscard]] VkShaderModule Handle() const {
    return shader_module_;
  }

  [[nodiscard]] const class Device *Device() const {
    return device_;
  }

 private:
  const class Device *device_{};
  VkShaderModule shader_module_{};
};

std::vector<uint32_t> CompileGLSLToSPIRV(const std::string &glsl_code,
                                         VkShaderStageFlagBits shader_stage);
}  // namespace grassland::vulkan
