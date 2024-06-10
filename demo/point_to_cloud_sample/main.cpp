#include "long_march.h"
#include "random"

using namespace long_march;

bool Inside(const geometry::Vector3<float> &v) {
  return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] < 1.0;
}

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::vector<geometry::Vector3<float>> points;
  for (int i = 0; i < 1000000; ++i) {
    std::uniform_real_distribution<float> dis(-1.0, 1.0);
    auto p = geometry::Vector3<float>{dis(gen), dis(gen), dis(gen)};
    if (Inside(p)) {
      points.push_back(p);
    }
  }

  auto mesh = geometry::PointToMesh(points.data(), points.size(), 0.025f, 5);
  mesh.InitializeTexCoords();
  mesh.GenerateNormals();
  mesh.GenerateTangents();
  mesh.SaveObjFile("sphere.obj");

  return 0;
}
