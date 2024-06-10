#pragma once
#include "grassland/data_structure/grid/linear_grid.h"
#include "grassland/data_structure/grid/linear_grid_view.h"
#include "thrust/device_vector.h"

namespace grassland::data_structure {
template <typename ContentType>
class LinearGridCUDA {
 public:
  LinearGridCUDA(size_t width,
                 size_t height,
                 const ContentType &default_value = ContentType{})
      : LinearGridCUDA(width, height, 1, default_value) {
  }

  LinearGridCUDA(size_t width,
                 size_t height,
                 size_t depth,
                 const ContentType &default_value = ContentType{})
      : width_(width),
        height_(height),
        depth_(depth),
        buffer_(width * height * depth, default_value),
        x_stride_(1),
        y_stride_(width),
        z_stride_(width * height) {
  }

  LinearGridCUDA(const LinearGrid<ContentType> &grid)
      : width_(grid.width()),
        height_(grid.height()),
        depth_(grid.depth()),
        buffer_(grid.buffer()),
        x_stride_(grid.x_stride()),
        y_stride_(grid.y_stride()),
        z_stride_(grid.z_stride()) {
  }

  ~LinearGridCUDA() = default;

  offset_t offset(offset_t x, offset_t y) const {
    return x * x_stride_ + y * y_stride_;
  }

  offset_t offset(offset_t x, offset_t y, offset_t z) const {
    return x * x_stride_ + y * y_stride_ + z * z_stride_;
  }

  size_t width() const {
    return width_;
  }

  size_t height() const {
    return height_;
  }

  size_t depth() const {
    return depth_;
  }

  ContentType *data() {
    return buffer_.data().get();
  }

  const ContentType *data() const {
    return buffer_.data().get();
  }

  thrust::device_vector<ContentType> &buffer() {
    return buffer_;
  }

  const thrust::device_vector<ContentType> &buffer() const {
    return buffer_;
  }

  LinearGridView<ContentType> view() {
    return {width_, height_, depth_, buffer_.data().get()};
  }

  operator LinearGridView<ContentType>() {
    return view();
  }

  LinearGrid<ContentType> to_host() const {
    LinearGrid<ContentType> grid(width_, height_, depth_);
    thrust::copy(buffer_.begin(), buffer_.end(), grid.buffer().begin());
    return grid;
  }

  operator LinearGrid<ContentType>() const {
    return to_host();
  }

 private:
  thrust::device_vector<ContentType> buffer_;
  size_t width_;
  size_t height_;
  size_t depth_;
  size_t x_stride_;
  size_t y_stride_;
  size_t z_stride_;
};
}  // namespace grassland::data_structure
