#include "grassland/physics/basic_functions.h"

namespace grassland {

template <typename Real>
struct SphereSDF {
  typedef Real Scalar;
  typedef Eigen::Vector<Real, 3> InputType;
  typedef Eigen::Matrix<Real, 1, 1> OutputType;

  LM_DEVICE_FUNC bool ValidInput(const InputType &v) const {
    return (v - center).norm() > algebra::Eps<Real>() * 100;
  }

  LM_DEVICE_FUNC OutputType operator()(const InputType &v) const {
    return OutputType{(v - center).norm() - radius};
  }

  LM_DEVICE_FUNC Eigen::
      Matrix<Real, OutputType::SizeAtCompileTime, InputType::SizeAtCompileTime>
      Jacobian(const InputType &v) const {
    return (v - center).normalized().transpose();
  }

  LM_DEVICE_FUNC HessianTensor<Real,
                               OutputType::SizeAtCompileTime,
                               InputType::SizeAtCompileTime>
  Hessian(const InputType &v) const {
    HessianTensor<Real, OutputType::SizeAtCompileTime,
                  InputType::SizeAtCompileTime>
        H;
    auto v_hat = (v - center).normalized().derived();
    H.m[0] =
        (H.m[0].Identity() - v_hat * v_hat.transpose()) / (v - center).norm();
    return H;
  }

  Eigen::Vector3<Real> center{0, 0, 0};
  Real radius{1.0};
};

template <typename Real>
struct LineSDF {
  typedef Real Scalar;
  typedef Eigen::Vector<Real, 3> InputType;
  typedef Eigen::Matrix<Real, 1, 1> OutputType;

  LM_DEVICE_FUNC bool ValidInput(const InputType &v) const {
    return operator()(v).value() > algebra::Eps<Real>() * 100;
  }

  LM_DEVICE_FUNC OutputType operator()(const InputType &v) const {
    Eigen::Vector3<Real> ab = B - A;
    Real t = (v - A).dot(ab) / ab.squaredNorm();
    t = max(Real(0), min(Real(1), t));
    return OutputType{(v - (A + t * ab)).norm()};
  }

  LM_DEVICE_FUNC Eigen::
      Matrix<Real, OutputType::SizeAtCompileTime, InputType::SizeAtCompileTime>
      Jacobian(const InputType &v) const {
    Eigen::Vector3<Real> ab = B - A;
    Real t = (v - A).dot(ab) / ab.squaredNorm();
    t = max(Real(0), min(Real(1), t));
    return (v - (A + t * ab)).normalized();
  }

  LM_DEVICE_FUNC HessianTensor<Real,
                               OutputType::SizeAtCompileTime,
                               InputType::SizeAtCompileTime>
  Hessian(const InputType &v) const {
    HessianTensor<Real, OutputType::SizeAtCompileTime,
                  InputType::SizeAtCompileTime>
        H;
    Eigen::Vector3<Real> ab = B - A;
    Real t = (v - A).dot(ab) / ab.squaredNorm();
    Eigen::Vector3<Real> center = A + max(Real(0), min(Real(1), t)) * ab;
    if (t < Real(0.0) || t > Real(1.0)) {
      auto v_hat = (v - center).normalized().derived();
      H.m[0] =
          (H.m[0].Identity() - v_hat * v_hat.transpose()) / (v - center).norm();
    } else {
      auto v_hat = (v - center).normalized().derived();
      auto ab_hat = ab.normalized().derived();
      H.m[0] = (H.m[0].Identity() - ab_hat * ab_hat.transpose() -
                v_hat * v_hat.transpose()) /
               (v - center).norm();
    }
    return H;
  }

  Eigen::Vector3<Real> A{0, 0, 0};
  Eigen::Vector3<Real> B{1, 1, 1};
};

template <typename Real>
struct CapsuleSDF {
  typedef Real Scalar;
  typedef Eigen::Vector<Real, 3> InputType;
  typedef Eigen::Matrix<Real, 1, 1> OutputType;

  LM_DEVICE_FUNC bool ValidInput(const InputType &v) const {
    return LineSDF<Real>{A, B}(v).value() > algebra::Eps<Real>() * 100;
  }

  LM_DEVICE_FUNC OutputType operator()(const InputType &v) const {
    Eigen::Vector3<Real> ab = B - A;
    Real t = (v - A).dot(ab) / ab.squaredNorm();
    t = max(Real(0), min(Real(1), t));
    return OutputType{(v - (A + t * ab)).norm() - radius};
  }

  LM_DEVICE_FUNC Eigen::
      Matrix<Real, OutputType::SizeAtCompileTime, InputType::SizeAtCompileTime>
      Jacobian(const InputType &v) const {
    Eigen::Vector3<Real> ab = B - A;
    Real t = (v - A).dot(ab) / ab.squaredNorm();
    t = max(Real(0), min(Real(1), t));
    return (v - (A + t * ab)).normalized();
  }

  LM_DEVICE_FUNC HessianTensor<Real,
                               OutputType::SizeAtCompileTime,
                               InputType::SizeAtCompileTime>
  Hessian(const InputType &v) const {
    HessianTensor<Real, OutputType::SizeAtCompileTime,
                  InputType::SizeAtCompileTime>
        H;
    Eigen::Vector3<Real> ab = B - A;
    Real t = (v - A).dot(ab) / ab.squaredNorm();
    Eigen::Vector3<Real> center = A + max(Real(0), min(Real(1), t)) * ab;
    if (t < Real(0.0) || t > Real(1.0)) {
      auto v_hat = (v - center).normalized().derived();
      H.m[0] =
          (H.m[0].Identity() - v_hat * v_hat.transpose()) / (v - center).norm();
    } else {
      auto v_hat = (v - center).normalized().derived();
      auto ab_hat = ab.normalized().derived();
      H.m[0] = (H.m[0].Identity() - ab_hat * ab_hat.transpose() -
                v_hat * v_hat.transpose()) /
               (v - center).norm();
    }
    return H;
  }

  Eigen::Vector3<Real> A{0, 0, 0};
  Eigen::Vector3<Real> B{1, 1, 1};
  Real radius{1.0};
};

template <typename Real>
struct CubeSDF {
  typedef Real Scalar;
  typedef Eigen::Vector<Real, 3> InputType;
  typedef Eigen::Matrix<Real, 1, 1> OutputType;

  LM_DEVICE_FUNC bool ValidInput(const InputType &p) const {
    return true;
  }

  LM_DEVICE_FUNC OutputType operator()(const InputType &p) const {
    Eigen::Vector3<Real> p0 = p - center;
    Eigen::Vector3<Real> q =
        p0.cwiseAbs() - Eigen::Vector3<Real>(size, size, size);
    if (q.maxCoeff() > 0) {
      q = q.cwiseMax(Eigen::Vector3<Real>(0, 0, 0));
      return OutputType{q.norm()};
    } else {
      return OutputType{q.maxCoeff()};
    }
  }

  LM_DEVICE_FUNC Eigen::
      Matrix<Real, OutputType::SizeAtCompileTime, InputType::SizeAtCompileTime>
      Jacobian(const InputType &p) const {
    Eigen::Vector3<Real> p0 = p - center;
    Eigen::Vector3<Real> q =
        p0.cwiseAbs() - Eigen::Vector3<Real>(size, size, size);
    if (q.maxCoeff() > 0) {
      q = q.cwiseMax(Eigen::Vector3<Real>(0, 0, 0));
      // multiply by sign to get the correct direction
      for (int i = 0; i < 3; i++) {
        if (p0(i) < 0) {
          q(i) = -q(i);
        }
      }
      return q.normalized().transpose();
    } else {
      // Find max component
      int max_idx = 0;
      for (int i = 1; i < 3; i++) {
        if (q(i) > q(max_idx)) {
          max_idx = i;
        }
      }
      Eigen::RowVector3<Real> result = Eigen::RowVector3<Real>::Zero();
      result(max_idx) = 1;
      if (p0(max_idx) < 0) {
        result(max_idx) = -1;
      }
      return result;
    }
  }

  LM_DEVICE_FUNC HessianTensor<Real,
                               OutputType::SizeAtCompileTime,
                               InputType::SizeAtCompileTime>
  Hessian(const InputType &v) const {
    HessianTensor<Real, OutputType::SizeAtCompileTime,
                  InputType::SizeAtCompileTime>
        H;
    Eigen::Vector3<Real> v_diff = v - center;
    Eigen::Vector3<Real> p = v_diff;
    H.m[0] = Eigen::Matrix3<Real>::Identity();
    for (int dim = 0; dim < 3; dim++) {
      if (v(dim) < -size) {
        p(dim) = -size;
      } else if (v(dim) > size) {
        p(dim) = size;
      } else {
        p(dim) = v_diff(dim);
        H.m[0](dim, dim) = 0;
      }
    }
    v_diff = v_diff - p;
    Real v_diff_norm = v_diff.norm();
    if (v_diff_norm > algebra::Eps<Real>()) {
      v_diff = v_diff / v_diff_norm;
      H.m[0] = (H.m[0] - v_diff * v_diff.transpose()) / v_diff_norm;
      return H;
    } else {
      return {};
    }
  }

  Eigen::Vector3<Real> center{0, 0, 0};
  Real size{1.0};
};

}  // namespace grassland
