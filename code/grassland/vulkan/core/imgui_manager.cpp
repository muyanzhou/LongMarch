#include "grassland/vulkan/core/imgui_manager.h"

namespace grassland::vulkan {

ImGuiManager::ImGuiManager(Core *core,
                           Image *image,
                           const char *font_file_path,
                           float font_size)
    : core_(core), image_(image) {
  DescriptorPoolSize descriptor_pool_size;
  descriptor_pool_size.descriptor_type_count = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 32}};

  core_->Device()->CreateDescriptorPool(descriptor_pool_size, 1000,
                                        &descriptor_pool_);

  ImGui::CreateContext();
  ImGui::StyleColorsClassic();
  ImGui_ImplGlfw_InitForVulkan(core_->Settings().window, true);

  VkAttachmentDescription color_attachment = {};
  color_attachment.format = image_->Format();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
  color_attachment.flags = 0;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  core_->Device()->CreateRenderPass({color_attachment}, {color_attachment_ref},
                                    &render_pass_);

  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = core_->Device()->Instance()->Handle();
  init_info.PhysicalDevice = core_->Device()->PhysicalDevice().Handle();
  init_info.Device = core_->Device()->Handle();
  init_info.QueueFamily = core_->GraphicsQueue()->QueueFamilyIndex();
  init_info.Queue = core_->GraphicsQueue()->Handle();
  init_info.DescriptorPool = descriptor_pool_->Handle();
  init_info.RenderPass = render_pass_->Handle();
  init_info.MinImageCount = 3;
  init_info.ImageCount = 3;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&init_info);

  render_pass_->CreateFramebuffer({image_->ImageView()}, image_->Extent(),
                                  &framebuffer_);
    core_->FrameSizeEvent().RegisterCallback(
      [this](uint32_t width, uint32_t height) {
        render_pass_->CreateFramebuffer({image_->ImageView()}, image_->Extent(),
                                        &framebuffer_);
      },
      0);

  auto &io = ImGui::GetIO();
  if (font_file_path) {
    io.Fonts->AddFontFromFileTTF(font_file_path, font_size, nullptr,
                                 io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->Build();
  } else {
    ImFontConfig im_font_config{};
    im_font_config.SizePixels = font_size;
    io.Fonts->AddFontDefault(&im_font_config);
  }

  ImGui_ImplVulkan_CreateFontsTexture();
}

void ImGuiManager::Render(VkCommandBuffer cmd_buffer) const {
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = render_pass_->Handle();
  renderPassInfo.framebuffer = framebuffer_->Handle();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = framebuffer_->Extent();
  renderPassInfo.clearValueCount = 0;
  renderPassInfo.pClearValues = nullptr;

  vkCmdBeginRenderPass(cmd_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);
  vkCmdEndRenderPass(cmd_buffer);
}

ImGuiManager::~ImGuiManager() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  framebuffer_.reset();
  render_pass_.reset();
  descriptor_pool_.reset();
}

void ImGuiManager::BeginFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
  ImGui::Render();
}

}  // namespace grassland::vulkan
