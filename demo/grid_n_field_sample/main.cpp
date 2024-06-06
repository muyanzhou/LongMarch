#include "long_march.h"
#include "random"

using namespace long_march;

double SphereSignedDistanceFunction(const geometry::Vector3<double> &pos,
                                    double radius = 1.0,
                                    const geometry::Vector3<double> &centre =
                                        geometry::Vector3<double>{0, 0, 0}) {
  return (pos - centre).norm() - radius;
}

double CubeSignedDistanceFunction(const geometry::Vector3<double> &pos,
                                  double size = 1.0,
                                  const geometry::Vector3<double> &centre =
                                      geometry::Vector3<double>{0, 0, 0}) {
  geometry::Vector3<double> q =
      (pos - centre).cwiseAbs() - geometry::Vector3<double>{size, size, size};
  if (q[0] < 0.0 && q[1] < 0.0 && q[2] < 0.0) {
    return std::max(q[0], std::max(q[1], q[2]));
  }
  q = q.cwiseMax(0.0);
  return q.norm();
}

double SignedDistanceFunction(const geometry::Vector3<double> &pos) {
  Eigen::AngleAxisd rot(0.3 * acos(-1),
                        geometry::Vector3<double>{1, 2, 3}.normalized());
  return CubeSignedDistanceFunction(rot.toRotationMatrix() * pos, 2.5);
  return SphereSignedDistanceFunction(pos, 5.0, {0, 0, 0});
}

int main() {
  geometry::Field<double, data_structure::LinearGrid<double>, double> field(
      201, 201, 201, 0.05, {-5.0, -5.0, -5.0}, 1);
  data_structure::LinearGridView<double> grid_view(field.grid());
  geometry::Field<double, decltype(grid_view), double> field_view(
      1.0, {-5.0, -5.0, -5.0}, grid_view);

  for (int i = 0; i < field.width(); i++) {
    for (int j = 0; j < field.height(); j++) {
      for (int k = 0; k < field.depth(); k++) {
        field(i, j, k) = SignedDistanceFunction(field.get_position(i, j, k));
      }
    }
  }

  //  for (int k = 0; k < field.depth(); k++) {
  //      for (int j = 0; j < field.height(); j++) {
  //          for (int i = 0; i < field.width(); i++) {
  //              std::cout << field(i, j, k) << " ";
  //          }
  //          std::cout << std::endl;
  //      }
  //      std::cout <<
  //      "======================================================================"
  //      << std::endl;
  //  }

  auto mesh = geometry::MarchingCubes(field, 0.0);
  mesh.GenerateNormals(-1.0);
  mesh.SaveObjFile("marching_cubes_sphere.obj");
  return 0;
}
