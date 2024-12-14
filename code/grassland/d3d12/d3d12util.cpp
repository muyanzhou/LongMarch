#include "grassland/d3d12/d3d12util.h"

namespace grassland::d3d12 {

std::string HRESULTToString(HRESULT hr) {
  char buffer[256] = {};
  sprintf(buffer, "HRESULT: 0x%08X", hr);
  return buffer;
}

size_t SizeByFormat(DXGI_FORMAT format) {
  switch (format) {
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_SNORM:
      return 1;

    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_SNORM:
      return 2;

    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
      return 4;

    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
      return 4;

    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_TYPELESS:
      return 8;

    case DXGI_FORMAT_R32G32B32_SINT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_TYPELESS:
      return 12;

    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
      return 16;

    default:
      return 0;
  }
}

}  // namespace grassland::d3d12
