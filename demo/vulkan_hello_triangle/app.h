#pragma once
#include "long_march.h"

class Application {
 public:
  Application();

  ~Application();

  void Run();

 private:
  void OnInit();

  void OnUpdate();

  void OnRender();

  void OnShutdown();

  GLFWwindow *window_;

  std::shared_ptr<long_march::vulkan::Instance> instance_;
  std::shared_ptr<long_march::vulkan::Surface> surface_;
  std::shared_ptr<long_march::vulkan::Device> device_;
  std::shared_ptr<long_march::vulkan::Swapchain> swapchain_;
};
