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

  void BeginFrame();

  void EndFrame();

  GLFWwindow *window_;

  int max_frames_in_flight_ = 2;

  std::shared_ptr<long_march::vulkan::Instance> instance_;
  std::shared_ptr<long_march::vulkan::Surface> surface_;
  std::shared_ptr<long_march::vulkan::Device> device_;

  std::shared_ptr<long_march::vulkan::Swapchain> swapchain_;
  std::shared_ptr<long_march::vulkan::Queue> graphics_queue_;
  std::shared_ptr<long_march::vulkan::Queue> present_queue_;
  std::shared_ptr<long_march::vulkan::Queue> transfer_queue_;

  std::shared_ptr<long_march::vulkan::CommandPool> graphics_command_pool_;
  std::shared_ptr<long_march::vulkan::CommandPool> transfer_command_pool_;
  std::vector<std::shared_ptr<long_march::vulkan::CommandBuffer>>
      command_buffers_;

  std::vector<std::shared_ptr<long_march::vulkan::Semaphore>>
      image_available_semaphores_;
  std::vector<std::shared_ptr<long_march::vulkan::Semaphore>>
      render_finished_semaphores_;
  std::vector<std::shared_ptr<long_march::vulkan::Fence>> in_flight_fences_;

  uint32_t current_frame_ = 0;
  uint32_t image_index_ = 0;
};
