#include "iostream"
#include "long_march.h"
#include "thrust/device_vector.h"
#include "thrust/host_vector.h"

using namespace long_march;

__global__ void kernel(geometry::Triangle3d *triangles) {
  geometry::Triangle<double, 3> triangle;
  triangle[0] << 1, 0, 0;
  triangle[1] << 0, 1, 0;
  triangle[2] << 0, 0, 1;
  triangles[0] = triangle;
  auto v = triangle.normal();
  printf("%f %f %f\n", v[0], v[1], v[2]);
}

int main() {
  thrust::device_vector<geometry::Triangle3d> triangles(1);
  kernel<<<1, 1>>>(thrust::raw_pointer_cast(triangles.data()));
  cudaDeviceSynchronize();
  thrust::host_vector<geometry::Triangle3d> h_triangles = triangles;
  std::cout << h_triangles[0].m << std::endl;
}
