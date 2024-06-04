#pragma once
#include "grassland/geometry/geometry_utils.h"

namespace grassland::geometry {
template <typename Scalar, int dim>
struct AxisAlignedBoundingBox;

template <typename Scalar>
struct AxisAlignedBoundingBox<Scalar, 3> {
  Vector3<Scalar> min_bound;
  Vector3<Scalar> max_bound;
};

template <typename Scalar>
struct AxisAlignedBoundingBox<Scalar, 2> {
  Vector2<Scalar> min_bound;
  Vector2<Scalar> max_bound;
};

template <typename Scalar>
using AxisAlignedBoundingBox2 = AxisAlignedBoundingBox<Scalar, 2>;
template <typename Scalar>
using AxisAlignedBoundingBox3 = AxisAlignedBoundingBox<Scalar, 3>;

using AxisAlignedBoundingBox2f = AxisAlignedBoundingBox2<float>;
using AxisAlignedBoundingBox2d = AxisAlignedBoundingBox2<double>;

using AxisAlignedBoundingBox3f = AxisAlignedBoundingBox3<float>;
using AxisAlignedBoundingBox3d = AxisAlignedBoundingBox3<double>;

template <typename Scalar, int dim>
using AABB = AxisAlignedBoundingBox<Scalar, dim>;
template <typename Scalar>
using AABB2 = AxisAlignedBoundingBox<Scalar, 2>;
template <typename Scalar>
using AABB3 = AxisAlignedBoundingBox<Scalar, 3>;

using AABB2f = AABB2<float>;
using AABB2d = AABB2<double>;

using AABB3f = AABB3<float>;
using AABB3d = AABB3<double>;
}  // namespace grassland::geometry
