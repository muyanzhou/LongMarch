#include "rtapp.h"

RayTracingApp::RayTracingApp() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  window_ = glfwCreateWindow(1920, 1080, "Ray Tracing", nullptr, nullptr);

  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Failed to create window");
  }
}

RayTracingApp::~RayTracingApp() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void RayTracingApp::Run() {
  OnInit();

  while (!glfwWindowShouldClose(window_)) {
    OnUpdate();
    OnRender();
    glfwPollEvents();
  }

  if (core_) {
    core_->Device()->WaitIdle();
  }

  OnClose();
}

void RayTracingApp::OnInit() {
  CreateCoreObjects();
  CreateFrameAssets();
}

void RayTracingApp::OnClose() {
  DestroyFrameAssets();
  DestroyCoreObjects();
}

void RayTracingApp::OnUpdate() {
}

void RayTracingApp::OnRender() {
  core_->BeginFrame();
  frame_image_->ClearColor(core_->CommandBuffer()->Handle(),
                           {0.6f, 0.7f, 0.8f, 1.0f});
  core_->OutputFrame(frame_image_.get());
  core_->EndFrame();
}

void RayTracingApp::CreateCoreObjects() {
  vulkan::CoreSettings core_settings;
  core_settings.window = window_;
  core_settings.enable_ray_tracing = true;
  core_ = std::make_unique<vulkan::Core>(core_settings);
}

void RayTracingApp::DestroyCoreObjects() {
  core_.reset();
}

void RayTracingApp::CreateFrameAssets() {
  core_->Device()->CreateImage(VK_FORMAT_B8G8R8A8_UNORM,
                               core_->Swapchain()->Extent(), &frame_image_);
}

void RayTracingApp::DestroyFrameAssets() {
  frame_image_.reset();
}
