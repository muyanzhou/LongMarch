#include "grassland/d3d12/buffer.h"

namespace grassland::d3d12 {

Buffer::Buffer(const ComPtr<ID3D12Resource> &buffer) : buffer_(buffer) {
}

void *Buffer::Map() const {
  void *data;
  buffer_->Map(0, nullptr, &data);
  return data;
}

void Buffer::Unmap() const {
  buffer_->Unmap(0, nullptr);
}

void CopyBuffer(ID3D12GraphicsCommandList *command_list,
                Buffer *src_buffer,
                Buffer *dst_buffer,
                size_t size,
                size_t src_offset,
                size_t dst_offset) {
  CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      dst_buffer->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ,
      D3D12_RESOURCE_STATE_COPY_DEST);

  command_list->ResourceBarrier(1, &barrier);

  command_list->CopyBufferRegion(dst_buffer->Handle(), dst_offset,
                                 src_buffer->Handle(), src_offset, size);

  barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      dst_buffer->Handle(), D3D12_RESOURCE_STATE_COPY_DEST,
      D3D12_RESOURCE_STATE_GENERIC_READ);

  command_list->ResourceBarrier(1, &barrier);
}

}  // namespace grassland::d3d12
