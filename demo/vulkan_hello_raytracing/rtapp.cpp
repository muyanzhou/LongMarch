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
  CreateObjectAssets();
}

void RayTracingApp::OnClose() {
  DestroyObjectAssets();
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

void RayTracingApp::CreateObjectAssets() {
  std::vector<glm::vec3> vertices = {
      {1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};
  std::vector<uint32_t> indices = {0, 1, 2};
  core_->CreateStaticBuffer<glm::vec3>(
      vertices.size(),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      &vertex_buffer_);
  core_->CreateStaticBuffer<uint32_t>(
      indices.size(),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      &index_buffer_);
  vertex_buffer_->UploadContents(vertices.data(), vertices.size());
  index_buffer_->UploadContents(indices.data(), indices.size());
  core_->CreateBottomLevelAccelerationStructure(vertex_buffer_->GetBuffer(),
                                                index_buffer_->GetBuffer(),
                                                sizeof(glm::vec3), &blas_);
  core_->CreateTopLevelAccelerationStructure({{blas_.get(), glm::mat4{1.0f}}},
                                             &tlas_);
}

void RayTracingApp::DestroyObjectAssets() {
  tlas_.reset();
  blas_.reset();
  vertex_buffer_.reset();
  index_buffer_.reset();
}
