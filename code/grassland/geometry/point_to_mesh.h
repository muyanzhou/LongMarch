#pragma once
#include "grassland/geometry/axis_aligned_bounding_box.h"
#include "grassland/geometry/marching_cubes.h"

namespace grassland::geometry {
template <typename Scalar>
Mesh<Scalar> PointToMesh(const Vector3<Scalar> *points,
                         size_t n_points,
                         Scalar radius,
                         size_t n_subdivisions = 1,
                         Scalar margin = 2.0,
                         int range = 2) {
  geometry::AxisAlignedBoundingBox3<Scalar> aabb;

  for (size_t i = 0; i < n_points; ++i) {
    aabb.Expand(points[i]);
  }

  aabb.min_bound -= Vector3<Scalar>{radius, radius, radius} * margin;
  aabb.max_bound += Vector3<Scalar>{radius, radius, radius} * margin;

  Vector3<Scalar> grid_size = (aabb.Size() / radius) * n_subdivisions;

  size_t width = static_cast<size_t>(std::ceil(grid_size[0]));
  size_t height = static_cast<size_t>(std::ceil(grid_size[1]));
  size_t depth = static_cast<size_t>(std::ceil(grid_size[2]));

  geometry::Field<Scalar, Scalar> field(
      width, height, depth, radius / n_subdivisions, aabb.min_bound, 1.0);

  for (size_t i = 0; i < n_points; ++i) {
    Vector3<Scalar> grid_pos = field.to_grid_position(points[i]);
    offset_t x = static_cast<offset_t>(std::floor(grid_pos[0]));
    offset_t y = static_cast<offset_t>(std::floor(grid_pos[1]));
    offset_t z = static_cast<offset_t>(std::floor(grid_pos[2]));
    offset_t low_x = x - n_subdivisions * range;
    offset_t low_y = y - n_subdivisions * range;
    offset_t low_z = z - n_subdivisions * range;
    offset_t high_x = x + n_subdivisions * range + 1;
    offset_t high_y = y + n_subdivisions * range + 1;
    offset_t high_z = z + n_subdivisions * range + 1;
    low_x = std::max(low_x, offset_t{0});
    low_y = std::max(low_y, offset_t{0});
    low_z = std::max(low_z, offset_t{0});
    high_x = std::min(high_x, static_cast<offset_t>(field.width()));
    high_y = std::min(high_y, static_cast<offset_t>(field.height()));
    high_z = std::min(high_z, static_cast<offset_t>(field.depth()));

    auto kernel_function = [](Scalar distance) -> Scalar {
      if (distance < 1.0) {
        return 2.0 - distance * distance;
      } else if (distance < 2.0) {
        return (2.0 - distance) * (2.0 - distance);
      }
      return 0.0;
    };

    for (offset_t x = low_x; x < high_x; ++x) {
      for (offset_t y = low_y; y < high_y; ++y) {
        for (offset_t z = low_z; z < high_z; ++z) {
          Vector3<Scalar> grid_pos{Scalar(x), Scalar(y), Scalar(z)};
          Vector3<Scalar> world_pos = field.to_world_position(grid_pos);
          Scalar distance = (world_pos - points[i]).norm() / radius;
          if (distance < range) {
            field(x, y, z) -= kernel_function(distance);
          }
        }
      }
    }
  }

  Mesh<Scalar> mesh = MarchingCubes<Scalar, Scalar>(field);
  return mesh;
}
}  // namespace grassland::geometry
