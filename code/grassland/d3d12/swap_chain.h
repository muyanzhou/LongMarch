#pragma once
#include "grassland/d3d12/descriptor_heap.h"
#include "grassland/d3d12/dxgi_factory.h"

namespace grassland::d3d12 {
class SwapChain {
 public:
  SwapChain(const ComPtr<IDXGISwapChain3> &swap_chain);

  IDXGISwapChain3 *Handle() const {
    return swap_chain_.Get();
  }

  ID3D12Resource *BackBuffer(uint32_t index) const {
    return back_buffers_[index].Get();
  }

  CD3DX12_CPU_DESCRIPTOR_HANDLE RTVCPUHandle(uint32_t index) const {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        rtv_heap_->GetCPUDescriptorHandleForHeapStart(), index,
        rtv_descriptor_size_);
  }

  CD3DX12_GPU_DESCRIPTOR_HANDLE RTVGPUHandle(uint32_t index) const {
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(
        rtv_heap_->GetGPUDescriptorHandleForHeapStart(), index,
        rtv_descriptor_size_);
  }

 private:
  ComPtr<IDXGISwapChain3> swap_chain_;
  ComPtr<ID3D12DescriptorHeap> rtv_heap_;
  uint32_t rtv_descriptor_size_{};
  std::vector<ComPtr<ID3D12Resource>> back_buffers_;
};
}  // namespace grassland::d3d12
