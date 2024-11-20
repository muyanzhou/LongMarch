#include "function_derivative_test.h"

TEST(Physics, FunctionDerivativeElasticNeoHookean) {
  TestFunctionSet<ElasticNeoHookean<double>>();
}
