#include "function_derivative_test.h"

TEST(Physics, FunctionDerivativeElasticNeoHookean) {
  TestFunctionSet<ElasticNeoHookean<double>>();
}

TEST(Physics, FunctionDerivativeElasticNeoHookeanF3x2) {
  TestFunctionSet<ElasticNeoHookeanF3x2<double>>();
}

TEST(Physics, FunctionDerivativeElasticNeoHookeanTriangle) {
  Eigen::Matrix2<double> Dm;
  do {
    Dm = Eigen::Matrix2<double>::Random();
  } while (Dm.determinant() < 0);
  TestFunctionSet<ElasticNeoHookeanTriangle<double>>({1.0, 1.0, Dm});
}
