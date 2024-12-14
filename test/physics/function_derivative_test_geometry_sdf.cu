#include "function_derivative_test.h"

TEST(Physics, FunctionDerivativeSphereSDF) {
  SphereSDF<double> f;
  f.center = Eigen::Vector3d::Random();
  f.radius = 3.0;
  TestFunctionSet<SphereSDF<double>>(f);
}

TEST(Physics, FunctionDerivativeLineSDF) {
  for (int i = 0; i < 100; i++) {
    LineSDF<double> f;
    f.A = Eigen::Vector3d::Random();
    f.B = Eigen::Vector3d::Random();
    TestFunctionSet<LineSDF<double>>(f, 1);
  }
}

TEST(Physics, FunctionDerivativeCapsuleSDF) {
  for (int i = 0; i < 100; i++) {
    CapsuleSDF<double> f;
    f.A = Eigen::Vector3d::Random();
    f.B = Eigen::Vector3d::Random();
    f.radius = 3.0;
    TestFunctionSet<CapsuleSDF<double>>(f, 1);
  }
}

TEST(Physics, FunctionDerivativeCubeSDF) {
  for (int i = 0; i < 100; i++) {
    CubeSDF<double> f;
    f.center = {0.0, 0.0, 0.0};
    f.size = 0.1;
    TestFunctionSet<CubeSDF<double>>(f, 1);
  }
}
