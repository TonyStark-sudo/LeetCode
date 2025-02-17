// Author: zhangsongpeng

#include "base/key_frame.h"

#include "base/pose.h"
#include "base/projection.h"

namespace colmap {
namespace {

static const double kNaN = std::numeric_limits<double>::quiet_NaN();

}  // namespace

KeyFrame::KeyFrame()
    : key_frame_id_(kInvalidKeyFrameId),
      name_(""),
      qvec_(1.0, 0.0, 0.0, 0.0),
      tvec_(0.0, 0.0, 0.0),
      qvec_prior_(kNaN, kNaN, kNaN, kNaN),
      tvec_prior_(kNaN, kNaN, kNaN) {}

void KeyFrame::NormalizeQvec() { qvec_ = NormalizeQuaternion(qvec_); }

Eigen::Matrix3x4d KeyFrame::ProjectionMatrix() const {
  return ComposeProjectionMatrix(qvec_, tvec_);
}

Eigen::Matrix3x4d KeyFrame::InverseProjectionMatrix() const {
  return InvertProjectionMatrix(ComposeProjectionMatrix(qvec_, tvec_));
}

Eigen::Matrix3d KeyFrame::RotationMatrix() const {
  return QuaternionToRotationMatrix(qvec_);
}

Eigen::Vector3d KeyFrame::ProjectionCenter() const {
  return ProjectionCenterFromPose(qvec_, tvec_);
}

}  // namespace colmap
