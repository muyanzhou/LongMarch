#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "long_march.h"

using namespace long_march;

class RayTracingApp {
 public:
  RayTracingApp();
  ~RayTracingApp();
  void Run();

 private:
  void OnInit();
  void OnUpdate();
  void OnRender();
  void OnClose();

  void CreateCoreObjects();
  void CreateFrameAssets();
  void CreateObjectAssets();

  void DestroyCoreObjects();
  void DestroyFrameAssets();
  void DestroyObjectAssets();

  GLFWwindow *window_;

  std::unique_ptr<vulkan::Core> core_;
  std::unique_ptr<vulkan::Image> frame_image_;

  std::unique_ptr<vulkan::StaticBuffer<glm::vec3>> vertex_buffer_;
  std::unique_ptr<vulkan::StaticBuffer<uint32_t>> index_buffer_;

  std::unique_ptr<vulkan::Buffer> buffer_;
  std::unique_ptr<vulkan::AccelerationStructure> blas_;
  std::unique_ptr<vulkan::AccelerationStructure> tlas_;
};
