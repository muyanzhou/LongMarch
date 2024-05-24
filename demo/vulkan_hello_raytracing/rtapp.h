#pragma once
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

  void DestroyCoreObjects();
  void DestroyFrameAssets();

  GLFWwindow *window_;

  std::unique_ptr<vulkan::Core> core_;
  std::unique_ptr<vulkan::Image> frame_image_;
};
