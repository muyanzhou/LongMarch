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
}

void Application::OnUpdate() {
}

void Application::OnRender() {
}

void Application::OnShutdown() {
  swapchain_.reset();
  device_.reset();
  surface_.reset();
  instance_.reset();
}
