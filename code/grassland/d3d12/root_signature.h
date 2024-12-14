#pragma once
#include "grassland/d3d12/device.h"

namespace grassland::d3d12 {

class RootSignature {
 public:
  RootSignature(const ComPtr<ID3D12RootSignature> &root_signature);

  ID3D12RootSignature *Handle() const {
    return root_signature_.Get();
  }

 private:
  ComPtr<ID3D12RootSignature> root_signature_;
};

}  // namespace grassland::d3d12
