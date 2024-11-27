#pragma once
#include "fem_elements.h"
#include "grassland/physics/basic_functions.h"

namespace grassland {
template <typename Real>
struct ElasticNeoHookean {
  typedef Real Scalar;
  typedef Eigen::Matrix<Real, 3, 3> InputType;
  typedef Eigen::Matrix<Real, 1, 1> OutputType;

  LM_DEVICE_FUNC bool ValidInput(const InputType &F) const {
    return F.determinant() > 0;
  }

  LM_DEVICE_FUNC OutputType operator()(const InputType &F) const {
    Determinant3<Real> det3;
    auto J = det3(F).value();
    auto log_J = log(J);
    Eigen::Map<const Eigen::Vector<Real, 9>> F_map(F.data());
    auto I2 = F_map.dot(F_map);
    OutputType res{0.5 * mu * (I2 - 3.0) - mu * log_J +
                   0.5 * lambda * log_J * log_J};
    return res;
  }

  LM_DEVICE_FUNC Eigen::
      Matrix<Real, OutputType::SizeAtCompileTime, InputType::SizeAtCompileTime>
      Jacobian(const InputType &F) const {
    LogDeterminant3<Real> log_J;
    LogSquareDeterminant3<Real> log_2_J;
    Eigen::Map<const Eigen::RowVector<Real, 9>> F_map(F.data());
    return mu * (F_map - log_J.Jacobian(F)) +
           0.5 * lambda * log_2_J.Jacobian(F);
  }

  LM_DEVICE_FUNC HessianTensor<Real,
                               OutputType::SizeAtCompileTime,
                               InputType::SizeAtCompileTime>
  Hessian(const InputType &F) const {
    HessianTensor<Real, OutputType::SizeAtCompileTime,
                  InputType::SizeAtCompileTime>
        H;
    LogDeterminant3<Real> log_J;
    LogSquareDeterminant3<Real> log_2_J;
    H.m[0] =
        mu * (Eigen::Matrix<Real, 9, 9>::Identity() - log_J.Hessian(F).m[0]) +
        0.5 * lambda * log_2_J.Hessian(F).m[0];
    return H;
  }

  Real mu{1.0};
  Real lambda{1.0};
};

template <typename Real>
struct ElasticNeoHookeanF3x2 {
  typedef Real Scalar;
  typedef Eigen::Matrix<Real, 3, 2> InputType;
  typedef Eigen::Matrix<Real, 1, 1> OutputType;

  LM_DEVICE_FUNC bool ValidInput(const InputType &F) const {
    return F.col(0).cross(F.col(1)).norm() > algebra::Eps<Real>() * 100;
  }

  LM_DEVICE_FUNC OutputType operator()(const InputType &F) const {
    CrossNorm<Real> cross_norm;
    auto J = cross_norm(F).value();
    auto log_J = log(J);
    Eigen::Map<const Eigen::Vector<Real, 6>> F_map(F.data());
    auto I2 = F_map.dot(F_map) + Real(1.0);
    OutputType res{0.5 * mu * (I2 - 3.0) - mu * log_J +
                   0.5 * lambda * log_J * log_J};
    return res;
  }

  LM_DEVICE_FUNC Eigen::
      Matrix<Real, OutputType::SizeAtCompileTime, InputType::SizeAtCompileTime>
      Jacobian(const InputType &F) const {
    CrossNorm<Real> cross_norm;
    auto J = cross_norm(F).value();
    auto log_J = log(J);
    Eigen::Map<const Eigen::Vector<Real, 6>> F_map(F.data());
    auto inv_J = 1.0 / J;
    auto cross_norm_J = cross_norm.Jacobian(F);
    return mu * (F_map.transpose() - inv_J * cross_norm_J) +
           0.5 * lambda * (2.0 * log_J * inv_J * cross_norm_J);
  }

  LM_DEVICE_FUNC HessianTensor<Real,
                               OutputType::SizeAtCompileTime,
                               InputType::SizeAtCompileTime>
  Hessian(const InputType &F) const {
    HessianTensor<Real, OutputType::SizeAtCompileTime,
                  InputType::SizeAtCompileTime>
        H;
    CrossNorm<Real> cross_norm;
    auto J = cross_norm(F).value();
    auto log_J = log(J);
    Eigen::Map<const Eigen::Vector<Real, 6>> F_map(F.data());
    auto inv_J = 1.0 / J;
    auto inv_J2 = inv_J * inv_J;
    auto cross_norm_J = cross_norm.Jacobian(F);
    auto cross_norm_H = cross_norm.Hessian(F);
    H.m[0] = mu * (Eigen::Matrix<Real, 6, 6>::Identity() -
                   (inv_J * cross_norm_H.m[0] -
                    inv_J2 * cross_norm_J.transpose() * cross_norm_J)) +
             0.5 * lambda *
                 (2.0 * log_J * inv_J * cross_norm_H.m[0] +
                  (2.0 * inv_J2 * (1.0 - log_J)) * cross_norm_J.transpose() *
                      cross_norm_J);
    return H;
  }

  Real mu{1.0};
  Real lambda{1.0};
};

template <typename Real>
struct ElasticNeoHookeanTriangle {
  typedef Real Scalar;
  typedef Eigen::Matrix<Real, 3, 3> InputType;
  typedef Eigen::Matrix<Real, 1, 1> OutputType;

  LM_DEVICE_FUNC bool ValidInput(const InputType &V) const {
    FEMTriangleDeformationGradient3x2<Real> deformation_gradient3x2{Dm};
    ElasticNeoHookeanF3x2<Real> neo_hookean_f3x2{mu, lambda};
    return deformation_gradient3x2.ValidInput(V) &&
           neo_hookean_f3x2.ValidInput(deformation_gradient3x2(V));
  }

  LM_DEVICE_FUNC OutputType operator()(const InputType &V) const {
    FEMTriangleDeformationGradient3x2<Real> deformation_gradient3x2{Dm};
    ElasticNeoHookeanF3x2<Real> neo_hookean_f3x2{mu, lambda};
    return neo_hookean_f3x2(deformation_gradient3x2(V));
  }

  LM_DEVICE_FUNC Eigen::
      Matrix<Real, OutputType::SizeAtCompileTime, InputType::SizeAtCompileTime>
      Jacobian(const InputType &V) const {
    FEMTriangleDeformationGradient3x2<Real> deformation_gradient3x2{Dm};
    ElasticNeoHookeanF3x2<Real> neo_hookean_f3x2{mu, lambda};
    return neo_hookean_f3x2.Jacobian(deformation_gradient3x2(V)) *
           deformation_gradient3x2.Jacobian(V);
  }

  LM_DEVICE_FUNC HessianTensor<Real,
                               OutputType::SizeAtCompileTime,
                               InputType::SizeAtCompileTime>
  Hessian(const InputType &F) const {
    FEMTriangleDeformationGradient3x2<Real> deformation_gradient3x2{Dm};
    ElasticNeoHookeanF3x2<Real> neo_hookean_f3x2{mu, lambda};
    return neo_hookean_f3x2.Hessian(deformation_gradient3x2(F)) *
           deformation_gradient3x2.Jacobian(F);
  }

  Real mu{1.0};
  Real lambda{1.0};
  Eigen::Matrix2<Real> Dm;
};

}  // namespace grassland
