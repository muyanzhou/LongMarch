#include "cmath"
#include "iostream"
#include "long_march.h"
#include "thrust/device_vector.h"
#include "thrust/host_vector.h"

using namespace long_march;

using real = float;

int main() {
  std::vector<geometry::Vector3<real>> positions = {
      {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
      {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1},
  };
  std::vector<uint32_t> indices = {
      0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4,
      1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 0, 4, 3, 4, 7,
  };
  geometry::Mesh<real> mesh(positions.size(), indices.size(), indices.data(),
                            positions.data());
  mesh.SplitVertices();
  mesh.MergeVertices();
  mesh.GenerateNormals(0);
  mesh.SaveObjFile("cube.obj");
}
