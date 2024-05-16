#pragma once

#include "grassland/utils/double_ptr.h"
#include "grassland/utils/log.h"

namespace grassland {
#if defined(__CUDACC__)
#define LM_DEVICE_FUNC __device__ __host__
#else
#define LM_DEVICE_FUNC
#endif
}  // namespace grassland
