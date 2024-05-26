#pragma once

#include "grassland/vulkan/buffer.h"
#include "grassland/vulkan/command_buffer.h"
#include "grassland/vulkan/command_pool.h"
#include "grassland/vulkan/device.h"
#include "grassland/vulkan/fence.h"
#include "grassland/vulkan/instance.h"
#include "grassland/vulkan/queue.h"
#include "grassland/vulkan/semaphore.h"
#include "grassland/vulkan/surface.h"
#include "grassland/vulkan/swap_chain.h"

namespace grassland::vulkan {
struct CoreSettings {
  GLFWwindow *window{nullptr};
  bool enable_validation_layers{kDefaultEnableValidationLayers};
  bool enable_ray_tracing{false};
  int device_index{-1};
  int max_frames_in_flight{3};
};

template <class Type>
class StaticBuffer;

template <class Type>
class DynamicBuffer;

class Core {
 public:
  Core(const CoreSettings &settings = CoreSettings{});

  ~Core();

  [[nodiscard]] class Surface *Surface() const {
    return surface_.get();
  }

  [[nodiscard]] class Instance *Instance() const {
    return instance_.get();
  }

  [[nodiscard]] class Device *Device() const {
    return device_.get();
  }

  [[nodiscard]] Queue *GraphicsQueue() const {
    return graphics_queue_.get();
  }

  [[nodiscard]] Queue *TransferQueue() const {
    return transfer_queue_.get();
  }

  [[nodiscard]] Queue *PresentQueue() const {
    return present_queue_.get();
  }

  [[nodiscard]] CommandPool *GraphicsCommandPool() const {
    return graphics_command_pool_.get();
  }

  [[nodiscard]] CommandPool *TransferCommandPool() const {
    return transfer_command_pool_.get();
  }

  [[nodiscard]] class Swapchain *Swapchain() const {
    return swap_chain_.get();
  }

  [[nodiscard]] int MaxFramesInFlight() const {
    return settings_.max_frames_in_flight;
  }

  // Get the current frame index
  [[nodiscard]] uint32_t CurrentFrame() const {
    return current_frame_;
  }

  // Get current image index
  [[nodiscard]] uint32_t ImageIndex() const {
    return image_index_;
  }

  // Get the current command buffer
  [[nodiscard]] class CommandBuffer *CommandBuffer() const {
    return command_buffers_[current_frame_].get();
  }

  [[nodiscard]] const CoreSettings &Settings() const {
    return settings_;
  }

  template <class Type>
  VkResult CreateStaticBuffer(
      size_t length = 1,
      VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      double_ptr<StaticBuffer<Type>> pp_buffer = nullptr);

  template <class Type>
  VkResult CreateDynamicBuffer(
      size_t length = 1,
      VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      double_ptr<DynamicBuffer<Type>> pp_buffer = nullptr);

  VkResult CreateBottomLevelAccelerationStructure(
      VkDeviceAddress vertex_buffer_address,
      VkDeviceAddress index_buffer_address,
      uint32_t num_vertex,
      VkDeviceSize stride,
      uint32_t primitive_count,
      double_ptr<AccelerationStructure> pp_blas);

  VkResult CreateBottomLevelAccelerationStructure(
      Buffer *vertex_buffer,
      Buffer *index_buffer,
      VkDeviceSize stride,
      double_ptr<AccelerationStructure> pp_blas);

  template <class VertexType>
  VkResult CreateBottomLevelAccelerationStructure(
      const std::vector<VertexType> &vertices,
      const std::vector<uint32_t> &indices,
      double_ptr<AccelerationStructure> pp_blas);

  VkResult CreateTopLevelAccelerationStructure(
      const std::vector<std::pair<AccelerationStructure *, glm::mat4>> &objects,
      double_ptr<AccelerationStructure> pp_tlas);

  void SingleTimeCommands(const std::function<void(VkCommandBuffer)> &function);

  // Begin Frame function and End Frame function
  void BeginFrame();

  void EndFrame();

  void OutputFrame(Image *image);

  void RebuildSwapChain();

  EventManager<void(int, int)> &FrameSizeEvent() {
    return frame_size_event_;
  }

  EventManager<void(double, double)> &CursorPosEvent() {
    return cursor_pos_event_;
  }

  EventManager<void(int, int, int)> &MouseButtonEvent() {
    return mouse_button_event_;
  }

  EventManager<void(int, int, int, int)> &KeyEvent() {
    return key_event_;
  }

  EventManager<void(int, const char **)> &DropEvent() {
    return drop_event_;
  }

  EventManager<void(double, double)> &ScrollEvent() {
    return scroll_event_;
  }

 private:
  void RegisterGLFWWindowEventCallbacks();

  CoreSettings settings_;
  std::unique_ptr<class Instance> instance_;
  std::unique_ptr<class Surface> surface_;
  std::unique_ptr<class Device> device_;
  std::unique_ptr<Queue> graphics_queue_;
  std::unique_ptr<Queue> transfer_queue_;
  std::unique_ptr<Queue> present_queue_;
  std::unique_ptr<CommandPool> graphics_command_pool_;
  std::unique_ptr<CommandPool> transfer_command_pool_;
  std::vector<std::unique_ptr<class CommandBuffer>> command_buffers_;
  std::unique_ptr<class Swapchain> swap_chain_;

  // Semaphores and fences
  std::vector<std::unique_ptr<class Semaphore>> image_available_semaphores_;
  std::vector<std::unique_ptr<class Semaphore>> render_finish_semaphores_;
  std::vector<std::unique_ptr<class Fence>> in_flight_fences_;
  uint32_t current_frame_{0};
  uint32_t image_index_{0};

  EventManager<void(int, int)> frame_size_event_;
  EventManager<void(double, double)> cursor_pos_event_;
  EventManager<void(int, int, int)> mouse_button_event_;
  EventManager<void(int, int, int, int)> key_event_;
  EventManager<void(int, const char **)> drop_event_;
  EventManager<void(double, double)> scroll_event_;
};

template <class VertexType>
VkResult grassland::vulkan::Core::CreateBottomLevelAccelerationStructure(
    const std::vector<VertexType> &vertices,
    const std::vector<uint32_t> &indices,
    grassland::double_ptr<grassland::vulkan::AccelerationStructure> pp_blas) {
  std::unique_ptr<vulkan::Buffer> vertex_buffer;
  std::unique_ptr<vulkan::Buffer> index_buffer;
  device_->CreateBuffer(
      vertices.size() * sizeof(VertexType),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_CPU_TO_GPU, &vertex_buffer);
  device_->CreateBuffer(
      indices.size() * sizeof(uint32_t),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VMA_MEMORY_USAGE_CPU_TO_GPU, &index_buffer);

  std::memcpy(vertex_buffer->Map(), vertices.data(),
              vertices.size() * sizeof(VertexType));
  vertex_buffer->Unmap();
  std::memcpy(index_buffer->Map(), indices.data(),
              indices.size() * sizeof(uint32_t));
  index_buffer->Unmap();

  return CreateBottomLevelAccelerationStructure(
      vertex_buffer.get(), index_buffer.get(), sizeof(VertexType), pp_blas);
}
}  // namespace grassland::vulkan
