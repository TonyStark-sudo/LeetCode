// Copyright (c) 2023, ETH Zurich and UNC Chapel Hill.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of ETH Zurich and UNC Chapel Hill nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: Johannes L. Schoenberger (jsch-at-demuc-dot-de)

#ifndef COLMAP_SRC_BASE_COST_FUNCTIONS_H_
#define COLMAP_SRC_BASE_COST_FUNCTIONS_H_

#include <Eigen/Core>

#include <ceres/ceres.h>
#include <ceres/rotation.h>

namespace colmap {

// Standard bundle adjustment cost function for variable
// camera pose and calibration and point parameters.
template <typename CameraModel>
class BundleAdjustmentCostFunction {
 public:
  explicit BundleAdjustmentCostFunction(const Eigen::Vector2d& point2D)
      : observed_x_(point2D(0)), observed_y_(point2D(1)) {}

  static ceres::CostFunction* Create(const Eigen::Vector2d& point2D) {
    return (new ceres::AutoDiffCostFunction<
            BundleAdjustmentCostFunction<CameraModel>, 2, 4, 3, 3,
            CameraModel::kNumParams>(
        new BundleAdjustmentCostFunction(point2D)));
  }

  template <typename T>
  bool operator()(const T* const qvec, const T* const tvec,
                  const T* const point3D, const T* const camera_params,
                  T* residuals) const {
    // Rotate and translate.
    T projection[3];
    ceres::UnitQuaternionRotatePoint(qvec, point3D, projection);
    projection[0] += tvec[0];
    projection[1] += tvec[1];
    projection[2] += tvec[2];

    // Project to image plane.
    projection[0] /= projection[2];
    projection[1] /= projection[2];

    // Distort and transform to pixel space.
    CameraModel::WorldToImage(camera_params, projection[0], projection[1],
                              &residuals[0], &residuals[1]);

    // Re-projection error.
    residuals[0] -= T(observed_x_);
    residuals[1] -= T(observed_y_);

    return true;
  }

 private:
  const double observed_x_;
  const double observed_y_;
};

// Bundle adjustment cost function for variable
// camera calibration and point parameters, and fixed camera pose.
template <typename CameraModel>
class BundleAdjustmentConstantPoseCostFunction {
 public:
  BundleAdjustmentConstantPoseCostFunction(const Eigen::Vector4d& qvec,
                                           const Eigen::Vector3d& tvec,
                                           const Eigen::Vector2d& point2D)
      : qw_(qvec(0)),
        qx_(qvec(1)),
        qy_(qvec(2)),
        qz_(qvec(3)),
        tx_(tvec(0)),
        ty_(tvec(1)),
        tz_(tvec(2)),
        observed_x_(point2D(0)),
        observed_y_(point2D(1)) {}

  static ceres::CostFunction* Create(const Eigen::Vector4d& qvec,
                                     const Eigen::Vector3d& tvec,
                                     const Eigen::Vector2d& point2D) {
    return (new ceres::AutoDiffCostFunction<
            BundleAdjustmentConstantPoseCostFunction<CameraModel>, 2, 3,
            CameraModel::kNumParams>(
        new BundleAdjustmentConstantPoseCostFunction(qvec, tvec, point2D)));
  }

  template <typename T>
  bool operator()(const T* const point3D, const T* const camera_params,
                  T* residuals) const {
    const T qvec[4] = {T(qw_), T(qx_), T(qy_), T(qz_)};

    // Rotate and translate.
    T projection[3];
    ceres::UnitQuaternionRotatePoint(qvec, point3D, projection);
    projection[0] += T(tx_);
    projection[1] += T(ty_);
    projection[2] += T(tz_);

    // Project to image plane.
    projection[0] /= projection[2];
    projection[1] /= projection[2];

    // Distort and transform to pixel space.
    CameraModel::WorldToImage(camera_params, projection[0], projection[1],
                              &residuals[0], &residuals[1]);

    // Re-projection error.
    residuals[0] -= T(observed_x_);
    residuals[1] -= T(observed_y_);

    return true;
  }

 private:
  const double qw_;
  const double qx_;
  const double qy_;
  const double qz_;
  const double tx_;
  const double ty_;
  const double tz_;
  const double observed_x_;
  const double observed_y_;
};

// Rig bundle adjustment cost function for variable camera pose and calibration
// and point parameters. Different from the standard bundle adjustment function,
// this cost function is suitable for camera rigs with consistent relative poses
// of the cameras within the rig. The cost function first projects points into
// the local system of the camera rig and then into the local system of the
// camera within the rig.
template <typename CameraModel>
class RigBundleAdjustmentCostFunction {
 public:
  explicit RigBundleAdjustmentCostFunction(const Eigen::Vector2d& point2D)
      : observed_x_(point2D(0)), observed_y_(point2D(1)) {}

  static ceres::CostFunction* Create(const Eigen::Vector2d& point2D) {
    return (new ceres::AutoDiffCostFunction<
            RigBundleAdjustmentCostFunction<CameraModel>, 2, 4, 3, 4, 3, 3,
            CameraModel::kNumParams>(
        new RigBundleAdjustmentCostFunction(point2D)));
  }

  template <typename T>
  bool operator()(const T* const rig_qvec, const T* const rig_tvec,
                  const T* const rel_qvec, const T* const rel_tvec,
                  const T* const point3D, const T* const camera_params,
                  T* residuals) const {
    // Concatenate rotations.
    T qvec[4];
    ceres::QuaternionProduct(rel_qvec, rig_qvec, qvec);

    // Concatenate translations.
    T tvec[3];
    ceres::UnitQuaternionRotatePoint(rel_qvec, rig_tvec, tvec);
    tvec[0] += rel_tvec[0];
    tvec[1] += rel_tvec[1];
    tvec[2] += rel_tvec[2];

    // Rotate and translate.
    T projection[3];
    ceres::UnitQuaternionRotatePoint(qvec, point3D, projection);
    projection[0] += tvec[0];
    projection[1] += tvec[1];
    projection[2] += tvec[2];

    // Project to image plane.
    projection[0] /= projection[2];
    projection[1] /= projection[2];

    // Distort and transform to pixel space.
    CameraModel::WorldToImage(camera_params, projection[0], projection[1],
                              &residuals[0], &residuals[1]);

    // Re-projection error.
    residuals[0] -= T(observed_x_);
    residuals[1] -= T(observed_y_);

    return true;
  }

 private:
  const double observed_x_;
  const double observed_y_;
};

// Cost function for refining two-view geometry based on the Sampson-Error.
//
// First pose is assumed to be located at the origin with 0 rotation. Second
// pose is assumed to be on the unit sphere around the first pose, i.e. the
// pose of the second camera is parameterized by a 3D rotation and a
// 3D translation with unit norm. `tvec` is therefore over-parameterized as is
// and should be down-projected using `SphereManifold`.
class RelativePoseCostFunction {
 public:
  RelativePoseCostFunction(const Eigen::Vector2d& x1, const Eigen::Vector2d& x2)
      : x1_(x1(0)), y1_(x1(1)), x2_(x2(0)), y2_(x2(1)) {}

  static ceres::CostFunction* Create(const Eigen::Vector2d& x1,
                                     const Eigen::Vector2d& x2) {
    return (new ceres::AutoDiffCostFunction<RelativePoseCostFunction, 1, 4, 3>(
        new RelativePoseCostFunction(x1, x2)));
  }

  template <typename T>
  bool operator()(const T* const qvec, const T* const tvec,
                  T* residuals) const {
    Eigen::Matrix<T, 3, 3, Eigen::RowMajor> R;
    ceres::QuaternionToRotation(qvec, R.data());

    // Matrix representation of the cross product t x R.
    Eigen::Matrix<T, 3, 3> t_x;
    t_x << T(0), -tvec[2], tvec[1], tvec[2], T(0), -tvec[0], -tvec[1], tvec[0],
        T(0);

    // Essential matrix.
    const Eigen::Matrix<T, 3, 3> E = t_x * R;

    // Homogeneous image coordinates.
    const Eigen::Matrix<T, 3, 1> x1_h(T(x1_), T(y1_), T(1));
    const Eigen::Matrix<T, 3, 1> x2_h(T(x2_), T(y2_), T(1));

    // Squared sampson error.
    const Eigen::Matrix<T, 3, 1> Ex1 = E * x1_h;
    const Eigen::Matrix<T, 3, 1> Etx2 = E.transpose() * x2_h;
    const T x2tEx1 = x2_h.transpose() * Ex1;
    residuals[0] = x2tEx1 * x2tEx1 /
                   (Ex1(0) * Ex1(0) + Ex1(1) * Ex1(1) + Etx2(0) * Etx2(0) +
                    Etx2(1) * Etx2(1));

    return true;
  }

 private:
  const double x1_;
  const double y1_;
  const double x2_;
  const double y2_;
};

class PriorPoseCostFunction {
 public:
  PriorPoseCostFunction(const Eigen::Vector4d& qprior,
                        const Eigen::Vector3d& tprior,
                        const std::vector<double> &qweight,
                        const std::vector<double> &tweight)
      : qw_(qprior(0)), qx_(qprior(1)), qy_(qprior(2)), qz_(qprior(3)),
        tx_(tprior(0)), ty_(tprior(1)), tz_(tprior(2)),
        qweight_(qweight), tweight_(tweight) {}

  static ceres::CostFunction* Create(const Eigen::Vector4d& qprior,
                                     const Eigen::Vector3d& tprior,
                                     const std::vector<double> &qweight,
                                     const std::vector<double> &tweight) {
    return (new ceres::AutoDiffCostFunction<PriorPoseCostFunction, 6, 4, 3>(new PriorPoseCostFunction(qprior, tprior, qweight, tweight)));
  }

  template <typename T>
  bool operator()(const T* const qvec, const T* const tvec,
                  T* residuals) const {
    T qr[4];
    T tr[3];

    T qp_inv[4];
    T tp[3];

    qp_inv[0] = T(qw_);
    qp_inv[1] = T(-qx_);
    qp_inv[2] = T(-qy_);
    qp_inv[3] = T(-qz_);

    tp[0] = T(tx_);
    tp[1] = T(ty_);
    tp[2] = T(tz_);

    ceres::QuaternionProduct(qp_inv, qvec, qr);

    tr[0] = tvec[0] - tp[0];
    tr[1] = tvec[1] - tp[1];
    tr[2] = tvec[2] - tp[2];

    // Keep rotation in residuals
    residuals[0] = qr[1] * T(2.0 * qweight_[0]);
    residuals[1] = qr[2] * T(2.0 * qweight_[1]);
    residuals[2] = qr[3] * T(2.0 * qweight_[2]);
    residuals[3] = tr[0] * T(tweight_[0]);
    residuals[4] = tr[1] * T(tweight_[1]);
    residuals[5] = tr[2] * T(tweight_[2]);

    return true;
  }

 private:
  const double qw_;
  const double qx_;
  const double qy_;
  const double qz_;
  const double tx_;
  const double ty_;
  const double tz_;
  const std::vector<double> qweight_;
  const std::vector<double> tweight_;
};

class RelPoseCostFunction {
 public:
  RelPoseCostFunction(const Eigen::Vector4d& rqprior,
                        const Eigen::Vector3d& rtprior,
                        const std::vector<double> &rqweight,
                        const std::vector<double> &rtweight)
      : rqw_(rqprior(0)), rqx_(rqprior(1)), rqy_(rqprior(2)), rqz_(rqprior(3)),
        rtx_(rtprior(0)), rty_(rtprior(1)), rtz_(rtprior(2)),
        rqweight_(rqweight), rtweight_(rtweight) {}

  static ceres::CostFunction* Create(const Eigen::Vector4d& rqprior,
                                     const Eigen::Vector3d& rtprior,
                                     const std::vector<double> &rqweight,
                                     const std::vector<double> &rtweight) {
    return (new ceres::AutoDiffCostFunction<RelPoseCostFunction, 6, 4, 3, 4, 3>(new RelPoseCostFunction(rqprior, rtprior, rqweight, rtweight)));
  }

  template <typename T>
  bool operator()(const T* const qvec0, const T* const tvec0,
                  const T* const qvec1, const T* const tvec1,
                  T* residuals) const {
    Eigen::Quaternion<T> quat0{qvec0[0], qvec0[1], qvec0[2], qvec0[3]};
    Eigen::Quaternion<T> quat1{qvec1[0], qvec1[1], qvec1[2], qvec1[3]};
    Eigen::Matrix<T, 3, 1> t0{tvec0[0], tvec0[1], tvec0[2]};
    Eigen::Matrix<T, 3, 1> t1{tvec1[0], tvec1[1], tvec1[2]};
    Eigen::Quaternion<T> rq = quat0.conjugate() * quat1;
    Eigen::Matrix<T, 3, 1> rt = quat0.conjugate() * (t1 - t0);

    Eigen::Quaternion<T> dquat{T(rqw_), T(rqx_), T(rqy_), T(rqz_)};
    Eigen::Matrix<T, 3, 1> dtrans{T(rtx_), T(rty_), T(rtz_)};
    dtrans = dquat.conjugate() * (-dtrans);
    dquat = dquat.conjugate();
    dtrans = dtrans + dquat * rt;
    dquat = dquat * rq;

    residuals[0] = dquat.x() * T(2.0 * rqweight_[0]);
    residuals[1] = dquat.y() * T(2.0 * rqweight_[1]);
    residuals[2] = dquat.z() * T(2.0 * rqweight_[2]);
    residuals[3] = dtrans[0] * T(rtweight_[0]);
    residuals[4] = dtrans[1] * T(rtweight_[1]);
    residuals[5] = dtrans[2] * T(rtweight_[2]);

    return true;
  }

 private:
  const double rqw_;
  const double rqx_;
  const double rqy_;
  const double rqz_;
  const double rtx_;
  const double rty_;
  const double rtz_;
  const std::vector<double> rqweight_;
  const std::vector<double> rtweight_;
};

class PriorScalarCostFunction {
 public:
  PriorScalarCostFunction(const double sprior, const double sweight): sprior_(sprior), sweight_(sweight) {}

  static ceres::CostFunction* Create(const double sprior, const double sweight) {
    return (new ceres::AutoDiffCostFunction<PriorScalarCostFunction, 1, 1>(new PriorScalarCostFunction(sprior, sweight)));
  }

  template <typename T>
  bool operator()(const T* const s, T* residuals) const {
    residuals[0] = (s[0] - T(sprior_)) * T(sweight_);
    return true;
  }

 private:
  const double sprior_;
  const double sweight_;
};

class InterpolationCostFunction {
 public:
  InterpolationCostFunction(const double tb,
                            const double t0,
                            const double t1,
                            const std::vector<double> &qweight,
                            const std::vector<double> &tweight)
      : tb_(tb), t0_(t0), t1_(t1), qweight_(qweight), tweight_(tweight) {}

  static ceres::CostFunction* Create(const double tb,
                                     const double t0,
                                     const double t1,
                                     const std::vector<double> &qweight,
                                     const std::vector<double> &tweight) {
    return (new ceres::AutoDiffCostFunction<InterpolationCostFunction, 6, 4, 3, 1, 4, 3, 4, 3>(new InterpolationCostFunction(tb, t0, t1, qweight, tweight)));
  }

  template <typename T>
  bool operator()(const T* const qwb, const T* const twb,
                  const T* const dt,
                  const T* const qwb0, const T* const twb0,
                  const T* const qwb1, const T* const twb1,
                  T* residuals) const {
    T tb = T(tb_);
    T t0 = T(t0_);
    T t1 = T(t1_);
    T total_dt = t1 - t0;
    T s = (dt[0] + tb - t0) / total_dt;
    Eigen::Quaternion<T> quat_wb0{qwb0[0], qwb0[1], qwb0[2], qwb0[3]};
    Eigen::Quaternion<T> quat_wb1{qwb1[0], qwb1[1], qwb1[2], qwb1[3]};
    Eigen::Quaternion<T> quat_wbx = quat_wb0.slerp(s, quat_wb1);
    Eigen::Matrix<T, 3, 1> trans_wbx{(T(1.0) - s) * twb0[0] + T(s) * twb1[0], (T(1.0) - s) * twb0[1] + T(s) * twb1[1], (T(1.0) - s) * twb0[2] + T(s) * twb1[2]};

    Eigen::Quaternion<T> quat_wb{qwb[0], qwb[1], qwb[2], qwb[3]};
    Eigen::Matrix<T, 3, 1> trans_wb{twb[0], twb[1], twb[2]};

    Eigen::Quaternion<T> dquat = quat_wb.conjugate() * quat_wbx;
    Eigen::Matrix<T, 3, 1> dtrans = quat_wb.conjugate() * (trans_wbx - trans_wb);

    residuals[0] = dquat.x() * T(2.0 * qweight_[0]);
    residuals[1] = dquat.y() * T(2.0 * qweight_[1]);
    residuals[2] = dquat.z() * T(2.0 * qweight_[2]);
    residuals[3] = dtrans[0] * T(tweight_[0]);
    residuals[4] = dtrans[1] * T(tweight_[1]);
    residuals[5] = dtrans[2] * T(tweight_[2]);

    return true;
  }

 private:
  const double tb_;
  const double t0_;
  const double t1_;
  const std::vector<double> qweight_;
  const std::vector<double> tweight_;
};

class RigidTransformationCostFunction {
 public:
  RigidTransformationCostFunction(const std::vector<double> &qweight,
                                  const std::vector<double> &tweight)
      : qweight_(qweight), tweight_(tweight) {}

  static ceres::CostFunction* Create(const std::vector<double> &qweight,
                                     const std::vector<double> &tweight) {
    return (new ceres::AutoDiffCostFunction<RigidTransformationCostFunction, 6, 4, 3, 4, 3, 4, 3>(new RigidTransformationCostFunction(qweight, tweight)));
  }

  template <typename T>
  bool operator()(const T* const qbc, const T* const tbc,
                  const T* const qcw, const T* const tcw,
                  const T* const qwb, const T* const twb,
                  T* residuals) const {
    Eigen::Quaternion<T> quat_wb{qwb[0], qwb[1], qwb[2], qwb[3]};
    Eigen::Matrix<T, 3, 1> trans_wb{twb[0], twb[1], twb[2]};

    Eigen::Quaternion<T> quat_bc{qbc[0], qbc[1], qbc[2], qbc[3]};
    Eigen::Matrix<T, 3, 1> trans_bc{tbc[0], tbc[1], tbc[2]};
    Eigen::Quaternion<T> quat_cw{qcw[0], qcw[1], qcw[2], qcw[3]};
    Eigen::Matrix<T, 3, 1> trans_cw{tcw[0], tcw[1], tcw[2]};

    Eigen::Quaternion<T> quat_bw = quat_bc * quat_cw;
    Eigen::Matrix<T, 3, 1> trans_bw = quat_bc * trans_cw + trans_bc;
    Eigen::Quaternion<T> dquat = quat_bw * quat_wb;
    Eigen::Matrix<T, 3, 1> dtrans = trans_wb + quat_wb * trans_bw;

    residuals[0] = dquat.x() * T(2.0 * qweight_[0]);
    residuals[1] = dquat.y() * T(2.0 * qweight_[1]);
    residuals[2] = dquat.z() * T(2.0 * qweight_[2]);
    residuals[3] = dtrans[0] * T(tweight_[0]);
    residuals[4] = dtrans[1] * T(tweight_[1]);
    residuals[5] = dtrans[2] * T(tweight_[2]);

    return true;
  }

 private:
  const std::vector<double> qweight_;
  const std::vector<double> tweight_;
};

class AlignmentCostFunction {
 public:
  AlignmentCostFunction(const double tc,
                        const double t0,
                        const double t1,
                        const std::vector<double> &qweight,
                        const std::vector<double> &tweight)
      : tc_(tc), t0_(t0), t1_(t1), qweight_(qweight), tweight_(tweight) {}

  static ceres::CostFunction* Create(const double tc,
                                     const double t0,
                                     const double t1,
                                     const std::vector<double> &qweight,
                                     const std::vector<double> &tweight) {
    return (new ceres::AutoDiffCostFunction<AlignmentCostFunction, 6, 4, 3, 1, 4, 3, 4, 3, 4, 3>(new AlignmentCostFunction(tc, t0, t1, qweight, tweight)));
  }

  template <typename T>
  bool operator()(const T* const qbc, const T* const tbc,
                  const T* const dt,
                  const T* const qcw, const T* const tcw,
                  const T* const qwb0, const T* const twb0,
                  const T* const qwb1, const T* const twb1,
                  T* residuals) const {
    T tc = T(tc_);
    T t0 = T(t0_);
    T t1 = T(t1_);
    T total_dt = t1 - t0;
    T s = (dt[0] + tc - t0) / total_dt;
    Eigen::Quaternion<T> quat_wb0{qwb0[0], qwb0[1], qwb0[2], qwb0[3]};
    Eigen::Quaternion<T> quat_wb1{qwb1[0], qwb1[1], qwb1[2], qwb1[3]};
    Eigen::Quaternion<T> quat_wb = quat_wb0.slerp(s, quat_wb1);
    Eigen::Matrix<T, 3, 1> trans_wb{(T(1.0) - s) * twb0[0] + T(s) * twb1[0], (T(1.0) - s) * twb0[1] + T(s) * twb1[1], (T(1.0) - s) * twb0[2] + T(s) * twb1[2]};

    Eigen::Quaternion<T> quat_bc{qbc[0], qbc[1], qbc[2], qbc[3]};
    Eigen::Matrix<T, 3, 1> trans_bc{tbc[0], tbc[1], tbc[2]};
    Eigen::Quaternion<T> quat_cw{qcw[0], qcw[1], qcw[2], qcw[3]};
    Eigen::Matrix<T, 3, 1> trans_cw{tcw[0], tcw[1], tcw[2]};

    Eigen::Quaternion<T> quat_bw = quat_bc * quat_cw;
    Eigen::Matrix<T, 3, 1> trans_bw = quat_bc * trans_cw + trans_bc;
    Eigen::Quaternion<T> dquat = quat_bw * quat_wb;
    Eigen::Matrix<T, 3, 1> dtrans = trans_bw + quat_bw * trans_wb;

    residuals[0] = dquat.x() * T(2.0 * qweight_[0]);
    residuals[1] = dquat.y() * T(2.0 * qweight_[1]);
    residuals[2] = dquat.z() * T(2.0 * qweight_[2]);
    residuals[3] = dtrans[0] * T(tweight_[0]);
    residuals[4] = dtrans[1] * T(tweight_[1]);
    residuals[5] = dtrans[2] * T(tweight_[2]);

    return true;
  }

 private:
  const double tc_;
  const double t0_;
  const double t1_;
  const std::vector<double> qweight_;
  const std::vector<double> tweight_;
};

inline void SetQuaternionManifold(ceres::Problem* problem, double* qvec) {
#if CERES_VERSION_MAJOR >= 2 && CERES_VERSION_MINOR >= 1
  problem->SetManifold(qvec, new ceres::QuaternionManifold);
#else
  problem->SetParameterization(qvec, new ceres::QuaternionParameterization);
#endif
}

inline void SetSubsetManifold(int size, const std::vector<int>& constant_params,
                              ceres::Problem* problem, double* params) {
#if CERES_VERSION_MAJOR >= 2 && CERES_VERSION_MINOR >= 1
  problem->SetManifold(params,
                       new ceres::SubsetManifold(size, constant_params));
#else
  problem->SetParameterization(
      params, new ceres::SubsetParameterization(size, constant_params));
#endif
}

template <int size>
inline void SetSphereManifold(ceres::Problem* problem, double* params) {
#if CERES_VERSION_MAJOR >= 2 && CERES_VERSION_MINOR >= 1
  problem->SetManifold(params, new ceres::SphereManifold<size>);
#else
  problem->SetParameterization(
      params, new ceres::HomogeneousVectorParameterization(size));
#endif
}

}  // namespace colmap

#endif  // COLMAP_SRC_BASE_COST_FUNCTIONS_H_
