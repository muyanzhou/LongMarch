#include "app.h"

Application::Application() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
  if (!window_) {
    throw std::runtime_error("Failed to create window");
  }
}

Application::~Application() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void Application::Run() {
  OnInit();
  while (!glfwWindowShouldClose(window_)) {
    OnUpdate();
    OnRender();
    glfwPollEvents();
  }
  device_->WaitIdle();
  OnShutdown();
}

void Application::OnInit() {
  VkResult result;
  result = long_march::vulkan::CreateInstance(
      long_march::vulkan::InstanceCreateHint{true}, &instance_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan instance");
  }

  result = instance_->CreateSurfaceFromGLFWWindow(window_, &surface_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan surface");
  }

  long_march::vulkan::DeviceFeatureRequirement feature_requirement;
  feature_requirement.surface = surface_.get();
  feature_requirement.enable_raytracing_extension = false;

  result = instance_->CreateDevice(surface_.get(), false, &device_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan device");
  }

  long_march::LogInfo(
      "Device: {}",
      device_->PhysicalDevice().GetPhysicalDeviceProperties().deviceName);

  result = device_->CreateSwapchain(surface_.get(), &swapchain_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan swapchain");
  }

  device_->GetQueue(device_->PhysicalDevice().GraphicsFamilyIndex(), 0,
                    &graphics_queue_);
  device_->GetQueue(
      device_->PhysicalDevice().PresentFamilyIndex(surface_.get()), 0,
      &present_queue_);
  device_->GetQueue(device_->PhysicalDevice().TransferFamilyIndex(), -1,
                    &transfer_queue_);

  result = device_->CreateCommandPool(&graphics_command_pool_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create graphics command pool");
  }

  result = device_->CreateCommandPool(
      device_->PhysicalDevice().TransferFamilyIndex(),
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &transfer_command_pool_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create transfer command pool");
  }

  command_buffers_.resize(max_frames_in_flight_);

  image_available_semaphores_.resize(max_frames_in_flight_);
  render_finished_semaphores_.resize(max_frames_in_flight_);
  in_flight_fences_.resize(max_frames_in_flight_);

  for (int i = 0; i < max_frames_in_flight_; ++i) {
    result = device_->CreateSemaphore(&image_available_semaphores_[i]);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to create image available semaphore");
    }
    result = device_->CreateSemaphore(&render_finished_semaphores_[i]);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to create render finished semaphore");
    }
    result = device_->CreateFence(true, &in_flight_fences_[i]);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to create in flight fence");
    }
    result = graphics_command_pool_->AllocateCommandBuffer(
        VK_COMMAND_BUFFER_LEVEL_PRIMARY, &command_buffers_[i]);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate command buffer");
    }
  }
}

void Application::OnUpdate() {
}

void Application::OnRender() {
  BeginFrame();

  VkImage image = swapchain_->Image(image_index_);

  VkCommandBuffer command_buffer = command_buffers_[current_frame_]->Handle();

  // Transit Layout from UNDEFINED to PRESENT_SRC
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.srcAccessMask = 0;
  barrier.dstAccessMask = 0;

  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  EndFrame();
}

void Application::OnShutdown() {
  image_available_semaphores_.clear();
  render_finished_semaphores_.clear();
  in_flight_fences_.clear();
  command_buffers_.clear();
  graphics_command_pool_.reset();
  transfer_command_pool_.reset();
  graphics_queue_.reset();
  present_queue_.reset();
  transfer_queue_.reset();
  swapchain_.reset();
  device_.reset();
  surface_.reset();
  instance_.reset();
}

void Application::BeginFrame() {
  VkFence fence = in_flight_fences_[current_frame_]->Handle();
  vkWaitForFences(device_->Handle(), 1, &fence, VK_TRUE, UINT64_MAX);
  vkResetFences(device_->Handle(), 1, &fence);

  if (window_) {
    VkSemaphore image_available_semaphore =
        image_available_semaphores_[current_frame_]->Handle();
    VkResult result = swapchain_->AcquireNextImage(
        std::numeric_limits<uint64_t>::max(), image_available_semaphore,
        VK_NULL_HANDLE, &image_index_);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      // Recreate swapchain
      device_->WaitIdle();
      swapchain_.reset();
      device_->CreateSwapchain(surface_.get(), &swapchain_);
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("Failed to acquire next image");
    }
  }

  VkCommandBuffer command_buffer = command_buffers_[current_frame_]->Handle();

  vkResetCommandBuffer(command_buffer, 0);

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
    throw std::runtime_error("Failed to begin recording command buffer");
  }
}

void Application::EndFrame() {
  VkCommandBuffer command_buffer = command_buffers_[current_frame_]->Handle();
  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer");
  }

  VkSemaphore image_available_semaphore =
      image_available_semaphores_[current_frame_]->Handle();
  VkSemaphore render_finished_semaphore =
      render_finished_semaphores_[current_frame_]->Handle();

  VkFence fence = in_flight_fences_[current_frame_]->Handle();
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  if (window_) {
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &image_available_semaphore;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &render_finished_semaphore;
  }

  if (vkQueueSubmit(graphics_queue_->Handle(), 1, &submit_info, fence) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to submit command buffer");
  }

  if (window_) {
    VkSwapchainKHR swapchain = swapchain_->Handle();
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_finished_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index_;

    VkResult result =
        vkQueuePresentKHR(present_queue_->Handle(), &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      device_->WaitIdle();
      swapchain_.reset();
      device_->CreateSwapchain(surface_.get(), &swapchain_);
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to present image");
    }
  }

  current_frame_ = (current_frame_ + 1) % max_frames_in_flight_;
}
